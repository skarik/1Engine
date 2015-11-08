
#ifndef _AFTER_VOXEL_TERRAIN_DATA_SAMPLER_H_
#define _AFTER_VOXEL_TERRAIN_DATA_SAMPLER_H_

#include "after/types/terrain/BlockData.h"
#include "after/types/terrain/BlockTracker.h"

class CVoxelTerrain;

namespace Terrain
{
	class DataSampler
	{
	public:
		explicit				DataSampler ( CVoxelTerrain* n_terrain );
								~DataSampler ( void );

		//=========================================//
		// Generate queries
		//=========================================//

		//		BlockTrackerAt
		// Builds a tracker to the block at the given position
		void					BlockTrackerAt ( const Vector3d& n_position, BlockTrackInfo& io_tracker );
		//		BlockAt
		// Grabs the block data at the given position. May or may not build a tracker to do so.
		void					BlockAt ( const Vector3d& n_position, terra_b& io_block );

	private:
		CVoxelTerrain*			terrain;
	};
}

#endif//_AFTER_VOXEL_TERRAIN_DATA_SAMPLER_H_