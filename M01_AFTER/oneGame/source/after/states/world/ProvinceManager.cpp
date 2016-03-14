//===============================================================================================//
//
//	class CRegionManager
// implementation of the town and region manager
//
// When writing functionality for this class, keep in mind that it will be run in a separate threads.
// That is THREADS, plural. This will have run of the main thread, simulation thread, and generation thread.
// In addition to those, it has its own thread that it uses to run tasks.
// Thus, IO functions must be checked, and interfaces with other parts of the game must be thread-safe.
//
//===============================================================================================//

#include "ProvinceManager.h"
/*#include "CTownManager.h"

#include "CBinaryFile.h"
#include "FileUtils.h"
#include "CVoxelTerrain.h"
#include "CTerraGenDefault.h"
#include "CGameSettings.h"
*/
#include "after/states/CWorldState.h"

#include "after/terrain/VoxelTerrain.h"
#include "after/terrain/generation/regions/CRegionGenerator.h"
/*#include "Zones.h"

#include "NamingUtils.h"
*/
#include <list>

// Additional World namespace
namespace World
{
	// Global pointer to active town manager
	CProvinceManager* ProvinceManager = NULL;

	// Constructor
	CProvinceManager::CProvinceManager( CVoxelTerrain* pTerrain )
		: CGameBehavior()//, terrain( pTerrain ), generator( pGenerator )
	{
		//ImageToFloorPlan( ".res/terra/b1.png" );

		/*regionCounter = 10;

		// Open region map
		char stemp_fn [256];

		if ( !regionFile.IsOpen() ) {
			sprintf( stemp_fn, "%s.regions/map", CGameSettings::Active()->MakeWorldSaveDirectory().c_str() );
			if ( IO::FileExists( stemp_fn ) ) {
				regionFile.Open( stemp_fn, CBinaryFile::IO_READ|CBinaryFile::IO_WRITE );
				regionIndex = regionFile.ReadUInt32();
			}
			else {
				regionFile.Open( stemp_fn, CBinaryFile::IO_READ|CBinaryFile::IO_WRITE );
				regionIndex = 1;
				regionFile.WriteUInt32( regionIndex );
			}
		}*/

		// Set active town manager.
		if ( ProvinceManager == NULL ) {
			ProvinceManager = this;
		}
	}

	CProvinceManager::~CProvinceManager ( void )
	{
		//regionFile.Close();

		// if this is the active town manager, clear it.
		if ( World::ProvinceManager == this ) {
			World::ProvinceManager = NULL;
		}
	}

	//	Update
	// Runs in the main game thread.
	// Will simulate events, such as world hysteria, revolts, rebellions, or other live events that need a frame by frame tracker.
	// Must only have very light calculations. Heavy calculations should go into Simulate().
	void CProvinceManager::Update ( void )
	{
		if ( ActiveGameWorld == NULL )
			return;		// We cannot perform work on towns without the world to keep track of it.
	}

	//	Simulate
	// Runs in the simulation side thread of the terrain thread.
	// Should simulate spread of culture and ideas.
	void CProvinceManager::Simulate ( void )
	{

	}

	//=========================================//
	// REGION QUERY
	//=========================================//

	//  GetRegion
	// Returns the region index that the given position belongs to.
	uint32_t CProvinceManager::GetRegion ( const rangeint& x, const rangeint& y )
	{
		return terrain->GetRegionGenerator()->Region_ClosestValidToIndex( WorldVector(x,y,0) );
	}

	//=========================================//
	// PROVINCE INFO QUERY
	//=========================================//

	//	GetProvinceInfo
	// Returns province info that's saved. Returns if true if valid data filled.
	bool CProvinceManager::GetProvinceInfo ( uint32_t n_region, province_properties_t* out_province_info )
	{
		throw Core::NotYetImplementedException();
	}
	//	GetProvinceName
	// Returns province name. Array should be 256 chars long. Returns if true if valid data filled.
	bool CProvinceManager::GetProvinceName ( uint32_t n_region, char* out_province_name )
	{
		throw Core::NotYetImplementedException();
	}

	//	Generate
	// Runs in the generation side thread of the terrain thread.
	// Given an input position, will ensure the regions near the positions are generated.
	// If it can not find the region file, it will create the region.
	//void CProvinceManager::Generate ( const RangeVector& position )
	//{
	//	char stemp_fn [256];

	//	// Find current region.
	//	{
	//		bool foundRegion = false;

	//		// Open region map
	//		if ( !regionFile.IsOpen() ) {
	//			sprintf( stemp_fn, "%s.regions/map", CGameSettings::Active()->MakeWorldSaveDirectory().c_str() );
	//			if ( IO::FileExists( stemp_fn ) ) {
	//				regionFile.Open( stemp_fn, CBinaryFile::IO_READ|CBinaryFile::IO_WRITE );
	//				regionIndex = regionFile.ReadUInt32();
	//			}
	//			else {
	//				regionFile.Open( stemp_fn, CBinaryFile::IO_READ|CBinaryFile::IO_WRITE );
	//				regionIndex = 1;
	//				regionFile.WriteUInt32( regionIndex );
	//			}
	//		}
	//		// Read region map
	//		if ( regionFile.IsOpen() ) {
	//			foundRegion = (RegionsSearch(position) != uint32_t(-1));
	//			// End while loop
	//		}
	//		else {
	//			// Create region file
	//			//regionFile.Open( stemp_fn, CBinaryFile::IO_WRITE );
	//			// print error
	//			cout << "Could not create region map file!!!" << endl;
	//		}

