
#include "after/entities/props/CTerrainProp.h"
//#include "CBoob.h"
//#include "CVoxelTerrain.h"
#include "after/terrain/data/GamePayload.h"
#include "after/terrain/data/Node.h"
#include "after/terrain/VoxelTerrain.h"

void CTerrainProp::RemoveFromTerrain ( void )
{
	/*CBoob::ComponentIterator it = find( mBlockInfo.pBoob->v_component.begin(), mBlockInfo.pBoob->v_component.end(), this );
	if ( it != mBlockInfo.pBoob->v_component.end() )
		mBlockInfo.pBoob->v_component.erase( it );
	else
		cout << "Warning: Unable to find component in associated terrain!" << endl;*/
	//throw std::exception();
	if ( VoxelTerrain::GetActive() )
	{
		uint32_t indexer;
		Terrain::GamePayload* mGameData = VoxelTerrain::GetActive()->AquireGamePayload( transform.position, indexer );
	
		auto it = std::find ( mGameData->props.begin(), mGameData->props.end(), this );
		if ( it != mGameData->props.end() ) {
			mGameData->props.erase( it );
		}
		else {
			throw std::exception();
		}

		VoxelTerrain::GetActive()->ReleaseGamePayload( mGameData, indexer );
	}
}
void CTerrainProp::AddToTerrain ( void )
{
	/*CBoob::ComponentIterator it = find( mBlockInfo.pBoob->v_component.begin(), mBlockInfo.pBoob->v_component.end(), this );
	if ( it == mBlockInfo.pBoob->v_component.end() )
		mBlockInfo.pBoob->v_component.push_back( this );
	else
		cout << "Warning: Duplicate component in associated terrain!" << endl;*/

	// Add this component to the target sector.
	//VoxelTerrain::GetActive()->SectorAtPosition // Need a function in VoxelTerrain that returns gamedata sector at position.
	if ( VoxelTerrain::GetActive() )
	{
		uint32_t indexer;
		Terrain::GamePayload* mGameData = VoxelTerrain::GetActive()->AquireGamePayload( transform.position, indexer );
	
		auto it = std::find ( mGameData->props.begin(), mGameData->props.end(), this );
		if ( it == mGameData->props.end() ) {
			mGameData->props.push_back( this );
		}
		else {
			throw std::exception();
		}

		VoxelTerrain::GetActive()->ReleaseGamePayload( mGameData, indexer );
	}
}