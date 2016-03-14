
#include <algorithm>
#include "VoxelTerrain.h"

#include "after/terrain/io/COctreeIO.h"
#include "after/terrain/system/MemoryManager.h"
#include "after/terrain/system/DataSampler.h"
#include "after/terrain/system/TerrainRenderer.h"
#include "after/terrain/system/JobHandler.h"

#include "after/terrain/generation/CWorldGenerator.h"
#include "after/terrain/generation/patterns/CPatternController.h"
#include "after/terrain/generation/regions/CRegionGenerator.h"

// define the terrain list
std::vector<CVoxelTerrain*>	CVoxelTerrain::TerrainList;

// add self to the list
CVoxelTerrain::CVoxelTerrain ( void )
	: CGameBehavior(), IO(m_io), Memory(m_memory), Sampler(m_sampler), Renderer(m_renderer)
{
	_Init();
	// add to list after init
	TerrainList.push_back( this );
}
// remove self from the list
CVoxelTerrain::~CVoxelTerrain ( void )
{
	auto listPosition = std::find( TerrainList.begin(), TerrainList.end(), this );
	if ( listPosition == TerrainList.end() ) {
		throw Core::InvalidCallException();
	}
	else {
		TerrainList.erase(listPosition);
		if ( !TerrainList.empty() ) {
			TerrainList[0]->MakeActive();
		}
	}
	// after removed from list, free
	_Free();
}
// move self to the front of the list
void CVoxelTerrain::MakeActive ( void )
{
	auto listPosition = std::find( TerrainList.begin(), TerrainList.end(), this );
	if ( listPosition == TerrainList.end() ) {
		throw Core::InvalidCallException();
	}
	else {
		std::iter_swap( TerrainList.begin(), listPosition );
	}
}
// return 0th terrain or NULL
CVoxelTerrain* CVoxelTerrain::GetActive ( void )
{
	if ( !TerrainList.empty() ) {
		return TerrainList[0];
	}
	else {
		return NULL;
	}
}

//=========================================//
// System State

//		_Init
void CVoxelTerrain::_Init ( void )
{
	m_system_active = false;

	m_io		= new Terrain::COctreeIO( NULL, this );
	m_memory	= new Terrain::MemoryManager();
	m_sampler	= new Terrain::DataSampler(this);
	m_renderer	= new Terrain::TerrainRenderer(this);
	//m_jobs		= new Terrain::JobHandler(this);

	m_genny		= NULL;
	m_patterns	= NULL;
	m_regions	= NULL;
}
//		_Free
void CVoxelTerrain::_Free ( void )
{
	SetSystemPaused ( true );

	delete_safe( m_io );
	delete_safe( m_memory );
	delete_safe( m_sampler );
	delete_safe( m_renderer );
	//delete_safe( m_jobs );

	delete_safe( m_genny );
	delete_safe( m_patterns );
	delete_safe( m_regions );
}

//		SetSystemPaused
// Set if the terrain should chew up all the engine resources or not.
// When disabled, the processes of loading, saving, and simulation are all disabled.
void CVoxelTerrain::SetSystemPaused ( const bool n_pauseSystem )
{
	m_system_active = !n_pauseSystem;

	// TODO: need to empty out the job queue
}
// is the system paused? (todo: check for complete pause and emptied queue)
bool CVoxelTerrain::GetSystemPaused ( void ) const
{
	return m_system_active;
}
// reinitializes the terrain information			
// Used for when swapping between save files, and need to remove old savefile
void CVoxelTerrain::ResetSystem ( void )
{
	SetSystemPaused( true );
	SetGenerator( NULL );
}
// gives the terrain a new generator to work with
void CVoxelTerrain::SetGenerator ( Terrain::CWorldGenerator* n_genny )
{
	if ( m_genny != NULL )
	{
		m_genny->Cleanup();	
	}
	m_genny = n_genny;
}
// Returns the current generator being worked with
Terrain::CWorldGenerator* CVoxelTerrain::GetGenerator ( void )
{
	return m_genny;
}
// Returns the current region generator being worked with
Terrain::CRegionGenerator* CVoxelTerrain::GetRegionGenerator ( void )
{
	return m_regions;
}


//=========================================//
// System Location State

// The current WorldVector that the terrain is centered around
WorldVector CVoxelTerrain::GetCenterSector ()
{
	throw Core::NotYetImplementedException();
}
// The current Vector3d_d that the terrain is centered around, in feet (return the WorldVector*64.0F as a Vector3d_d)
Vector3d_d CVoxelTerrain::GetCenterPosition ()
{
	throw Core::NotYetImplementedException(); 
}
// Set the terrain's center position using a 3d vector in sector space
void CVoxelTerrain::SetCenterSector ( const WorldVector& vect )
{
	throw Core::NotYetImplementedException();
}
// Set the terrain's center position using a 3d vector in feet
void CVoxelTerrain::SetCenterPosition ( const Vector3d_d& vect )
{
	SetCenterSector( WorldVector(
		(rangeint)std::floor(vect.x/64.0F),
		(rangeint)std::floor(vect.y/64.0F),
		(rangeint)std::floor(vect.z/64.0F) )
		);
}


//=========================================//
// Engine Behavior