	//		// If cannot find region for this area
	//		if ( !foundRegion ) {
	//			cout << "Adding region (" << position.x << "," << position.y << "," << position.z << ") to region map. Commencing floodfill." << endl;
	//			// Floodfill for the area
	//			uint32_t resultRegion = RegionsFloodfill( position );

	//			{
	//				//mutex::scoped_lock( regionFileLock ); // LOCK FILE FOR WRITE
	//				regionFileLock.lock();
	//				{
	//					// Update the region index
	//					fseek( regionFile.GetFILE(), 0, SEEK_SET );
	//					regionFile.WriteUInt32( regionIndex );
	//				}
	//				regionFileLock.unlock();
	//			}

	//			// If valid region created, then give it properties
	//			if ( m_generateProperties && ( resultRegion != uint32_t(-1) )&&( resultRegion != uint32_t(-2) ))
	//			{
	//				RegionCreateProvince( resultRegion );
	//			}
	//		}

	//		// After X amount of generates, save the region file.
	//		if ( --regionCounter == 0 ) {
	//			regionCounter = 100;
	//			//mutex::scoped_lock( regionFileLock ); // LOCK FILE FOR REFRESH
	//			//boost::lock_guard<mutex> guard(regionFileLock);
	//			regionFileLock.lock();
	//			{
	//				regionFile.Close(); // This is how the region file is saved.
	//				// And open it again
	//				sprintf( stemp_fn, "%s.regions/map", CGameSettings::Active()->MakeWorldSaveDirectory().c_str() );
	//				regionFile.Open( stemp_fn, CBinaryFile::IO_READ|CBinaryFile::IO_WRITE );
	//			}
	//			regionFileLock.unlock();
	//		}
	//	}
	//	// End find current region.
	//}


	////  GetRegion
	//// Returns the region index that the given position belongs to.
	//uint32_t	CRegionManager::GetRegion ( const rangeint& x, const rangeint& y )
	//{
	//	return RegionsSearch( RangeVector( x,y,0 ) ); // RegionsSearch is THREAD SAFE.
	//}

	////	GetProvinceArea
	//// Returns all the areas in the given province, along with their data.
	//// Returns count of areas in the given region
	//uint32_t	CRegionManager::GetProvinceArea ( uint32_t n_region, vector<sProvinceAreaEntry>& out_province_area )
	//{
	//	CBinaryFile file;

	//	// Open province file
	//	ProvinceOpenFile( n_region, file, false );

	//	// Clear list
	//	out_province_area.clear();

	//	// Read in data
	//	if ( file.IsOpen() )
	//	{
	//		// Seek to the beginning of the region data
	//		fseek( file.GetFILE(), 512, SEEK_SET );

	//		size_t read_size;
	//		size_t read_count = 0;
	//		uint32_t	max_read_count;
	//		char buffer [1024];

	//		// Read in the data
	//		max_read_count = file.ReadUInt32();

	//		// Read in until hit the max region count
	//		while ( read_count < max_read_count )
	//		{
	//			// Search file for areas, buffer at a time
	//			read_size = file.ReadUCharArray( (uchar*)buffer, 1024 );
	//			read_size /= sizeof(sProvinceAreaEntry);
	//			// Add loaded data to list
	//			for ( uint i = 0; i < std::min<uint32_t>(read_size,max_read_count-read_count); ++i ) {
	//				out_province_area.push_back( ((sProvinceAreaEntry*)(buffer))[i] );
	//			}
	//			// Increase read count
	//			read_count += read_size;
	//		}

	//		// Return list size
	//		return (uint32_t)out_province_area.size();
	//	}
	//	return 0;
	//}

	////	GetProvinceTownAreas
	//// Populates a vector of sProvinceAreaEntry with all the town areas in the province.
	//// Can be used in conjuction with the town manager.
	//// Returns number of areas.
	//uint32_t CRegionManager::GetProvinceTownAreas ( uint32_t n_region, vector<sProvinceAreaEntry>& out_province_area )
	//{
	//	CBinaryFile file;

	//	// Open province file
	//	ProvinceOpenFile( n_region, file, false );

	//	// Clear list
	//	out_province_area.clear();

	//	// Read in data
	//	if ( file.IsOpen() )
	//	{
	//		// Seek to the beginning of the region data
	//		fseek( file.GetFILE(), 512, SEEK_SET );

	//		size_t read_size;
	//		size_t read_count = 0;
	//		uint32_t	max_read_count;
	//		char buffer [1024];

	//		// Read in the data
	//		max_read_count = file.ReadUInt32();

