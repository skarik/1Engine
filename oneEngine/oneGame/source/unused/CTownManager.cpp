
#include "CTownManager.h"
#include "CRegionManager.h"

#include "CBinaryFile.h"
#include "FileUtils.h"
#include "CVoxelTerrain.h"
#include "CTerraGenDefault.h"
#include "CGameSettings.h"

#include "CWorldState.h"
#include "Zones.h"

#include "Math.h"
#include "NamingUtils.h"

#include "CDebugDrawer.h"

#include <list>
#include "random_vector.h"
using std::list;
using std::cout;
using std::endl;
using std::max;
using std::min;

// Global pointer to active town manager
namespace World
{
	CTownManager* ActiveTownManager = NULL;
};

CTownManager::CTownManager( CVoxelTerrain* pTerrain, CTerraGenDefault* pGenerator )
	: CGameBehavior(), terrain( pTerrain ), generator( pGenerator )
{
	//ImageToFloorPlan( ".res/terra/b1.png" );

	townCounter = 4;

	// Open town map
	char stemp_fn [256];

	if ( !townMapFile.IsOpen() ) {
		sprintf( stemp_fn, "%s.towns/map", CGameSettings::Active()->GetTerrainSaveDir().c_str() );
		if ( IO::FileExists( stemp_fn ) ) {
			townMapFile.Open( stemp_fn, CBinaryFile::IO_READ|CBinaryFile::IO_WRITE );
		}
		else {
			townMapFile.Open( stemp_fn, CBinaryFile::IO_READ|CBinaryFile::IO_WRITE );
			// If file is clean, then dirty it.
			fseek( townMapFile.GetFILE(), 0, SEEK_SET );
			memset( io_buffer, 0, 1024 );
			fwrite( io_buffer, 1024, 1, townMapFile.GetFILE() );
			fwrite( io_buffer, 1024, 1, townMapFile.GetFILE() );
		}
	}

	// Set active town manager.
	if ( World::ActiveTownManager == NULL ) {
		World::ActiveTownManager = this;
	}

	// Prepass all needed files
	PrepassCultureManifest();
}

CTownManager::~CTownManager ( void )
{
	//regionFile.Close();

	// if this is the active town manager, clear it.
	if ( World::ActiveTownManager == this ) {
		World::ActiveTownManager = NULL;
	}
}

