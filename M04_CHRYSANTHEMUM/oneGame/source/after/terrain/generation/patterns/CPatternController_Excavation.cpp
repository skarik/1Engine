
#include "CPatternController.h"
#include "after/types/terrain/BlockType.h"
#include "after/terrain/VoxelTerrain.h"
#include "after/terrain/generation/CWorldGen_Terran.h"

#include "after/terrain/edit/csg/SidebufferVolumeEditor.h"

using namespace Terrain;
using namespace World;

void CPatternController::Excavate ( const quickAccessData& qd )
{
	// All towns in memory should be used.....or should they be passed down the stack, and loaded on demand?

	// After performing generation, need to load up towns from the nearby into memory
	// Load in the towns into a stack. Load them at run time. One pass. Just as fast.

	std::vector<RangeVector> t_townList;
	const int xyRange	= 7;
	const int zRange	= 5;
	for ( int i = -xyRange; i <= xyRange; ++i )
	{
		for ( int j = -xyRange; j <= xyRange; ++j )
		{
			for ( int k = -zRange; k <= zRange; ++k )
			{
				RangeVector indexer = RangeVector( qd.indexer.x + i, qd.indexer.y + j, qd.indexer.z + k );
				if ( Pattern_ExistsAt( indexer ) )
				{
					patternData_t		pattern;
					patternQuickInfo_t	quickinfo;
					patternGameInfo_t	gameinfo;
					IO_LoadPattern( indexer, pattern, quickinfo, gameinfo );
					// Now excavate town

					for ( uint index = 0; index < pattern.buildings.size(); ++index ) {
						Excavate_Building( pattern.buildings[i], indexer, quickinfo, gameinfo, qd );
					}

					// End town excavation
				}
			}
		}
	}

}


// Excavate_Building ( ) : Rasterizes the given building at the given position
void CPatternController::Excavate_Building ( const patternBuilding_t& n_building, const RangeVector& n_index, const patternQuickInfo_t& qi, const patternGameInfo_t& gi, const quickAccessData& qd )
{
	//for ( uint i = 0; i < boxes.size(); ++i )
	//{
	if ( n_building.type == World::eBuildingType::BLD_Stoneblock )
	{
		Vector3d_d position ( n_building.box.position.x, n_building.box.position.y, n_building.box.position.z );
		Vector3d_d size ( n_building.box.size.x, n_building.box.size.y, n_building.box.size.z );
		size *= 0.5;
		position += size;
		qd.editor->Add_Box( position, size, EB_STONEBRICK );
	}
	//}
}