	//		// Read in until hit the max region count
	//		while ( read_count < max_read_count )
	//		{
	//			// Search file for areas, buffer at a time
	//			read_size = file.ReadUCharArray( (uchar*)buffer, 1024 );
	//			read_size /= sizeof(sProvinceAreaEntry);
	//			// Add loaded data to list
	//			for ( uint i = 0; i < std::min<uint32_t>(read_size,max_read_count-read_count); ++i ) {
	//				if ( ((sProvinceAreaEntry*)(buffer))[i].info & 0x01 ) {
	//					out_province_area.push_back( ((sProvinceAreaEntry*)(buffer))[i] );
	//				}
	//			}
	//			// Increase read count
	//			read_count += read_size;
	//		}

	//		// Return list size
	//		return (uint32_t)out_province_area.size();
	//	}
	//	return 0;
	//}

	////	GetProvinceHasTowns
	//// Returns if province has had town generation.
	//uint8_t	CRegionManager::GetProvinceHasTowns ( uint32_t n_region )
	//{
	//	CBinaryFile file;
	//	// Open province file
	//	ProvinceOpenFile( n_region, file, false );
	//	if ( file.IsOpen() )
	//	{
	//		// Seek to pos
	//		fseek( file.GetFILE(), 256, SEEK_SET );
	//		// Read data
	//		uint8_t state;
	//		fread( &state, sizeof(uint8_t), 1, file.GetFILE() );
	//		return state;
	//	}
	//	else {
	//		return 255;
	//	}
	//}
	////	SetProvinceHasTowns
	//// Sets if province has had town generation.
	//void	CRegionManager::SetProvinceHasTowns ( uint32_t n_region, uint8_t n_towns_exist )
	//{
	//	CBinaryFile file;
	//	// Open province file
	//	ProvinceOpenFile( n_region, file, false );
	//	if ( file.IsOpen() )
	//	{
	//		// Seek to position
	//		fseek( file.GetFILE(), 256, SEEK_SET );
	//		// Write to data
	//		fwrite( &n_towns_exist, sizeof(uint8_t), 1, file.GetFILE() );
	//	}
	//}

	////	GetProvinceInfo
	//// Returns province info that's saved. Returns if true if valid data filled.
	//bool	CRegionManager::GetProvinceInfo ( uint32_t n_region, sProvinceInfo* out_province_info )
	//{
	//	CBinaryFile file;
	//	// Open province file
	//	ProvinceOpenFile( n_region, file, false );
	//	if ( file.IsOpen() )
	//	{
	//		// Seek to pos
	//		fseek( file.GetFILE(), 256+64, SEEK_SET );
	//		// Read data
	//		fread( out_province_info, sizeof(sProvinceInfo), 1, file.GetFILE() );
	//		return true;
	//	}
	//	else {
	//		return false;
	//	}
	//}
	////	SetProvinceInfo
	//// Sets and saves province info.
	//void	CRegionManager::SetProvinceInfo ( uint32_t n_region, const sProvinceInfo* n_province_info )
	//{
	//	CBinaryFile file;
	//	// Open province file
	//	ProvinceOpenFile( n_region, file, false );
	//	if ( file.IsOpen() )
	//	{
	//		// Seek to position
	//		fseek( file.GetFILE(), 256+64, SEEK_SET );
	//		// Write to data
	//		fwrite( n_province_info, sizeof(sProvinceInfo), 1, file.GetFILE() );
	//	}
	//}

	////	GetProvinceName
	//// Returns province name. Array should be 256 chars long. Returns if true if valid data filled.
	//bool	CRegionManager::GetProvinceName ( uint32_t n_region, char* out_province_name )
	//{
	//	CBinaryFile file;
	//	// Open province file
	//	ProvinceOpenFile( n_region, file, false );
	//	if ( file.IsOpen() )
	//	{
	//		// Seek to pos
	//		fseek( file.GetFILE(), 0, SEEK_SET );
	//		// Read data
	//		fread( out_province_name, 256, 1, file.GetFILE() );
	//		return true;
	//	}
	//	else {
	//		return false;
	//	}
	//}
	////	SetProvinceName
	//// Sets and saves province name.
	//void	CRegionManager::SetProvinceName ( uint32_t n_region, const char* n_province_name )
	//{
	//	CBinaryFile file;
	//	// Open province file
	//	ProvinceOpenFile( n_region, file, false );
	//	if ( file.IsOpen() )
	//	{
	//		// Seek to position
	//		fseek( file.GetFILE(), 0, SEEK_SET );
	//		// Write to data
	//		fwrite( n_province_name, 256, 1, file.GetFILE() );
	//	}
	//}





	//// RegionsFloodfill ( RANGEVECTOR source position )
	//// Generates a region given a position to start with.
	//// Will call RegionsAdd on regions it finds that are not part of a region.
	//// Map file must already be open.
	//// Returns the ID of the region created, -2 if no region, or -1 if failed.
	//uint32_t CRegionManager::RegionsFloodfill ( const RangeVector& sourcePosition )
	//{
	//	// So, here's how it goes. We floodfill.
	//	uint32_t regionSize = 0;

	//	// First, need the source color.
	//	uchar sourceBiome = generator->TerraGen_priv_GetBiome( Vector3d(sourcePosition.x+0.5f,sourcePosition.y+0.5f,0) );
	//	uchar sourceTerra = generator->iCurrentTerraType;