//	Update
// Runs in the main game thread.
// Will simulate events, such as hysteria, fires, and other live events that need a frame by frame tracker.
// Must only have very light calculations. Heavy calculations should go into Simulate().
void CTownManager::Update ( void )
{
	if ( ActiveGameWorld == NULL )
		return;		// We cannot perform work on towns without the world to keep track of it.

	/*mutex::scoped_lock( townMapFileLock );
	for ( uint i = 0; i < townMap.size(); ++i )
	{
		for ( uint j = 0; j < townMap[i].entry.roads.size(); ++j )
		{
			Debug::Drawer->DrawRay( Ray(townMap[i].entry.roads[j].location.pos*64.0f,townMap[i].entry.roads[j].location.dir*64.0f), Color(0.7,0,0,1) );
		}
	}*/
}
//	Simulate
// Runs in the simulation side thread of the terrain thread.
// Should simulate growth and construction of towns.
void CTownManager::Simulate ( void )
{
	//Settlers moving in and changing town type is an example of what would go here.
}
//	Generate
// Runs in the generation side thread of the terrain thread.
// Given an input position, will ensure the region it belongs to has its towns generated.
// Also will load the town map file and make sure it exists.
#include "CTownGenerationInterface.h"
#include "TownGen_A0.h"
#include "TownGen_A1.h"
#include "TownGen_BanditCamp0.h"
void CTownManager::Generate ( const RangeVector& position )
{
	char stemp_fn [256];
	uint32_t areaCount;
	uint32_t regionId;
	vector<CRegionManager::sProvinceAreaEntry> areaList;

	// Open the town map
	{
		if ( !townMapFile.IsOpen() ) {
			sprintf( stemp_fn, "%s.towns/map", CGameSettings::Active()->GetTerrainSaveDir().c_str() );
			if ( IO::FileExists( stemp_fn ) ) {
				townMapFile.Open( stemp_fn, CBinaryFile::IO_READ|CBinaryFile::IO_WRITE );
			}
			else {
				townMapFile.Open( stemp_fn, CBinaryFile::IO_READ|CBinaryFile::IO_WRITE );
				// If file is clean, then dirty it.
				fseek( townMapFile.GetFILE(), 0, SEEK_SET );
				memset( io_buffer, 0, 1024 );
				fwrite( io_buffer, 1024, 1, townMapFile.GetFILE() );
				fwrite( io_buffer, 1024, 1, townMapFile.GetFILE() );
			}
		}
	}

	regionId = World::ActiveRegionManager->GetRegion( position.x, position.y );
	{
		// Check that the region has not had towns generated yet
		uint8_t hasTowns = World::ActiveRegionManager->GetProvinceHasTowns( regionId );
		if ( hasTowns == 255 ) { // Will happen for bad regions (such as edge or ocean)
			//cout << "BAD TOWN QUERY ON REGION " << regionId << "!!!" << endl;
			return;
		}
		else if ( hasTowns != 0 ) {
			// If it has already been generated, then we skip the rest of the madness in here.
			return;
		}
		// If we're here, set it as towns made
		World::ActiveRegionManager->SetProvinceHasTowns( regionId, 1 );
	}

	// Get a list of all the areas in the current region, and generate towns using that list.
	areaCount = World::ActiveRegionManager->GetProvinceArea( regionId, areaList );

	// Generate towns
	ftype	townCount = 0;
	ftype	spawnChance = 0.0f;
	vector<RangeVector>	townPositions;

	for ( uint32_t i = 0; i < areaCount; ++i )
	{
		// For this area, do area generation, using the generator for sample info.
		RangeVector areaPosition;
		areaPosition.x = areaList[i].x;
		areaPosition.y = areaList[i].y;

		// At this point, check for the town file, as there are instances where a town's initial region no longer exists.
		if ( TownFileExists( areaPosition ) ) {
			// Skip this town (TODO: we most likely want to modify the region here)
			continue;
		}

		// Create base spawn chance.
		spawnChance = generator->noise_hf->Unnormalize( generator->noise_hf->Get( areaPosition.x+0.5f, areaPosition.y+0.5f ) ) + 0.5f;
		//spawnChance = Math.Lerp( 0.5f, spawnChance, 0.5f );

		// Sample the terrain type.
		areaBiome = generator->TerraGen_priv_GetBiome( Vector3d(areaPosition.x+0.5f,areaPosition.y+0.5f,0) );
		areaTerra = generator->iCurrentTerraType;
		areaElevation = generator->fCurrentElevation;

		// Set chance based on terrain type.
		switch ( areaTerra ) {
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
		switch ( areaBiome ) {
			// No towns
			case BIO_S_TUNDRA:	spawnChance -= 2.0f;	break;
		}
		// Decrease general spawn chance
		spawnChance -= 0.5f;
		// Modify chance based on size of area and the number of towns
		spawnChance -= townCount / (areaCount/20.0f); // 1 town every 20 areas
		// Smaller chance to spawn near towns in same region
		for ( uint j = 0; j < townPositions.size(); ++j ) {
			spawnChance -= std::max<ftype>( 0, 6-(Vector2d( (ftype)areaPosition.x-townPositions[j].x, (ftype)areaPosition.y-townPositions[j].y ).magnitude()) )*0.2f;
		}

		// Spawn town if chance allows it.
		if ( spawnChance >= (generator->noise->Unnormalize(generator->noise_hf->Get( areaPosition.y*2.3f+0.5f,areaPosition.x*2.3f+0.5f ))+0.5f) )
		{
			cout << "Making town at " << areaPosition.x << ", " << areaPosition.y << endl;

			// Create a town.
			sPattern	newTown;		genTown = &newTown;
			sTownEntry	newTownEntry;	genTownEntry = &newTownEntry;
			sTownInfo	newTownInfo;
			{	// Set default town properties
				if ( areaTerra == TER_SPIRES ) {
					areaElevation += 1.4f;
				}
				areaPosition.z = rangeint(areaElevation);

				newTownEntry.initial_region	= regionId;
				newTownEntry.region			= regionId;
				newTownEntry.position		= areaPosition;

				newTownEntry.townType		= World::TOWN_TYPE_NORMAL;
				// How to select different types of towns, now? Definitely could base it on region's culture.
				random_vector<World::ePatternType> townSampler;
				townSampler.push_back( World::TOWN_TYPE_NORMAL, 1.0f );
				townSampler.push_back( World::TOWN_TYPE_BANDIT_CAMP, 1.0f );
				newTownEntry.townType		= townSampler.get_target_value( generator->noise->Unnormalize(generator->noise_hhf->Get(areaPosition.y*1.7f-0.1f,areaPosition.x*1.7f+0.1f))+0.5f );

				// Generate town name
				newTownEntry.name	= NamingUtils::CreateBaseTownName();
			}
			{
				// Load town 
				CRegionManager::sProvinceInfo info;
				World::ActiveRegionManager->GetProvinceInfo( regionId, &info );
				// Set culture values
				memcpy( newTown.properties.culture, info.culture, 8 );
			}
			{
				// Choose starting point for the town.
				/*sPatternLandmark town_center;
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
				CTownGenerationInterface* t_interface;
				{
					static CTownGenerationInterface* tgDefault = new TownGen_A1;
					static CTownGenerationInterface* tgBandits = new TownGen_BanditCamp0;
					switch ( newTownEntry.townType ) {
					case World::TOWN_TYPE_NORMAL:		t_interface = tgDefault;
					case World::TOWN_TYPE_BANDIT_CAMP:	t_interface = tgBandits;
					default:							t_interface = tgDefault;
					}
				}

				t_interface->areaTerra		= areaTerra;
				t_interface->areaBiome		= areaBiome;
				t_interface->areaElevation	= areaElevation;
				t_interface->areaPosition	= areaPosition;
				t_interface->genTown		= genTown;
				t_interface->genTownEntry	= genTownEntry;
				t_interface->terrain		= terrain;
				t_interface->generator	= generator;
				t_interface->floorplans	= this;
				t_interface->Generate();
			}
			// Save town to disk.
			CBinaryFile town_file;
			//TownOpenFileWrite( newTownEntry.name, newTownEntry.initial_region, town_file );
			TownOpenFileWrite( newTownEntry.position, town_file );
			TownWriteInfo( town_file, &newTownInfo );
			TownWritePattern( town_file, &newTown );

			// Write new town to map
			TownSaveEntry( &newTownEntry );

			// Increment town count
			switch ( newTownEntry.townType )
			{
			case World::TOWN_TYPE_NORMAL:
				townCount += 1;
				break;
			default:
				townCount += 0.1f;
				break;
			}
			townPositions.push_back( areaPosition );
		}
	}

#ifdef _TOWN_PREGEN_SLOW_VARIANT_
	// After generating towns, go through the town positions and mark those positions in the region as having a town
	// (These likely will not be accurate later, so don't rely on these)
	{
		CBinaryFile province_file;
		World::ActiveRegionManager->ProvinceOpenFile( regionId, province_file, false );
		if ( province_file.IsOpen() ) {
			for ( uint i = 0; i < townPositions.size(); ++i ) {
				World::ActiveRegionManager->ProvinceSectorSetTowns( townPositions[i], province_file, true );
			}
		}
	}
#endif

	// After X amount of generates, save the region file.
	if ( --townCounter == 0 ) {
		townCounter = 4;
		townMapFile.Close(); // This is how the region file is saved.
		// Open the file back up
		//sprintf( stemp_fn, "%s.towns/map", CGameSettings::Active()->GetTerrainSaveDir().c_str() );
		townMapFile.Open( NULL, CBinaryFile::IO_READ|CBinaryFile::IO_WRITE );
	}
}

/*
//	GetTownInfo
// Returns town info that's saved. Returns if true if valid data filled.
bool CTownManager::GetTownInfo ( arstring<256> town_name, uint32_t n_region, sTownInfo* out_town_info )
{
	CBinaryFile file;
	// Open province file
	TownOpenFile( town_name, n_region, file, false );
	// Read in data
	return GetTownInfo( file, out_town_info );
}
bool CTownManager::GetTownInfo ( CBinaryFile& town_file, sTownInfo* out_town_info )
{
	if ( town_file.IsOpen() )
	{
		// Seek to pos
		fseek( town_file.GetFILE(), 0, SEEK_SET );
		// Read data
		fread( io_buffer, 512, 1, town_file.GetFILE() );
		memcpy( out_town_info, io_buffer, sizeof(sTownInfo) );
		return true;
	}
	else {
		return false;
	}
}
//	SetTownInfo
// Sets and saves town info.
void CTownManager::SetTownInfo ( arstring<256> town_name, uint32_t n_region, const sTownInfo* n_town_info )
{
	CBinaryFile file;
	// Open province file
	TownOpenFile( town_name, n_region, file, false );
	// Read in data
	SetTownInfo( file, n_town_info );
}
void CTownManager::SetTownInfo ( CBinaryFile& town_file, const sTownInfo* n_town_info )
{
	if ( town_file.IsOpen() )
	{
		// Seek to pos
		fseek( town_file.GetFILE(), 0, SEEK_SET );
		// Read data
		memcpy( io_buffer, n_town_info, sizeof(sTownInfo) );
		fwrite( io_buffer, 512, 1, town_file.GetFILE() );
	}
}
*/

//	LoadTowns ( RANGEVECTOR centerposition, INT width )
// Loads nearby towns from disk
void CTownManager::LoadTowns ( const RangeVector& ncenterposition, int nwidth )
{
	// Increment town map unused count. Erase anything with a high count.
	for ( vector<sTownUsageEntry>::iterator it = townMap.begin(); it != townMap.end(); )
	{
		if ( ++(it->unused_count) > 8 ) {
			it = townMap.erase(it);
		}
		else {
			++it;
		}
	}

	// Loop through area
	for ( rangeint nx = ncenterposition.x-nwidth; nx <= ncenterposition.x+nwidth; ++nx )
	{
		for ( rangeint ny = ncenterposition.y-nwidth; ny <= ncenterposition.y+nwidth; ++ny )
		{
			// First check current town map for the entry
			bool skip = false;
			for ( uint i = 0; i < townMap.size(); ++i ) {
				if ( (townMap[i].entry.position.x == nx) && (townMap[i].entry.position.y == ny) ) {
					// It's loaded, don't need to do anything but reset unused count.
					townMap[i].unused_count = 0;
					break;
				}
			}
			if ( skip ) {
				continue;
			}
			// Check the map file for the town
			sTownUsageEntry newTownEntry;
			newTownEntry.unused_count = 0;
			if ( TownSearchEntry( RangeVector(nx,ny,0), &(newTownEntry.entry) ) ) {
				// Add entry to the list
				townMap.push_back( newTownEntry );
			}
			// If there's still nothing, then no town exists at this point
		}
	}
}

bool CTownManager::GetTownAtSector( const RangeVector& inProvinceArea, sTownInfo* out_town_info )
{
	sTownUsageEntry newTownEntry;
	newTownEntry.unused_count = 0;
	if ( TownSearchEntry( inProvinceArea, &(newTownEntry.entry) ) ) {
		if ( out_town_info )
		{
			//*out_town_info = newTownEntry.entry;
			CBinaryFile file;
			TownOpenFile( inProvinceArea, file, false );
			if ( file.IsOpen() )
			{
				TownReadInfo( file, out_town_info );
			}
		}
		return true;
	}
	return false;
}



// TownOpenFile ( STRING townname, INTEGER initial_region_id, BINARYFILE file, BOOL create_file )
// TownOpenFile ( RANGEVECTOR position, BINARYFILE file, BOOL create_file )
// Opens the town file to read or write town information.
// On default, if the file does not exist, it will be created.
//void CTownManager::TownOpenFile ( arstring<256> town_name, uint32_t initial_region_id, CBinaryFile& file, const bool create_file )
void CTownManager::TownOpenFile ( const RangeVector& town_position, CBinaryFile& file, const bool create_file )
{
	char stemp_fn [256];
	if ( !file.IsOpen() ) {
		// Generate filename
		//sprintf( stemp_fn, "%s.towns/%d_%s", CGameSettings::Active()->GetTerrainSaveDir().c_str(), initial_region_id, town_name.c_str() ); 
		sprintf( stemp_fn, "%s.towns/t%d_%d", CGameSettings::Active()->GetTerrainSaveDir().c_str(), town_position.x, town_position.y ); 
		if ( IO::FileExists( stemp_fn ) ) {
			while ( !file.IsOpen() ) {
				file.OpenWait( stemp_fn, CBinaryFile::IO_READ|CBinaryFile::IO_WRITE );
			}
		}
		else if ( create_file ) {
			while ( !file.IsOpen() ) {
				file.OpenWait( stemp_fn, CBinaryFile::IO_READ|CBinaryFile::IO_WRITE );
			}

			memset( stemp_fn, 0, 256 );			// Set zero data
			file.WriteData( stemp_fn, 256 );	// Write 1024 bytes of no data
			file.WriteData( stemp_fn, 256 );
			file.WriteData( stemp_fn, 256 );
			file.WriteData( stemp_fn, 256 );
		}
	}
}
// TownOpenFileWrite ( STRING townname, INTEGER initial_region_id, BINARYFILE file, BOOL create_file )
// TownOpenFileWrite ( RANGEVECTOR position, BINARYFILE file, BOOL create_file )
// Opens the town file to write town information. Any existing information is deleted.
// On default, if the file does not exist, it will be created.
//void CTownManager::TownOpenFileWrite ( arstring<256> town_name, uint32_t initial_region_id, CBinaryFile& file, const bool create_file )
void CTownManager::TownOpenFileWrite ( const RangeVector& town_position, CBinaryFile& file, const bool create_file )
{
	char stemp_fn [256];
	if ( !file.IsOpen() ) {
		// Generate filename
		//sprintf( stemp_fn, "%s.towns/%d_%s", CGameSettings::Active()->GetTerrainSaveDir().c_str(), initial_region_id, town_name.c_str() ); 
		sprintf( stemp_fn, "%s.towns/t%d_%d", CGameSettings::Active()->GetTerrainSaveDir().c_str(), town_position.x, town_position.y ); 
		if ( IO::FileExists( stemp_fn ) ) {
			while ( !file.IsOpen() ) {
				file.OpenWait( stemp_fn, CBinaryFile::IO_WRITE );
			}
		}
		else if ( create_file ) {
			while ( !file.IsOpen() ) {
				file.OpenWait( stemp_fn, CBinaryFile::IO_WRITE );
			}

			memset( stemp_fn, 0, 256 );			// Set zero data
			file.WriteData( stemp_fn, 256 );	// Write 1024 bytes of no data
			file.WriteData( stemp_fn, 256 );
			file.WriteData( stemp_fn, 256 );
			file.WriteData( stemp_fn, 256 );
		}
	}
}
// TownFileExists ( RANGEVECTOR position )
// Checks if the given town exists. Doesn't not create the file.
bool CTownManager::TownFileExists ( const RangeVector& town_position )
{
	char stemp_fn [256];
	sprintf( stemp_fn, "%s.towns/t%d_%d", CGameSettings::Active()->GetTerrainSaveDir().c_str(), town_position.x, town_position.y ); 
	return IO::FileExists( stemp_fn );
}
// TownWriteInfo ( BINARYFILE file, TOWNINFO info_to_write )
// Writes town info in struct to file
void CTownManager::TownWriteInfo ( CBinaryFile& file, const sTownInfo* town_info )
{
	fseek( file.GetFILE(), 0, SEEK_SET );
	fwrite( town_info, sizeof(sTownInfo), 1, file.GetFILE() );
}
// TownWritePattern ( BINARYFILE file, PATTERN pattern_to_write )
// Writes town pattern to file
void CTownManager::TownWritePattern ( CBinaryFile& file, const sPattern* pattern )
{
	fseek( file.GetFILE(), 512, SEEK_SET );
	// Write properties
	fwrite( &(pattern->properties), 64, 1, file.GetFILE() );
	
	size_t		offset = 512+64;
	uint32_t	tempOut, i;

	// Write roads
	tempOut = pattern->roads.size();
	fseek( file.GetFILE(), offset, SEEK_SET );
	fwrite( &tempOut, 4,1, file.GetFILE() );
	offset += 4;
	for ( i = 0; i < tempOut; ++i ) {
		fseek( file.GetFILE(), offset, SEEK_SET );
		fwrite( &(pattern->roads[i]), 128, 1, file.GetFILE() );
		offset += 128;
	}
	// Write landmarks
	tempOut = pattern->landmarks.size();
	fseek( file.GetFILE(), offset, SEEK_SET );
	fwrite( &tempOut, 4,1, file.GetFILE() );
	offset += 4;
	for ( i = 0; i < tempOut; ++i ) {
		fseek( file.GetFILE(), offset, SEEK_SET );
		fwrite( &(pattern->landmarks[i]), 64, 1, file.GetFILE() );
		offset += 64;
	}
	// Write instances
	tempOut = pattern->instances.size();
	fseek( file.GetFILE(), offset, SEEK_SET );
	fwrite( &tempOut, 4,1, file.GetFILE() );
	offset += 4;
	for ( i = 0; i < tempOut; ++i ) {
		fseek( file.GetFILE(), offset, SEEK_SET );
		fwrite( &(pattern->instances[i]), 512, 1, file.GetFILE() );
		offset += 512;
	}
	// Write buildings
	tempOut = pattern->buildings.size();
	fseek( file.GetFILE(), offset, SEEK_SET );
	fwrite( &tempOut, 4,1, file.GetFILE() );
	offset += 4;
	for ( i = 0; i < tempOut; ++i ) {
		fseek( file.GetFILE(), offset, SEEK_SET );
		fwrite( &(pattern->buildings[i]), 256, 1, file.GetFILE() );
		offset += 256;
	}
}
// TownReadInfo ( BINARYFILE file, TOWNINFO info_to_use )
// Reads town info in struct from file
void CTownManager::TownReadInfo ( CBinaryFile& file, sTownInfo* town_info )
{
	fseek( file.GetFILE(), 0, SEEK_SET );
	fread( town_info, sizeof( sTownInfo ), 1, file.GetFILE() );
}
// TownReadPattern ( BINARYFILE file, PATTERN pattern_to_use )
// Reads town pattern from file
void CTownManager::TownReadPattern ( CBinaryFile& file, sPattern* pattern )
{
	fseek( file.GetFILE(), 512, SEEK_SET );
	
	// Read properties
	fread( &(pattern->properties), 64, 1, file.GetFILE() );

	size_t		offset = 512+64;
	uint32_t	tempIn, i;

	// Read roads
	pattern->roads.clear();
	fseek( file.GetFILE(), offset, SEEK_SET );
	fread( &tempIn, 4,1, file.GetFILE() );
	offset += 4;
	for ( i = 0; i < tempIn; ++i ) {
		pattern->roads.push_back( sPatternRoad() );
		fread( &(pattern->roads[i]), 128, 1, file.GetFILE() );
		offset += 128;
	}
	// Read landmarks
	pattern->landmarks.clear();
	fseek( file.GetFILE(), offset, SEEK_SET );
	fread( &tempIn, 4,1, file.GetFILE() );
	offset += 4;
	for ( i = 0; i < tempIn; ++i ) {
		pattern->landmarks.push_back( sPatternLandmark() );
		fread( &(pattern->landmarks[i]), 64, 1, file.GetFILE() );
		offset += 64;
	}
	// Read instances
	pattern->instances.clear();
	fseek( file.GetFILE(), offset, SEEK_SET );
	fread( &tempIn, 4,1, file.GetFILE() );
	offset += 4;
	for ( i = 0; i < tempIn; ++i ) {
		pattern->instances.push_back( sPatternInstance() );
		fread( &(pattern->instances[i]), 512, 1, file.GetFILE() );
		offset += 512;
	}
	// Read buildings
	pattern->buildings.clear();
	fseek( file.GetFILE(), offset, SEEK_SET );
	fread( &tempIn, 4,1, file.GetFILE() );
	offset += 4;
	for ( i = 0; i < tempIn; ++i ) {
		pattern->buildings.push_back( sPatternBuilding() );
		fread( &(pattern->buildings[i]), 256, 1, file.GetFILE() );
		offset += 256;
	}
}
// TownSaveEntry ( TOWN_ENTRY town )
// Saves town to the town map file.
void CTownManager::TownSaveEntry ( sTownEntry* town_entry )
{
	// Read in current list length
	uint32_t entryCount;
	fseek( townMapFile.GetFILE(), 0, SEEK_SET );
	fread( &entryCount, 4, 1, townMapFile.GetFILE() );
	// Write out incremented list length
	entryCount += 1;
	fseek( townMapFile.GetFILE(), 0, SEEK_SET );
	fwrite( &entryCount, 4, 1, townMapFile.GetFILE() );

	// Write new town entry
	fseek( townMapFile.GetFILE(), 512+512*entryCount, SEEK_SET );
	fwrite( town_entry, 512, 1, townMapFile.GetFILE() );
}
// TownSearchEntry ( RANGEVECTOR position, TOWN_ENTRY found_data )
// Searches for the town entry matching the position, and returns true if found.
// Saves the found entry's data into "found_data"
bool CTownManager::TownSearchEntry ( const RangeVector& town_position, sTownEntry* town_entry )
{
	mutex::scoped_lock( townMapFileLock );

	uint32_t entryCount;
	
	// Read in current list length
	fseek( townMapFile.GetFILE(), 0, SEEK_SET );
	entryCount = townMapFile.ReadUInt32();

	// Now read in the entries and search for the target
	{
		size_t	read_count = 0;
		size_t	read_size;
		while ( read_count < entryCount )
		{
			fseek( townMapFile.GetFILE(), 512+512*read_count, SEEK_SET );
			//read_size = townMapFile.ReadUCharArray( (uchar*)io_buffer,1024 );
			read_size = fread( io_buffer, 1, 1024, townMapFile.GetFILE() );
			read_size /= 512;
			for ( uint i = 0; i < std::min<uint32_t>( read_size, entryCount-read_count ); ++i )
			{
				sTownEntry* currentEntry = (sTownEntry*)( io_buffer+(i*512) );
				// Compare positions and return true when matches
				if ( (currentEntry->position.x == town_position.x) && (currentEntry->position.y == town_position.y) )
				{	// Copy data over before returning true
					if ( town_entry ) {	// Not given NULL
						memcpy( town_entry, currentEntry, sizeof(sTownEntry) );
					}
					return true;
				}
			}
			read_count += read_size;
			if ( read_size == 0 ) {
				Debug::Console->PrintWarning( "Town file corrupted (somehow). Fixing...\n" );
				fseek( townMapFile.GetFILE(), 0, SEEK_SET );
				entryCount = read_count;
				fwrite( &entryCount, sizeof(uint32_t), 1, townMapFile.GetFILE() );
			}
		}
	}

	return false;
}
