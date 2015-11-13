
// ======== CWorldGen_Terran_Patterns =======
// This is the definition for pattern creation

#include "CWorldGen_Terran.h"

#include "core/math/Math.h"
#include "core/math/Math3d.h"
#include "core/math/noise/SimplexNoise.h"
#include "core/math/random/Random.h"

#include "core/math/matrix/CMatrix.h"

#include "after/entities/foliage/CTerraFoliageFactory.h"
#include "after/entities/props/CTerrainPropFactory.h"

#include "after/terrain/edit/csg/SidebufferVolumeEditor.h"
#include "after/terrain/VoxelTerrain.h"

#include "after/terrain/generation/patterns/CPatternController.h"
#include "after/terrain/generation/regions/CRegionGenerator.h"

using namespace Terrain;

// =========================================
//     == Sector (Patterns + Regions) ==
// =========================================

void CWorldGen_Terran::Generate_Patterns ( const quickAccessData& qd, inputTerrain_t& n_terrain )
{
	// Generate region for the pattern system to work
	//n_terrain.regions->Generate( qd );
	// Generate and load patterns into memory
	//n_terrain.patterns->Generate( qd );

	// Generate region for the pattern system to work
	for ( char i = -5; i <= 5; ++i ) {
		for ( char j = -5; j <= 5; ++j ) {
			n_terrain.regions->Generate( RangeVector( qd.indexer.x+i,qd.indexer.y+j,qd.indexer.z ) );
		}
	}
	// Generate and load patterns into memory
	for ( char i = -5; i <= 5; ++i ) {
		for ( char j = -5; j <= 5; ++j ) {
			n_terrain.patterns->Generate( RangeVector( qd.indexer.x+i,qd.indexer.y+j,qd.indexer.z ) );
		}
	}

	// Excavate patterns
	n_terrain.patterns->Excavate( qd );
}