	//	if ( sourceTerra == TER_OCEAN || sourceTerra == TER_THE_EDGE ) {
	//		// Add position to regions as unused.
	//		RegionsAdd( sourcePosition, uint32_t(-2) );
	//		return uint32_t(-2);
	//	}

	//	// Now, floodfill using a queue
	//	list<RangeVector> floodQueue;
	//	floodQueue.push_back( sourcePosition );
	//	while ( !floodQueue.empty() )
	//	{
	//		// Get the current value on the queue
	//		RangeVector currentPosition = floodQueue.front();
	//		floodQueue.pop_front();
	//		// Check target color
	//		uchar targetBiome = generator->TerraGen_priv_GetBiome( Vector3d(currentPosition.x+0.5f,currentPosition.y+0.5f,0) );
	//		uchar targetTerra = generator->iCurrentTerraType;

	//		// Generate differences
	//		int majorDif = Zones.TerraCompare( sourceBiome, targetBiome );
	//		int minorDif = Zones.TerraCompare( sourceTerra, targetTerra );
	//		// Distance adds 1 to majorDif. Regions shouldn't be too large.
	//		int distDif = (int)( Vector2d((ftype)(sourcePosition.x-currentPosition.x),(ftype)(sourcePosition.y-currentPosition.y)).magnitude() * 0.03f );
	//		majorDif += distDif;
	//	
	//		// On majorDif, 4 is should stop. 5 is should definitely stop.
	//		// Same for minorDif is majorDif is 2 or smaller.
	//		if ( majorDif <= 2 ) {
	//			majorDif = minorDif+distDif;
	//		}

	//		bool addRegion = true;
	//		if ( majorDif == 4 ) {
	//			// Chance to stop with this region.
	//			if ( generator->Trd_Noise_Sample(currentPosition.x*3.3f,currentPosition.y*4.4f,majorDif*9.1f+minorDif*1.7f) > 0.0f ) {
	//				addRegion = false;
	//			}
	//		}
	//		else if ( majorDif >= 5 ) {
	//			// Always stop with this region
	//			addRegion = false;
	//		}
	//		// Check that the area is not already in the regions
	//		if ( addRegion ) {
	//			if ( RegionsSearch(currentPosition) != uint32_t(-1) ) {
	//				addRegion = false;
	//			}
	//		}

	//		// Add the region
	//		if ( addRegion )
	//		{
	//			regionSize += 1;
	//			RegionsAdd( currentPosition, regionIndex );
	//			// Add its neighbors to the floodfill algorithm.
	//			RangeVector addPosition;
	//			addPosition = RangeVector(currentPosition.x-1,currentPosition.y,0);
	//			//if ( std::find( floodQueue.begin(), floodQueue.end(), addPosition ) == floodQueue.end() ) {
	//				floodQueue.push_back( addPosition );
	//			//}
	//			addPosition = RangeVector(currentPosition.x+1,currentPosition.y,0);
	//			//if ( std::find( floodQueue.begin(), floodQueue.end(), addPosition ) == floodQueue.end() ) {
	//				floodQueue.push_back( addPosition );
	//			//}
	//			addPosition = RangeVector(currentPosition.x,currentPosition.y-1,0);
	//			//if ( std::find( floodQueue.begin(), floodQueue.end(), addPosition ) == floodQueue.end() ) {
	//				floodQueue.push_back( addPosition );
	//			//}
	//			addPosition = RangeVector(currentPosition.x,currentPosition.y+1,0);
	//			//if ( std::find( floodQueue.begin(), floodQueue.end(), addPosition ) == floodQueue.end() ) {
	//				floodQueue.push_back( addPosition );
	//			//}
	//		}
	//	}
	//
	//	// If the region is too small, it needs to take the neighbor regions.
	//	//if ( regionSize <= 5 ) // a kingdom should be at LEAST 20,000 sq feet
	//	// FUQ THAT. A KINGDOM SHOULD BE BIGGER
	//	m_generateProperties = true;
	//	if ( regionSize <= 13 )
	//	{
	//		// Look for nearby region.
	//		RangeVector newPosition = RangeVector(sourcePosition.x-1,sourcePosition.y,0);
	//		uint32_t nearRegion = RegionsSearch( newPosition );
	//		if ( (nearRegion == uint32_t(-1))||(nearRegion == uint32_t(-2)||(nearRegion == regionIndex)) ) {
	//			newPosition = RangeVector(sourcePosition.x+1,sourcePosition.y,0);
	//			nearRegion = RegionsSearch( newPosition );
	//		}
	//		if ( (nearRegion == uint32_t(-1))||(nearRegion == uint32_t(-2)||(nearRegion == regionIndex)) ) {
	//			newPosition = RangeVector(sourcePosition.x,sourcePosition.y-1,0);
	//			nearRegion = RegionsSearch( newPosition );
	//		}
	//		if ( (nearRegion == uint32_t(-1))||(nearRegion == uint32_t(-2)||(nearRegion == regionIndex)) ) {
	//			newPosition = RangeVector(sourcePosition.x,sourcePosition.y+1,0);
	//			nearRegion = RegionsSearch( newPosition );
	//		}
	//		// Select region
	//		if ( (nearRegion != uint32_t(-1))&&(nearRegion != uint32_t(-2)&&(nearRegion != regionIndex)) ) {
	//			// Make it the region
	//			RegionsSetFromRegion( regionIndex, nearRegion );
	//			cout << "  Setting " << regionIndex << " to " << nearRegion << endl;
	//			m_generateProperties = false;
	//			// Return that region
	//			return nearRegion;
	//		}
	//		else
	//		{
	//			if ( nearRegion == regionIndex ) {
	//				// Why are we here?
	//			}
	//			else if ( nearRegion == uint32_t(-1) )
	//			{
	//				// Generate that region
	//				RegionsFloodfill( newPosition );
	//				// Get the id of the region generated.
	//				nearRegion = RegionsSearch( newPosition );
	//				RegionsSetFromRegion( regionIndex, nearRegion );
	//				cout << "  Setting " << regionIndex << " to " << nearRegion << endl;
	//				m_generateProperties = false;
	//				// Return that region
	//				return nearRegion;
	//			}
	//			else 
	//			{
	//				// Set to invalid region
	//				RegionsSetFromRegion( regionIndex, uint32_t(-2) );
	//				// Return invalid area.
	//				return uint32_t(-2);
	//			}
	//		}
	//	}

