
#include "CTerrainGenerator.h"


// Constructor (set terrain reference to given one)
CTerrainGenerator::CTerrainGenerator ( CVoxelTerrain* inTerr, long int seed )
	: terrain( inTerr ), iTerrainSeed( seed )
{
	bSpreadTerrain = true;
}
CTerrainGenerator::~CTerrainGenerator ( void )
{
	;
}

// Begin to generate the terrain
void CTerrainGenerator::GenerateTerrain ( CBoob * pBoob, RangeVector const& position )
{
	// Primary main terrain pass
	//Debug::Console->PrintMessage( "   Primary pass (terrain) subdef_dungeon+negativedungeon...\n" );
	GenerateTerrainSector( pBoob, position );
	// Grass and various object generation pass
	//Debug::Console->PrintMessage( "   Second pass (features) subdef_instances...\n" );
	GenerateTerrainSectorSecondPass( pBoob, position );
	// Grass and various object generation pass
	//Debug::Console->PrintMessage( "   Final pass (biomes, grass and shiz) subdef_biome...\n" );
	GenerateTerrainSectorThirdPass( pBoob, position );
	// Clean up our mess of variables
	//Debug::Console->PrintMessage( "   Cleanup pass (lol bad vars) subdef_cleanuplol...\n" );
	GenerateTerrainSectorCleanup( pBoob );
}
// Same as previous functions, just without the large comments
void CTerrainGenerator::GenerateSingleTerrain ( CBoob * pBoob, RangeVector const& position )
{
	bSpreadTerrain = false;
	pBoob->bitinfo[0] = false;
	pBoob->bitinfo[3] = false;
	pBoob->bitinfo[7] = false;
	//Debug::Console->DisableOutput();
	GenerateTerrain( pBoob, position );
	//Debug::Console->EnableOutput();
	bSpreadTerrain = true;
}

// == Cleanup Pass ==
void CTerrainGenerator::GenerateTerrainSectorCleanup ( CBoob * pBoob )
{
	if (( pBoob != NULL )&&( pBoob->bitinfo[0] == true ))
	{
		pBoob->bitinfo[0] = false;
		pBoob->bitinfo[7] = false;

		// Boob now has data
		pBoob->hasBlockData = true;
		pBoob->hasPropData = true;

		if ( bSpreadTerrain )
		{
			// TODO: FIX THIS SOMEDAY
			GenerateTerrainSectorCleanup( pBoob->top );
			GenerateTerrainSectorCleanup( pBoob->front );
			GenerateTerrainSectorCleanup( pBoob->left );
			GenerateTerrainSectorCleanup( pBoob->bottom );
			GenerateTerrainSectorCleanup( pBoob->back );
			GenerateTerrainSectorCleanup( pBoob->right );
		}
	}
}