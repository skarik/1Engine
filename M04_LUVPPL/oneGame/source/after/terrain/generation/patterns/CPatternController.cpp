
#include "CPatternController.h"

#include "core/math/random/random_vector.h"
#include "core/math/random/Random.h"

#include "IPatternGeneration.h"
#include "after/terrain/generation/regions/CRegionGenerator.h"
#include "after/terrain/generation/CWorldGen_Terran.h"
#include "after/terrain/VoxelTerrain.h"

#include "after/utils/NamingUtils.h"

using namespace Terrain;
using namespace World;

CPatternController::CPatternController ( CVoxelTerrain* n_terra )
	: m_terrain( n_terra ), CGameBehavior()
{

}

CPatternController::~CPatternController ( void )
{

}



void CPatternController::Update ( void )
{

}


void CPatternController::Generate ( const RangeVector& n_index )
{
	CRegionGenerator* t_regionController = m_terrain->GetRegionGenerator();

	// Check current region
	uint32_t t_region = t_regionController->Region_ClosestValidToIndex( n_index );

	if ( t_region != REGION_FAILURE && t_region != REGION_NONE && t_region != REGION_FAILURE )
	{
		// Check that the region needs town generation
		m_gen_sync.lock();
		if ( t_regionController->IO_RegionGetSetHasTowns( t_region ) )
		{
			regionproperties_t t_regionProperties;
			t_regionController->IO_RegionLoadInfo( t_region, &t_regionProperties );
			// Loop through the area
			std::vector<RangeVector> t_sectors;
			t_regionController->IO_RegionLoadSectors( t_region, t_sectors );

			// Grab the sampler to use
			auto randomSampler = ((CWorldGen_Terran*)m_terrain->GetGenerator())->m_buf_general_noise;

			// Initialize the generation state
			std::vector<RangeVector> t_usedSectors;
			townGenerationState_t generationState = {0};
			generationState.region = t_region;
			generationState.area_count = t_sectors.size();
			generationState.region_properties = &t_regionProperties;

			// For each area, run the generation algorithm
			for ( uint i = 0; i < t_sectors.size(); ++i )
			{
				if ( Pattern_ExistsAt( t_sectors[i] ) ) {
					generationState.town_count += 1;
					continue;
				}
				// Grab spawn chance
				Real t_spawnChance = Gen_SpawnChance( generationState, t_sectors[i], t_usedSectors );
				// Check chance to want to make a town
				if ( t_spawnChance >= (randomSampler.sampleBufferMicro( t_sectors[i].y*2.3f+0.5f,t_sectors[i].x*2.3f+0.5f )/256.0 - 128)*1.5 )
				{
					// And make the town (this saves it to the hard drive)
					Gen_PatternType( generationState, t_sectors[i] );
					// Save generation data for decreasing chances in the next iteration
					t_usedSectors.push_back( t_sectors[i] );
				}
			}
			// End generation algorithm
		} // End region check for town generation
		m_gen_sync.unlock();
	}

}


Real CPatternController::Gen_SpawnChance ( townGenerationState_t& ts, const RangeVector& n_position, const std::vector<RangeVector>& n_townlist )
{
	Real spawnChance;
	// Grab low-res sampler
	auto randomSampler = ((CWorldGen_Terran*)m_terrain->GetGenerator())->m_buf_general_noise;

	// Create base spawn chance.
	//spawnChance = generator->noise_hf->Unnormalize( generator->noise_hf->Get( areaPosition.x+0.5f, areaPosition.y+0.5f ) ) + 0.5f;
	spawnChance = randomSampler.sampleBufferMacro( n_position.x+0.5, n_position.y+0.5 + n_position.z*0.02 )/256.0;
	//spawnChance = Math.Lerp( 0.5f, spawnChance, 0.5f );

	// Sample the terrain type.
	Vector3d_d t_worldPosition ( n_position.x*64 + 32, n_position.y*64 + 32, n_position.z*64 + 32 );
	ts.biome	= m_terrain->GetGenerator()->GetBiomeAt( t_worldPosition );
	ts.terrain	= m_terrain->GetGenerator()->GetTerrainAt( t_worldPosition );
	ts.elevation= m_terrain->GetGenerator()->GetLandmassity( Vector2d_d(t_worldPosition.x,t_worldPosition.y), false ) / 64.0;

	// Set chance based on terrain type.
	switch ( ts.terrain ) {
		case TER_FLATLANDS: spawnChance += 0.3f;	break;
		case TER_HILLLANDS:	spawnChance += 0.2f;	break;
		case TER_SPIRES:	spawnChance += 0.3f;	break;
		case TER_ISLANDS:	spawnChance += 0.1f;	break;
		case TER_BADLANDS:	spawnChance -= 0.2f;	break;
		case TER_DESERT:	spawnChance -= 0.3f;	break;
		case TER_OUTLANDS:	spawnChance -= 0.6f;	break;
		// No towns
		case TER_THE_EDGE:	spawnChance -= 2.0f;	break;
	}
	switch ( ts.biome ) {
		// No towns
		case BIO_S_TUNDRA:	spawnChance -= 2.0f;	break;
	}
	// Decrease general spawn chance
	spawnChance -= 0.5f;
	// Modify chance based on size of area and the number of towns
	spawnChance -= ts.town_count / (ts.area_count/20.0f); // 1 town every 20 areas
	// Smaller chance to spawn near towns in same region
	for ( uint j = 0; j < n_townlist.size(); ++j ) {
		spawnChance -= std::max<ftype>( 0, 7-(Vector2d( (ftype)n_position.x-n_townlist[j].x, (ftype)n_position.y-n_townlist[j].y ).magnitude()) )*0.6f;
	}

	// Return the generated chance
	return spawnChance;
}