	//	// Increment region index
	//	regionIndex += 1;

	//	// Return region index-1
	//	return regionIndex-1;
	//}
	//// RegionsAdd ( RANGEVECTOR position, INTEGER region_id )
	//// Adds a position to the region map file. Map file must already be open.
	//uint32_t CRegionManager::RegionsAdd ( const RangeVector& position, uint32_t region_id )
	//{
	//	uint32_t resultRegion;
	//	{
	//		// put region search into function
	//		resultRegion = RegionsSearch(position);
	//		if ( resultRegion != uint32_t(-1) ) {
	//			return resultRegion;
	//		}
	//	}
	//
	//	{
	//		//mutex::scoped_lock( regionFileLock ); // LOCK FILE FOR WRITE
	//		boost::lock_guard<mutex> guard(regionFileLock);

	//		fseek( regionFile.GetFILE(), 0, SEEK_END );
	//
	//		sRegionMapEntry newRegionEntry;
	//		newRegionEntry.position = position;
	//		newRegionEntry.region = region_id;

	//		regionFile.WriteData( (char*)&newRegionEntry, sizeof(sRegionMapEntry) );
	//	
	//		return region_id;
	//	}
	//}
	//// RegionsSearch ( RANGEVECTOR position )
	//// Searches for a position in the region map file. Map file must already be open.
	//// The position's array is returned. If it cannot be found, uint32_t(-1) is returned.
	//uint32_t CRegionManager::RegionsSearch ( const RangeVector& position )
	//{
	//	//mutex::scoped_lock( regionFileLock ); // LOCK FILE FOR SEEK
	//	boost::lock_guard<mutex> guard(regionFileLock);

	//	size_t	read_size;

	//	if ( !regionFile.IsOpen() ) {
	//		Debug::Console->PrintError( "HOLY SHIT, HOW DID YOU GET A CLOSED REGION FILE HERE?!\n" );
	//		return (uint32_t)(-1);
	//	}

	//	fseek( regionFile.GetFILE(), 4, SEEK_SET );
	//	while ( !feof( regionFile.GetFILE() ) ) //todo: this line!
	//	{
	//		// Search file for regions, buffer at a time
	//		read_size = regionFile.ReadUCharArray( (uchar*)bufferGeneration, 1024 );
	//		read_size /= sizeof(sRegionMapEntry);
	//		// Search through loaded data
	//		for ( uint i = 0; i < read_size; ++i ) {
	//			// Check XY position for matching region (NO 3D REGIONS)
	//			if (( ((sRegionMapEntry*)(bufferGeneration))[i].position.x == position.x )
	//				&&( ((sRegionMapEntry*)(bufferGeneration))[i].position.y == position.y )) {
	//				return ((sRegionMapEntry*)(bufferGeneration))[i].region;
	//			}
	//		}
	//	}
	//	// End while loop
	//	return (uint32_t)(-1);
	//}
	//// RegionsSet ( RANGEVECTOR position, INTEGER region_id )
	//// Sets a position in the region map file to the given region. Map file must already be open.
	//// If the position isn't in the region map, calls RegionsAdd.
	//// The region that the position actually got is returned.
	//uint32_t CRegionManager::RegionsSet ( const RangeVector& position, uint32_t region_id )
	//{

	//	{
	//		//mutex::scoped_lock( regionFileLock ); // LOCK FILE FOR WRITE
	//		boost::lock_guard<mutex> guard(regionFileLock);

	//		size_t	read_size;
	//		size_t	read_count = 0;

