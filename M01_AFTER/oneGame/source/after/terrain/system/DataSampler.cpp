
#include "DataSampler.h"

#include "core/common.h"
#include "after/terrain/VoxelTerrain.h"

Terrain::DataSampler::DataSampler ( CVoxelTerrain* n_terrain )
	: terrain( n_terrain )
{
	;
}
Terrain::DataSampler::~DataSampler( void )
{
	;
}


//=========================================//
// Generate queries
//=========================================//

//		BlockTrackerAt
// Builds a tracker to the block at the given position
void Terrain::DataSampler::BlockTrackerAt ( const Vector3d& n_position, BlockTrackInfo& io_tracker )
{
	throw Core::NotYetImplementedException();
}
//		BlockAt
// Grabs the block data at the given position. May or may not build a tracker to do so.
void Terrain::DataSampler::BlockAt ( const Vector3d& n_position, terra_b& io_block )
{
	throw Core::NotYetImplementedException();
}