
#include "MemoryManager.h"

#include "core/settings/CGameSettings.h"

#include "after/terrain/data/Node.h"

#include <iostream>
#include <memory>
#include <thread>

//===============================================================================================//
// Manager settings
//===============================================================================================//

uint32_t Terrain::MemoryManager::MemorySize_Sidebuffer	= sizeof(Terrain::terra_b) * 34*34*34;
uint32_t Terrain::MemoryManager::MemorySize_Payload		= sizeof(Terrain::terra_b) * BLOCK_COUNT;
uint32_t Terrain::MemoryManager::MemorySize_Node		= sizeof(Terrain::Node);

//===============================================================================================//
// Constructor and Destructor
//===============================================================================================//

Terrain::MemoryManager::MemoryManager ( void )
{
	if ( !Init() )
	{
		printf( "Could not allocate enough memory for terrain. Currently at density %d.", CGameSettings::Active()->i_cl_ter_Range );
		throw Core::OutOfMemoryException();
	}
}
Terrain::MemoryManager::~MemoryManager( void )
{
	Free();
}

//===============================================================================================//
// System Start and End
//===============================================================================================//

bool Terrain::MemoryManager::Init ( void )
{
	//m_memoryMaxBlocks = (uint32_t)( 64 * 255 * pow(2,1+CGameSettings::Active()->i_cl_ter_Range) );
	m_memoryMaxBlocks = (uint32_t)( 64 * pow(2,1+CGameSettings::Active()->i_cl_ter_Range) );
	std::cout << "   Block size: " << MemorySize_Payload << " with " << m_memoryMaxBlocks << " blocks." << std::endl;
	std::cout << "   Estimated size: " << ((MemorySize_Payload+1) * m_memoryMaxBlocks)/(1024*1024) << " MB" << std::endl;

	try
	{
		// Terrain Memory Management	
		uint32_t blocks = m_memoryMaxBlocks;
		// Attempt to allocate memory for the terrain
		m_memoryData =  new char [ blocks * MemorySize_Payload ];
		memset( m_memoryData, 0, blocks * MemorySize_Payload );
		// Allocate memory for the terra mem flags
		m_memoryUsage = new char [ blocks ];
		memset( m_memoryUsage, 0, blocks );

		// Set the last eight blocks as used
		for ( int i = 1; i <= 8; ++i ) {
			m_memoryUsage[blocks-i] = 1;
		}
	}
	catch ( std::bad_alloc )
	{
		return false;
	}
	return true;
}

void Terrain::MemoryManager::Free ( void )
{
	delete [] m_memoryData;
		m_memoryData = NULL;
	delete [] m_memoryUsage;
		m_memoryUsage = NULL;
}

//===============================================================================================//
// Memory Requests
//===============================================================================================//

Terrain::terra_b*	Terrain::MemoryManager::NewDataBlock  ( void )
//void Terrain::MemoryManager::NewDataBlock  ( Terrain::Payload** block )
{
	std::lock_guard<std::mutex> lock ( m_memoryLock );

	uint32_t blocks = m_memoryMaxBlocks;
	uint32_t normalized_index = 0;

	// Find the first open index
	while ( m_memoryUsage[normalized_index] != 0 )
		++normalized_index;

	if ( normalized_index >= blocks )
	{
		std::cout << "OH NO MY BOBA BALLS!" << std::endl;
		throw Core::OutOfMemoryException();
	}

	// Get the new memory index
	uint32_t index = normalized_index * MemorySize_Payload;

	// Set the block to the new value
	//(*block) = (subblock16*)(pTerraData + index);
	//(*block) = (Terrain::Sector*)(&(m_memoryData[index])); // should be the same
	//(*block) = new(&(m_memoryData[index])) Terrain::Sector; //should still work
	terra_b* block = (terra_b*) &(m_memoryData[index]);

	// Turn on flag
	m_memoryUsage[normalized_index] = 1;

	// Return allocated area
	return block;
}
void Terrain::MemoryManager::FreeDataBlock ( terra_b* block )
{
	std::lock_guard<std::mutex> lock ( m_memoryLock );

	uint32_t blocks = m_memoryMaxBlocks;
	uint32_t index = 0;

	// Figure out the index of block based on the memory address
	char* tblock = (char*)block;
	while ( tblock != m_memoryData + index )
		index += MemorySize_Payload;

	// Get the index of the mem flag array
	uint32_t normalized_index = index / MemorySize_Payload;

	if ( normalized_index >= blocks )
		exit(0);

	// Turn off flag
	m_memoryUsage[normalized_index] = 0;
}


//===============================================================================================//
// System State Query
//===============================================================================================//

Real Terrain::MemoryManager::GetMemoryUsage ( void )
{
	std::lock_guard<std::mutex> lock ( m_memoryLock );

	struct tBlockCounter {
		char* m_memoryUsage;
		uint32_t* count;
		uint32_t start;
		uint32_t end;

		void operator () ( void ) {
			for ( uint32_t i = start; i < end; ++i ) {
				if ( m_memoryUsage[i] ) {
					++(*count);
				}
			}
		}
	};

	// Create 4 counters
	uint32_t external_count[4];
	tBlockCounter counters [4];
	for ( uint i = 0; i < 4; ++i ) {
		external_count[i] = 0;
		counters[i].m_memoryUsage = m_memoryUsage;
		counters[i].count = &(external_count[i]);
		counters[i].start	= i * (m_memoryMaxBlocks/4);
		counters[i].end		= (i+1) * (m_memoryMaxBlocks/4);
	}
	// And run the four counters on four separate threads
	std::thread* threads[4];
	for ( uint i = 0; i < 4; ++i ) {
		threads[i] = new std::thread( counters[i] );
	}
	for ( uint i = 0; i < 4; ++i ) {
		threads[i]->join();
		delete threads[i];
	}

	// Count up the blocks the threads counted up
	uint32_t usedBlocks = 0;
	for ( uint i = 0; i < 4; ++i ) {
		usedBlocks += external_count[i];
	}

	// Return the percentage of used blocks
	return usedBlocks/((Real)(m_memoryMaxBlocks));
}
