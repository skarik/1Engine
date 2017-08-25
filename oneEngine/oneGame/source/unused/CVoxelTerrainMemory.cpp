
#include "CVoxelTerrain.h"

using std::cout;
using std::endl;

bool CVoxelTerrain::InitTerraMemory ( void )
{
	if ( !bLoadLowDetail )
		iMaxBlocks = (uint32_t)(cub(iTerrainSize) * 2 * 2.4) + 3;
	else
		iMaxBlocks = (uint32_t)( cub(iTerrainSize*2) * 1.8 * 2.4 ) + 3;

	try
	{
		// Terrain Memory Management	
		uint32_t blocks = iMaxBlocks;
		// Attempt to allocate memory for the terrain
		pTerraData =  new uint32_t [ blocks * 32768 ];
		memset( pTerraData, 0, blocks * 32768 );
		// Allocate memory for the terra mem flags
		pTerraUsage = new char [ blocks ];
		memset( pTerraUsage, 0, blocks );

		// Set the last two blocks as used
		pTerraUsage[blocks-1] = 1;
		pTerraUsage[blocks-2] = 1;
		pTerraUsage[blocks-3] = 1;
	}
	catch ( std::bad_alloc )
	{
		return false;
	}
	return true;
}

void CVoxelTerrain::FreeTerraMemory ( void )
{
	delete [] pTerraData;
		pTerraData = NULL;
	delete [] pTerraUsage;
		pTerraUsage = NULL;
}

void CVoxelTerrain::NewDataBlock  ( subblock16** block )
{
	uint32_t blocks = iMaxBlocks;
	uint32_t normalized_index = 0;

	// Find the first open index
	while ( pTerraUsage[normalized_index] != 0 )
		++normalized_index;

	if ( normalized_index >= blocks )
	{
		cout << "ERROR!" << endl;
		MessageBox( NULL, "Overflow on terrain memory block. This should never happen, so report this error!", "Error", 0 );
		exit(0);
	}

	// Get the new memory index
	uint32_t index = normalized_index * 32768;

	// Set the block to the new value
	//(*block) = (subblock16*)(pTerraData + index);
	(*block) = (subblock16*)(&(pTerraData[index])); // should be the same

	// Turn on flag
	pTerraUsage[normalized_index] = 1;
}
void CVoxelTerrain::FreeDataBlock ( subblock16* block )
{
	uint32_t blocks = iMaxBlocks;
	uint32_t index = 0;

	// Figure out the index of block based on the memory address
	uint32_t* tblock = (uint32_t*)block;
	while ( tblock != pTerraData + index )
		index += 32768;

	// Get the index of the mem flag array
	uint32_t normalized_index = index / 32768;

	if ( normalized_index >= blocks )
		exit(0);

	// Turn off flag
	pTerraUsage[normalized_index] = 0;
}

Terrain::terra_t_lod* CVoxelTerrain::GetLODDataBlock ( uint32_t level )
{
	uint32_t normalized_index = iMaxBlocks-level-1;
	terra_t_lod* c_level = (terra_t_lod*)(&(pTerraData[ normalized_index*32768 ]));
	return c_level;
}