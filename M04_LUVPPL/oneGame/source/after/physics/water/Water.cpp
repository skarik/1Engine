
// Includes
#include "Water.h"

// Looks at these objects
#include "physical/physics/CPhysics.h"
#include "after/terrain/VoxelTerrain.h"
#include "after/terrain/system/DataSampler.h"
#include "after/types/terrain/BlockType.h"
#include "after/types/terrain/BlockTracker.h"

// Check if position is in water
bool	CAfterWaterTester::PositionInside	( Vector3d const& pos )
{
	// First we check the terrain
	/*if (( !CVoxelTerrain::terrainList.empty() )&&( CVoxelTerrain::terrainList[0] != NULL ))
	{
		BlockTrackInfo blockInfo; //Changed from BlockInfo to BlockTrackInfo
		CVoxelTerrain::terrainList[0]->GetBlockInfoAtPosition( pos, blockInfo );

		// If the block is water, then position is in water
		if ( blockInfo.block.block == Terrain::EB_WATER )
		{
			return true;
		}
	}*/
	if ( CVoxelTerrain::GetActive() )
	{
		BlockTrackInfo blockInfo; //Changed from BlockInfo to BlockTrackInfo
		CVoxelTerrain::GetActive()->Sampler->BlockTrackerAt( pos, blockInfo );

		// If the block is water, then position is in water
		if ( blockInfo.block.block == Terrain::EB_WATER )
		{
			return true;
		}
	}


	// Then we ask the volume manager for the list of volumes

	// If the list isn't empty
		// Loop through all the water volumes for this point


	// If we still can't find anything that we're in, then we're not in water!
	return false;
}

// Returns the fluid velocity at the given position
Vector3d CAfterWaterTester::GetFlowField ( Vector3d const& pos )
{
	return Vector3d::zero;
}

//== Sea and Storm Compatibility==
// Grabs the height of the ocean at the current XY position.
float CAfterWaterTester::OceanHeight ( Vector3d const& pos )
{
	return 0;
}