#include "ruins/PatternDesertRuins.h"

// Gen_PatternType ( ) : Generates a pattern and its properties.
void CPatternController::Gen_PatternType ( townGenerationState_t& ts, const RangeVector& n_index )
{
	//std::cout << "Making town at " << n_index.x << ", " << n_index.y << ", " << n_index.z << std::endl;

	// Grab low-res sampler
	auto randomSampler = ((CWorldGen_Terran*)m_terrain->GetGenerator())->m_buf_general_noise;

	// Create a town.
	patternData_t		pattern;
	patternQuickInfo_t	patternQuickinfo;
	patternGameInfo_t	patternGameinfo;
	RangeVector			t_areaPosition = n_index;

	// Set default town properties
	{	
		if ( ts.terrain == TER_SPIRES ) {
			ts.elevation += 1.4f;
		}
		t_areaPosition.z = rangeint(ts.elevation); // TODO: Ignore this line for underground cities

		patternQuickinfo.initial_region	= ts.region;
		patternQuickinfo.region			= ts.region;
		patternQuickinfo.position		= n_index;

		patternQuickinfo.townType		= World::TOWN_TYPE_NORMAL;
		// How to select different types of towns, now? Definitely could base it on region's culture.
		random_vector<World::ePatternType> townSampler;
		townSampler.push_back( World::TOWN_TYPE_NORMAL, 1.0f );
		townSampler.push_back( World::TOWN_TYPE_BANDIT_CAMP, 1.0f );
		patternQuickinfo.townType		= townSampler.get_target_value( Random.Range(0,1) );
			//( randomSampler.sampleBufferMacro( areaPosition.y*2.7-0.1, areaPosition.x*2.7+0.1 )/256.0 )
			//);
			//townSampler.get_target_value( generator->noise->Unnormalize(generator->noise_hhf->Get(areaPosition.y*1.7f-0.1f,areaPosition.x*1.7f+0.1f))+0.5f );

		// Generate town name
		patternQuickinfo.name	= NamingUtils::CreateBaseTownName();
	}

	// Load region for its culture values 
	{
		// Set culture values
		memcpy( pattern.properties.culture, ts.region_properties->culture, 8 );
	}

	// Choose generation properties for the town
	{
		/*patternLandmark_t town_center;
		town_center.name = "Town Center";
		town_center.radius = 1.0f;
		town_center.position = Vector3d( areaPosition.x+0.5f,areaPosition.y+0.5f,areaElevation );
		genTown->landmarks.push_back( town_center );
		// Generate roads.
		GenerateRoads();
			cout << "  with " << genTown->roads.size() << " roads" << endl;
		// Generate locations.
		GenerateLocations();
			cout << "  with " << genTown->landmarks.size()+genTown->instances.size() << " locations" << endl;
		// Generate buildings.
		GenerateBuildings();
			cout << "   and " << genTown->buildings.size() << " buildings" << endl;
		cout << "Region is " << regionId << endl;*/
		// Generate the town
		//CTownGenerationInterface* t_interface;
		IPatternGeneration*	t_interface = NULL;

		// Create all the generation types on the stack (they're pretty small objects)
		PatternDesertRuins	tg_desertRuins;
		switch ( patternQuickinfo.townType ) {
			/*case World::TOWN_TYPE_NORMAL:
				t_interface = tgDefault;
				break;
			case World::TOWN_TYPE_BANDIT_CAMP:
				t_interface = tgBandits;
				break;*/
			default:
				t_interface = &tg_desertRuins;
		}

		// Set interface information
		t_interface->areaTerra		= ts.terrain;
		t_interface->areaBiome		= ts.biome;
		t_interface->areaElevation	= ts.elevation;
		t_interface->areaPosition	= n_index;
		t_interface->genTown		= &pattern;
		t_interface->genTownEntry	= &patternQuickinfo;
		t_interface->terrain		= m_terrain;
		//t_interface->generator		= generator;
		//t_interface->floorplans		= this;
		t_interface->Generate();
	}
	// Save town to disk.
	//CBinaryFile town_file;
	//TownOpenFileWrite( newTownEntry.name, newTownEntry.initial_region, town_file );
	//TownOpenFileWrite( newTownEntry.position, town_file );
	//TownWriteInfo( town_file, &newTownInfo );
	//TownWritePattern( town_file, &newTown );

	// Write new town to map
	//TownSaveEntry( &patternQuickinfo.townType );
	IO_SavePattern ( n_index, pattern, patternQuickinfo, patternGameinfo );

	// Increment town count
	switch ( patternQuickinfo.townType )
	{
	case World::TOWN_TYPE_NORMAL:
		ts.town_count += 1;
		break;
	default:
		ts.town_count += 0.1f;
		break;
	}

}