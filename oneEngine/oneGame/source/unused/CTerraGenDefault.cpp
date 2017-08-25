
// ======== CTerraGenDefault =======
// This is the definition for the constructor, destructor, 
// and high-level generation routines of the default generation.
// Initialization of the generator is handled in the constructor.


#include "CTerraGenDefault.h"
#include "CRegionManager.h"
#include "CTownManager.h"
#include "CGameSettings.h"
#include "CTimeProfiler.h"

// Initialize variables
CTerraGenDefault::CTerraGenDefault ( CVoxelTerrain* inTerrain, long int seed )
	: CTerrainGenerator ( inTerrain, seed )
{
	bHasSamplers = false;
	{   // Start off with null samplers
		noise		= NULL;
		noise_hf	= NULL;
		noise_hhf	= NULL;
		noise_lf	= NULL;
		noise_biome	= NULL;
		noise_terra = NULL;

		pSamplerTerrainType	= NULL;
		pSamplerBiomeType	= NULL;

		noise_pub		= NULL;
		noise_pub_hf	= NULL;
		noise_pub_hhf	= NULL;
		noise_pub_lf	= NULL;
		noise_pub_biome	= NULL;
		noise_pub_terra = NULL;
	}

	{   // Initialize dungeon table (resize it to start to reduce sizing overhead)
		dungeonList.rehash( 4096 );
	}

	{
		vCurrentFeatureIndex.z = -1;
		// Initialize feature states
		char stemp_fn [256];
		sprintf( stemp_fn, "%s.regions/features.state", CGameSettings::Active()->GetTerrainSaveDir().c_str() );
		CBinaryFile featureFile;
		featureFile.Open( stemp_fn, CBinaryFile::IO_READ );
		if ( !featureFile.IsOpen() ) {
			// Set an empty file
			iFeatureCount = 0;
			featureFile.Open( stemp_fn, CBinaryFile::IO_WRITE );
			featureFile.WriteUInt64( 0 );
		}
		else {
			// Read in feature index stepper
			iFeatureCount = featureFile.ReadUInt32();
			// Read in feature list as well
			uint32_t iFeatureActualCount = featureFile.ReadUInt32();
			featureList.resize( iFeatureActualCount );
			featureFile.ReadData( (char*)&(featureList[0]), sizeof(TerraFeature)*iFeatureActualCount );
		}
		// Feature file closes automatically
	}

	{	// Load region and town manager
		regionManager	= new CRegionManager( terrain, this );
		townManager		= new CTownManager( terrain, this );
	}
}

CTerraGenDefault::~CTerraGenDefault ( void )
{
	// Save state
	char stemp_fn [256];

	sprintf( stemp_fn, "%s.regions/features.state", CGameSettings::Active()->GetTerrainSaveDir().c_str() );
	{
		CBinaryFile featureFile;
		featureFile.Open( stemp_fn, CBinaryFile::IO_WRITE );
		featureFile.WriteUInt32( iFeatureCount );
		featureFile.Close();
	}

	// Free Memory
	delete noise;
	delete noise_hf;
	delete noise_hhf;
	delete noise_lf;
	delete noise_biome;
	delete noise_terra;

	delete noise_pub;
	delete noise_pub_hf;
	delete noise_pub_hhf;
	delete noise_pub_lf;
	delete noise_pub_biome;
	delete noise_pub_terra;

	if ( bHasSamplers )
	{
		delete [] pSamplerTerrainType;
		delete [] pSamplerBiomeType;
	}

	delete regionManager;
	delete townManager;
}

// Main Simulation Routine
void CTerraGenDefault::Simulate ( void )
{
	if ( regionManager ) {
		regionManager->Simulate();
	}
}


// Main Generation Routine