	//		fseek( regionFile.GetFILE(), 4, SEEK_SET );
	//		while ( !feof( regionFile.GetFILE() ) )
	//		{
	//			// Search file for regions, buffer at a time
	//			read_size = regionFile.ReadUCharArray( (uchar*)bufferGeneration, 1024 );
	//			read_size /= sizeof(sRegionMapEntry);
	//			// Search through loaded data
	//			for ( uint i = 0; i < read_size; ++i ) {
	//				// Check XY position for matching region (NO 3D REGIONS)
	//				if (( ((sRegionMapEntry*)(bufferGeneration))[i].position.x == position.x )
	//					&&( ((sRegionMapEntry*)(bufferGeneration))[i].position.y == position.y )) {
	//					// If it matches, seek to the region entry
	//					((sRegionMapEntry*)(bufferGeneration))[i].region = region_id;
	//					fseek( regionFile.GetFILE(), 4 + 1024*read_count, SEEK_SET );
	//					fwrite( bufferGeneration, read_size*sizeof(sRegionMapEntry), 1,regionFile.GetFILE() );
	//					// Return target region
	//					return region_id;
	//				}
	//			}
	//			// Increase read count 
	//			read_count += 1;
	//		}
	//	} 

	//	// If still here, then didn't find the region, so add it.
	//	return RegionsAdd( position, region_id );
	//}

	//// RegionsSetFromRegion ( INTEGER old_region_id, INTEGER new_region_id );
	//// Sets all regions with the given region ID to the new region. Map file must already be open.
	//void CRegionManager::RegionsSetFromRegion ( uint32_t old_region_id, uint32_t new_region_id )
	//{
	//	if ( old_region_id == new_region_id ) {
	//		cout << "DUPED REGIONS. PROBABLY USED RegionsSetFromRegion AS A FALLBACK. SKIPPING." << endl;
	//		return;
	//	}

	//	{
	//		//mutex::scoped_lock( regionFileLock ); // LOCK FILE FOR READ/WRITE
	//		boost::lock_guard<mutex> guard(regionFileLock);

	//		size_t	read_size;
	//		size_t	read_count = 0;
	//		bool	has_change = false;

	//		while ( !feof( regionFile.GetFILE() ) )
	//		{
	//			// Seek to target
	//			fseek( regionFile.GetFILE(), 4 + 1024*read_count, SEEK_SET );
	//			// Search file for regions, buffer at a time
	//			read_size = regionFile.ReadUCharArray( (uchar*)bufferGeneration, 1024 );
	//			read_size /= sizeof(sRegionMapEntry);
	//			// Reset dirty flag
	//			has_change = false;
	//			// Search through loaded data
	//			for ( uint i = 0; i < read_size; ++i ) {
	//				// Check for matching regions.
	//				if (( ((sRegionMapEntry*)(bufferGeneration))[i].region == old_region_id ))
	//				{
	//					// If it matches, set new region
	//					((sRegionMapEntry*)(bufferGeneration))[i].region = new_region_id;
	//					// Mark dirty
	//					has_change = true;
	//				}
	//			}
	//			// Write changes back to file if dirty
	//			if ( has_change )
	//			{
	//				fseek( regionFile.GetFILE(), 4 + 1024*read_count, SEEK_SET );
	//				fwrite( bufferGeneration, read_size*sizeof(sRegionMapEntry), 1,regionFile.GetFILE() );
	//			}
	//			// Increase read count 
	//			read_count += 1;
	//		}
	//	} 
	//}

	//// RegionsCreateProvince ( INTEGER region_id )
	//// Searches for all regions with the given region ID and adds them to their province file.
	//// Map file must already be open.
	//// Also gives the provice basic starting properties for culture.
	//void CRegionManager::RegionCreateProvince ( uint32_t region_id )
	//{
	//	// Generate base file
	//	{
	//		//mutex::scoped_lock( regionFileLock ); // LOCK FILE FOR READ/WRITE
	//		boost::lock_guard<mutex> guard(regionFileLock);
	//
	//		size_t	read_size;
	//		size_t	read_count = 0;
	//
	//		CBinaryFile province;
	//		ProvinceOpenFile( region_id, province );

	//		while ( !feof( regionFile.GetFILE() ) )
	//		{
	//			// Seek to target
	//			fseek( regionFile.GetFILE(), 4 + 1024*read_count, SEEK_SET );
	//			// Search file for regions, buffer at a time
	//			read_size = regionFile.ReadUCharArray( (uchar*)bufferGeneration, 1024 );
	//			read_size /= sizeof(sRegionMapEntry);
	//			// Search through loaded data
	//			for ( uint i = 0; i < read_size; ++i ) {
	//				// Check for matching regions.
	//				if ( ((sRegionMapEntry*)(bufferGeneration))[i].region == region_id )
	//				{
	//					// If it matches, set new region
	//					ProvinceAdd( ((sRegionMapEntry*)(bufferGeneration))[i].position, province );
	//				}
	//			}
	//			// Increase read count 
	//			read_count += 1;
	//		}
	//	}
	//	// Generate region properties
	//	{
	//		// First get list of areas
	//		vector<sProvinceAreaEntry> areaList;
	//		GetProvinceArea( region_id, areaList );
	//		if ( areaList.empty() ) {
	//			cout << "Empty area list" << endl;
	//			return;
	//		}