// framestep
void CVoxelTerrain::Update ( void )
{
	if ( m_system_active )
	{
/*
8:26 PM skarik said: If there's loaded needed, the a load thread is started.
8:27 PM skarik said: The load thread tries to find the data for its assigned sector. If it finds it, great, we're done.
8:27 PM skarik said: If not, then the load thread morphs into a generation thread, and the original load is pushed to the back of the load requests.
8:28 PM skarik said: Generation thread checks if there's a duplicate generation, and ends if there is. Otherwise, it generates the sector, saves it, and is done.
8:28 PM skarik said: The load request comes back to the front of the stack, and this time it's loaded.
8:29 PM skarik said: Reason for the requeue is that other sectors may already be saved, so shouldn't wait on a sector that may not be done.
8:29 PM skarik said: What do you think?
*/

		// First load happens on startup.

		// Update cannot add any load requests. That is handled by other inputs to the system.
	}
}

//=========================================//
// Game Data Query

// Grabs a game payload from the given terrain, given the input gameplay position.
// Returns the target payload (or NULL if not found) along with an indexer for the lock system.
// Remember to call ReleaseGamePayload() with the given indexer, or the system will lock up.
Terrain::GamePayload* CVoxelTerrain::AquireGamePayload ( const Vector3d& n_approximatePosition, uint32_t& o_indexer )
{
	return AquireGamePayload(
		GetCenterSector() + WorldVector(
			(rangeint)std::floor(n_approximatePosition.x/64.0F),
			(rangeint)std::floor(n_approximatePosition.y/64.0F),
			(rangeint)std::floor(n_approximatePosition.z/64.0F)
			),
		o_indexer );
}
Terrain::GamePayload* CVoxelTerrain::AquireGamePayload ( const WorldVector& n_sector_id, uint32_t& o_indexer )
{
	throw Core::NotYetImplementedException();
}
// Releases a given payload and/or payload indexer from the locked list so that the terrain system can use it.
// This absolutely must be called to accompany any AquireGamePayload() call.
void CVoxelTerrain::ReleaseGamePayload ( Terrain::GamePayload* n_gameData, const uint32_t& n_indexer )
{
	throw Core::NotYetImplementedException();
}

// Grabs an area's gamestate from the given terrain, given the input gameplay position.
// AreaGameState is specifically for data that can change often and extremely quickly.
// Returns the found area gamestate (or NULL if not found) along with an indexer for the lock system.
// Remember to call ReleaseAreaGamestate() with the given indexer, or the system will lock up during saving.
Terrain::AreaGameState* CVoxelTerrain::AquireAreaGamestate ( const Vector3d& n_approximatePosition, uint32_t& o_indexer )
{
	throw Core::NotYetImplementedException();
}
// Releases a given area gamestate and/or payload indexer from the locked list so that the terrain system can save it.
// This absolutely must be called to accompany any AquireAreaGamestate() call.
void CVoxelTerrain::ReleaseAreaGamestate ( Terrain::AreaGameState* n_gameData, const uint32_t& n_indexer )
{
	throw Core::NotYetImplementedException();
}
// Fills the given vector with a copy of the current gamestate, used for only reading.
// AreaGameState is specifically for data that can change often and extremely quickly.
// This includes items and NPCs in the area, as well as a reference to the grass list.
void CVoxelTerrain::GetAreaGamestateListCopy ( std::vector<Terrain::AreaGameState>& o_gamestatelist )
{
	throw Core::NotYetImplementedException();
}

//=========================================//
// Map State Query

//		GetMapCopy
// Locks the internal map, creates a copy, unlocks the map, and returns a copy.
// Values may no longer point to valid data after the copy is returned.
Terrain::MapStructure CVoxelTerrain::GetMapCopy ( void )
{
	{	// Wait for writers
		std::unique_lock<std::mutex> lock(m_map_write_lock);
		m_map_read_counter.add();
	}
	Terrain::MapStructure result = m_current_map;
	m_map_read_counter.sub();
	return result;
}
//		LockReadMapReference
// Locks the internal map and returns it. Assumes the map will not be modified.
// If the map cannot be locked for read, it will wait until it has been unlocked.
// Must be followed up with a UnlockReadMapReference.
const Terrain::MapStructure& CVoxelTerrain::LockReadMapReference ( void )
{
	{	// Wait for writers
		std::unique_lock<std::mutex> lock(m_map_write_lock);
		m_map_read_counter.add();
	}
	return m_current_map;
}
//		UnlockReadMapReference
// Releases a counted lock on the internal map.
void CVoxelTerrain::UnlockReadMapReference ( void )
{
	// Notify that it's able to be read again
	m_map_read_counter.sub();
}

//		LockWriteMapReference
// Locks the internal map for write and returns it. Assumes map will have a BB modification.
// If the map cannot be locked for write, it will wait until it has been unlocked.
// Must be followed up with a UnlockWriteMapReference.
Terrain::MapStructure& CVoxelTerrain::LockWriteMapReference ( void )
{
	m_map_write_lock.lock();
	m_map_read_counter.wait_for_0();
	return m_current_map;
}
//		UnlockWriteMapReference
// Releases an exclusive lock on the internal map.
void CVoxelTerrain::UnlockWriteMapReference ( void )
{
	m_map_write_lock.unlock();
}