// Begin to generate the terrain
void CTerraGenDefault::GenerateTerrain ( CBoob * pBoob, RangeVector const& position )
{
	TerraGenGetSamplers();
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

// === TERRAIN GENERATION PASSES ===
// == First Pass == 
void CTerraGenDefault::GenerateTerrainSector ( CBoob * pBoob, RangeVector const& position )
{
	if (( pBoob != NULL )&&( pBoob->bitinfo[3] == false ))
	{
		//TimeProfiler.BeginTimeProfile( "terraGen" );

		pBoob->bitinfo[3] = true;	// Set terrain as already generated

		// Set position
		pBoob->position = Vector3d( ftype(position.x*64+32),ftype(position.y*64+32),ftype(position.z*64+32) );
		pBoob->li_position = position;

		// Spread terrain generation. TODO: FIX THIS SOMEDAY
		if ( bSpreadTerrain )
		{
			if ( position.z >= 0 )
				GenerateTerrainSector( pBoob->top, RangeVector( position.x, position.y, position.z+1 ) );
			if ( position.x >= 0 )
				GenerateTerrainSector( pBoob->front, RangeVector( position.x+1, position.y, position.z ) );
			if ( position.y >= 0 )
				GenerateTerrainSector( pBoob->left, RangeVector( position.x, position.y+1, position.z ) );
			if ( position.z <= 0 )
				GenerateTerrainSector( pBoob->bottom, RangeVector( position.x, position.y, position.z-1 ) );
			if ( position.x <= 0 )
				GenerateTerrainSector( pBoob->back, RangeVector( position.x-1, position.y, position.z ) );
			if ( position.y <= 0 )
				GenerateTerrainSector( pBoob->right, RangeVector( position.x, position.y-1, position.z ) );
		}
		// Generate the terrain
		TerraGenLandscapePass( pBoob, position );
		// Put stone in the dirt
		TerraGenStonePass( pBoob, position );
		// Put dirt in the stone
		TerraGenUnderdirtPass( pBoob, position );
		// Make caves (is now in dungeons)
		//TerraGenCavePass( pBoob, position );
		// Make bridges (is now in caves)
		//TerraGenBridgePass( pBoob, position );
		// Make rivers
		TerraGenRiverPass( pBoob, position );

		//TimeProfiler.EndPrintTimeProfile( "terraGen" );
	}
}

// == Second Pass ==
void CTerraGenDefault::GenerateTerrainSectorSecondPass ( CBoob * pBoob, RangeVector const& position )
{
	if (( pBoob != NULL )&&( pBoob->bitinfo[0] == false ))
	{
		pBoob->bitinfo[0] = true;
		// TODO: FIX THIS SOMEDAY
		if ( bSpreadTerrain )
		{
			if ( position.z >= 0 )
				GenerateTerrainSectorSecondPass( pBoob->top, RangeVector( position.x, position.y, position.z+1 ) );
			if ( position.x >= 0 )
				GenerateTerrainSectorSecondPass( pBoob->front, RangeVector( position.x+1, position.y, position.z ) );
			if ( position.y >= 0 )
				GenerateTerrainSectorSecondPass( pBoob->left, RangeVector( position.x, position.y+1, position.z ) );
			if ( position.z <= 0 )
				GenerateTerrainSectorSecondPass( pBoob->bottom, RangeVector( position.x, position.y, position.z-1 ) );
			if ( position.x <= 0 )
				GenerateTerrainSectorSecondPass( pBoob->back, RangeVector( position.x-1, position.y, position.z ) );
			if ( position.y <= 0 )
				GenerateTerrainSectorSecondPass( pBoob->right, RangeVector( position.x, position.y-1, position.z ) );
		}
		// Create instanced stuff
		TerraGenInstancePass( pBoob, position );

		// Make dungeons
		TerraGenDungeonPass( pBoob, position );
		// Make patterns
		TerraGenPatternPass( pBoob, position );
	}
}

// == Third Pass ==
void CTerraGenDefault::GenerateTerrainSectorThirdPass ( CBoob * pBoob, RangeVector const& position )
{
	if (( pBoob != NULL )&&( pBoob->bitinfo[7] == false ))
	{
		pBoob->bitinfo[7] = true;
		// TODO: FIX THIS SOMEDAY
		if ( bSpreadTerrain )
		{
			if ( position.z >= 0 )
				GenerateTerrainSectorThirdPass( pBoob->top, RangeVector( position.x, position.y, position.z+1 ) );
			if ( position.x >= 0 )
				GenerateTerrainSectorThirdPass( pBoob->front, RangeVector( position.x+1, position.y, position.z ) );
			if ( position.y >= 0 )
				GenerateTerrainSectorThirdPass( pBoob->left, RangeVector( position.x, position.y+1, position.z ) );
			if ( position.z <= 0 )
				GenerateTerrainSectorThirdPass( pBoob->bottom, RangeVector( position.x, position.y, position.z-1 ) );
			if ( position.x <= 0 )
				GenerateTerrainSectorThirdPass( pBoob->back, RangeVector( position.x-1, position.y, position.z ) );
			if ( position.y <= 0 )
				GenerateTerrainSectorThirdPass( pBoob->right, RangeVector( position.x, position.y-1, position.z ) );
		}
		// Generate the grass, small details, and settle liquids
		TerraGenBiomePass ( pBoob, position );

		//TerraGenLiquidPass ( pBoob, position );
	}
}