	//		sProvinceInfo provinceInfo;
	//	
	//		// Generate culture values
	//		provinceInfo.culture[World::CULTURE_EUROPEAN]	=
	//			generator->noise->Unnormalize(generator->noise_terra->Get3D( generator->vMainSamplerOffset.x - region_id * 0.70f, (areaList[0].x+areaList[0].y)*0.1f, 127.32f ))+0.5f;
	//		provinceInfo.culture[World::CULTURE_ASIAN]		=
	//			generator->noise->Unnormalize(generator->noise_terra->Get3D( generator->vMainSamplerOffset.y - region_id * 0.71f, (areaList[0].x+areaList[0].y)*0.1f, 265.43f ))+0.5f;
	//		provinceInfo.culture[World::CULTURE_ARCANA]		= // Arcana is more rare
	//			(generator->noise->Unnormalize(generator->noise_terra->Get3D( generator->vMainSamplerOffset.z - region_id * 0.72f, (areaList[0].x+areaList[0].y)*0.1f, 378.23f ))+0.5f)*0.27f;
	//		// Normalize the culture values to 1
	//		ftype maxValue = 0;
	//		for ( uint i = 0; i < 3; ++i ) {
	//			maxValue += provinceInfo.culture[i];
	//		}
	//		maxValue = 1/maxValue;
	//		for ( uint i = 0; i < 3; ++i ) {
	//			provinceInfo.culture[i] *= maxValue;
	//		}

	//		// Create general opnions
	//		provinceInfo.magi_opinion	= generator->noise_biome->Get3D( (areaList[0].x+areaList[0].y)*0.1f, region_id * -0.50f, 154.76f ) * 0.2f;
	//		provinceInfo.tech_opinion	= generator->noise_biome->Get3D( (areaList[0].x+areaList[0].y)*0.1f, region_id * -0.51f, 223.64f ) * 0.2f;

	//		// Save culture settings
	//		SetProvinceInfo( region_id, &provinceInfo );

	//		// Output debug info
	//		cout << "Saved region " << region_id << " with info: " << endl;
	//		cout << "  CULTURE_EURO:   " << provinceInfo.culture[World::CULTURE_EUROPEAN] << endl;
	//		cout << "  CULTURE_ASIAN:  " << provinceInfo.culture[World::CULTURE_ASIAN] << endl;
	//		cout << "  CULTURE_ARCANA: " << provinceInfo.culture[World::CULTURE_ARCANA] << endl;
	//		cout << "  MAGI_OPINION: " << provinceInfo.magi_opinion << endl;
	//		cout << "  TECH_OPINION: " << provinceInfo.tech_opinion << endl;

	//		// Generate region name (TODO: look at surrounding area to get name!)
	//		SetProvinceName( region_id, NamingUtils::CreateBaseTownName() );

	//	}
	//}

	//// SectorSetHasTowns ( RANGEVECTOR position, BOOL new_value )
	//// If the given sector is in a region, sets that it contains a town.
	///*void CRegionManager::SectorSetHasTowns ( const RangeVector& position, bool has_towns )
	//{
	//	uint32_t region = RegionsSearch( position );
	//	if ( region != uint32_t(-1) )
	//	{
	//	
	//	}
	//}*/

	//// ============================================================================
	//// ====== PROVINCE CODE
	////

	//// ProvinceOpenFile ( INTEGER region_id, BINARYFILE file )
	//// Opens the region file to read or write province information.
	//// If the file does not exist, it will be created.
	//void CRegionManager::ProvinceOpenFile ( uint32_t region_id, CBinaryFile& file, const bool create_file )
	//{
	//	char stemp_fn [256];
	//	// Open province file map
	//	if ( !file.IsOpen() ) {
	//		sprintf( stemp_fn, "%s.regions/province_%d", CGameSettings::Active()->MakeWorldSaveDirectory().c_str(), region_id );	// Generate filename
	//		if ( IO::FileExists( stemp_fn ) ) {
	//			file.Open( stemp_fn, CBinaryFile::IO_READ|CBinaryFile::IO_WRITE );
	//		}
	//		else if ( create_file ) {
	//			file.Open( stemp_fn, CBinaryFile::IO_READ|CBinaryFile::IO_WRITE );
	//			file.WriteData( stemp_fn, 256 );	// Write filename as name

	//			memset( stemp_fn, 0, 256 );			// Set buffer to zero
	//			file.WriteData( stemp_fn, 256 );	// Write 256 of empty data

	//			file.WriteUInt64( 0 );				// Write zero for initial area count
	//		}
	//	}
	//}
	//// ProvinceAdd ( RANGEVECTOR position, BINARYFILE file )
	//// Adds the given position to the province file. Province file must be open.
	//void CRegionManager::ProvinceAdd ( const RangeVector& position, CBinaryFile& file )
	//{
	//	if ( !ProvinceSearch( position, file ) )
	//	{
	//		// Read current region count
	//		uint32_t regionCount;
	//		fseek( file.GetFILE(), 512, SEEK_SET );
	//		regionCount = file.ReadUInt32();

