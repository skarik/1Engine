
#include "COctreeTerrain.h"
#include "CGameSettings.h"
#include <memory>

uint32_t COctreeTerrain::sectorMemorySize = sizeof(Terrain::Sector);

bool COctreeTerrain::InitTerraMemory ( void )
{
	m_memoryMaxBlocks = (uint32_t)( 64 * 255 * pow(2,1+CGameSettings::Active()->i_cl_ter_Range) );
	std::cout << "   Block size: " << sectorMemorySize << " with " << m_memoryMaxBlocks << " blocks." << std::endl;
	std::cout << "   Estimated size: " << ((sectorMemorySize+1) * m_memoryMaxBlocks)/1048576 << " MB" << std::endl;

	try
	{
		// Terrain Memory Management	
		uint32_t blocks = m_memoryMaxBlocks;
		// Attempt to allocate memory for the terrain
		m_memoryData =  new char [ blocks * sectorMemorySize ];
		memset( m_memoryData, 0, blocks * sectorMemorySize );
		// Allocate memory for the terra mem flags
		m_memoryUsage = new char [ blocks ];
		memset( m_memoryUsage, 0, blocks );

		// Set the last four blocks as used
		m_memoryUsage[blocks-1] = 1;
		m_memoryUsage[blocks-2] = 1;
		m_memoryUsage[blocks-3] = 1;
		m_memoryUsage[blocks-4] = 1;
	}
	catch ( std::bad_alloc )
	{
		return false;
	}
	return true;
}

void COctreeTerrain::FreeTerraMemory ( void )
{
	delete [] m_memoryData;
		m_memoryData = NULL;
	delete [] m_memoryUsage;
		m_memoryUsage = NULL;
}

void COctreeTerrain::NewDataBlock  ( Terrain::Sector** block )
{
	mutex::scoped_lock lock ( m_memoryLock );

	uint32_t blocks = m_memoryMaxBlocks;
	uint32_t normalized_index = 0;

	// Find the first open index
	while ( m_memoryUsage[normalized_index] != 0 )
		++normalized_index;

	if ( normalized_index >= blocks )
	{
		std::cout << "OH NO MY BOBA BALLS!" << std::endl;
		MessageBox( NULL, "Ran out of memory in the terrain segment.", "Error", 0 );
		exit(0);
	}

	// Get the new memory index
	uint32_t index = normalized_index * sectorMemorySize;

	// Set the block to the new value
	//(*block) = (subblock16*)(pTerraData + index);
	//(*block) = (Terrain::Sector*)(&(m_memoryData[index])); // should be the same
	(*block) = new(&(m_memoryData[index])) Terrain::Sector; //should still work

	// Turn on flag
	m_memoryUsage[normalized_index] = 1;
}
void COctreeTerrain::FreeDataBlock ( Terrain::Sector* block )
{
	mutex::scoped_lock lock ( m_memoryLock );

	uint32_t blocks = m_memoryMaxBlocks;
	uint32_t index = 0;

	// Figure out the index of block based on the memory address
	char* tblock = (char*)block;
	while ( tblock != m_memoryData + index )
		index += sectorMemorySize;

	// Get the index of the mem flag array
	uint32_t normalized_index = index / sectorMemorySize;

	if ( normalized_index >= blocks )
		exit(0);

	// Turn off flag
	m_memoryUsage[normalized_index] = 0;
}

Real COctreeTerrain::GetMemoryUsage ( void )
{
	mutex::scoped_lock lock ( m_memoryLock );

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
	boost::thread* threads[4];
	for ( uint i = 0; i < 4; ++i ) {
		threads[i] = new thread( counters[i] );
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