	//		// Add province data to the end.
	//		fseek( file.GetFILE(), 512 + 4 + (regionCount)*sizeof(sProvinceAreaEntry), SEEK_SET );
	//		sProvinceAreaEntry newEntry;
	//		{
	//			newEntry.x = position.x;
	//			newEntry.y = position.y;
	//			newEntry.resources[0] = 0;
	//			newEntry.resources[1] = 0;
	//			newEntry.resources[2] = 0;
	//			newEntry.resources[3] = 0;
	//			newEntry.info = 0;
	//		}
	//		file.WriteData( (char*)(&newEntry), sizeof(sProvinceAreaEntry) );

	//		// Increment province data count.
	//		regionCount += 1;
	//		fseek( file.GetFILE(), 512, SEEK_SET );
	//		file.WriteUInt32( regionCount );
	//	}
	//}
	//// ProvinceSetName ( STRING name, BIRANYFILE file )
	//// Sets the name of the province. Province file must be open.
	//void CRegionManager::ProvinceSetName ( const char* name, CBinaryFile& file )
	//{
	//	// Seek to the beginnning of the file.
	//	fseek( file.GetFILE(), 0, SEEK_SET );
	//	// Write over the name area.
	//	file.WriteData( name, strlen(name)+1 );
	//}


	//// ProvinceSearch ( RANGEVECTOR position, BINRARYFILE file )
	//// Searches the given province for the given area. Province file must be open.
	//// Returns true the the area is part of the province, false otherwise.
	//bool CRegionManager::ProvinceSearch ( const RangeVector& position, CBinaryFile& file )
	//{
	//	// Seek to the beginning of the region data
	//	fseek( file.GetFILE(), 512, SEEK_SET );

	//	size_t read_size;
	//	size_t read_count = 0;
	//	uint32_t	max_read_count;
	//	char buffer [1024];

	//	// Read in the data
	//	max_read_count = file.ReadUInt32();

	//	// Read in until hit the max region count
	//	while ( read_count < max_read_count )
	//	{
	//		//fnl_assert( feof(file.GetFILE()) == 0 );
	//		// Search file for areas, buffer at a time
	//		size_t read_size_raw;
	//		read_size_raw = file.ReadUCharArray( (uchar*)buffer, 1024 ); // ReadUChar is used because it is done piecewise as opposed to total failure.
	//		read_size = read_size_raw / sizeof(sProvinceAreaEntry);
	//		// Search through loaded data
	//		for ( uint i = 0; i < std::min<uint32_t>(read_size,max_read_count-read_count); ++i ) {
	//			if (( ((sProvinceAreaEntry*)(buffer))[i].x == position.x )
	//			   &&(((sProvinceAreaEntry*)(buffer))[i].y == position.y )) {
	//					// Found it, return true.
	//				return true;
	//			}
	//		}
	//		// Increase read count
	//		read_count += read_size;
	//		// Quick hack here
	//		/*if ( read_size <= 0 ) {
	//			Debug::Console->PrintError( "Overflow on province!\n" );
	//			break;
	//		}*/
	//	}
	//	// Didn't find it, return false.
	//	return false;
	//}
	//// ProvinceSectorSetTowns ( RANGEVECTOR position, BINARYFILE file, BOOL has_town )
	//// Searchs the given province for the given area. Upon finding it, sets if area has a town.
	//// Returns true if province was found and the value was set to the new value.
	//bool CRegionManager::ProvinceSectorSetTowns ( const RangeVector& position, CBinaryFile& file, bool n_has_towns )
	//{
	//	// Seek to the beginning of the region data
	//	fseek( file.GetFILE(), 512, SEEK_SET );

	//	size_t read_size;
	//	size_t read_count = 0;
	//	uint32_t	max_read_count;
	//	char buffer [1024];

	//	// Read in the data
	//	max_read_count = file.ReadUInt32();

	//	// Read in until hit the max region count
	//	while ( read_count < max_read_count )
	//	{
	//		// Search file for areas, buffer at a time
	//		read_size = file.ReadUCharArray( (uchar*)buffer, 1024 ); // ReadUChar is used because it is done piecewise as opposed to total failure.
	//		read_size /= sizeof(sProvinceAreaEntry);
	//		// Search through loaded data
	//		for ( uint i = 0; i < std::min<uint32_t>(read_size,max_read_count-read_count); ++i ) {
	//			sProvinceAreaEntry& entry = ((sProvinceAreaEntry*)(buffer))[i];
	//			if ( ( entry.x == position.x )
	//			   &&( entry.y == position.y ))
	//			{	// Found it.
	//				// Change the town value
	//				if ( n_has_towns ) {
	//					entry.info |= 0x01;
	//				}
	//				else {
	//					entry.info &= ~0x01;
	//				}
	//				// Write the data back
	//				fseek( file.GetFILE(), 512 + sizeof(sProvinceAreaEntry)*(read_count+i), SEEK_SET );
	//				file.WriteData( (char*)&(((sProvinceAreaEntry*)(buffer))[i]), sizeof(sProvinceAreaEntry) );
	//				// Return true since found.
	//				return true;
	//			}
	//		}
	//		// Increase read count
	//		read_count += read_size;
	//	}
	//	// Didn't find it, return false.
	//	return false;
	//}

};