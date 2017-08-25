
#ifndef _AFTER_VOXEL_TERRAIN_H_
#define _AFTER_VOXEL_TERRAIN_H_

#include <vector>

#include "core/math/vect3d_template.h"
#include "core-ext/containers/arproperty.h"

#include "engine/behavior/CGameBehavior.h"

#include "after/terrain/data/GamePayload.h"
#include "after/terrain/data/GameState.h"

namespace Terrain
{
	class COctreeIO;
	class MemoryManager;
	class DataSampler;
	class TerrainRenderer;
	class CWorldGenerator;
	class CRegionGenerator;
	class CPatternController;
	class JobHandler;

	// Size of a voxel in world units (which is feet).
	const int BlockSize = 2;

	// Number of voxels in a typical sector
	const int SectorSize = (32*32*32);

	// Typical dimensions of a sector
	const WorldVector SectorDim = WorldVector(32,32,32);
}

class CVoxelTerrain : public CGameBehavior
{

public:
	// On construction, the voxel terrain is added to the list.
	// To ensure it is made active, call MakeActive() on the newly created terrain.
	explicit				CVoxelTerrain ( void );
	// On destruction, the voxel terrain is removed from the list.
	// If this is the 0th terrain, MakeActive() is automatically called on the next terrain.
							~CVoxelTerrain ( void );


	//=========================================//

	//		MakeActive
	// Makes the current terrain the 0th one in the list
	void					MakeActive ( void );
	//		GetActive
	// Returns the current 0th terrain, or NULL if there is no active terrain
	static CVoxelTerrain*	GetActive ( void );

	//=========================================//
	
	//		static TerrainList
	// Provides a list of all the active terrains. The 0th one in the list is the active one.
	// All terrains pull from the same pool of data, so maximum of active terrains should be 2.
	static std::vector<CVoxelTerrain*>	TerrainList;

public:
	//=========================================//
	// Engine Behavior

	//		Update()
	// Default framestep for the terrain
	void					Update ( void ) override;

public:
	//=========================================//
	// Game Data Query

	//		AquireGamePayload
	// Grabs a game payload from the given terrain, given the input gameplay position.
	// Returns the target payload (or NULL if not found) along with an indexer for the lock system.
	// Remember to call ReleaseGamePayload() with the given indexer, or the system will lock up.
	Terrain::GamePayload*	AquireGamePayload ( const Vector3d& n_approximatePosition, uint32_t& o_indexer );
	Terrain::GamePayload*	AquireGamePayload ( const WorldVector& n_sector_id, uint32_t& o_indexer );

	//		ReleaseGamePayload
	// Releases a given payload and/or payload indexer from the locked list so that the terrain system can use it.
	// This absolutely must be called to accompany any AquireGamePayload() call.
	void					ReleaseGamePayload ( Terrain::GamePayload* n_gameData, const uint32_t& n_indexer );


	//		AquireAreaGamestate
	// Grabs an area's gamestate from the given terrain, given the input gameplay position.
	// AreaGameState is specifically for data that can change often and extremely quickly.
	// Returns the found area gamestate (or NULL if not found) along with an indexer for the lock system.
	// Remember to call ReleaseAreaGamestate() with the given indexer, or the system will lock up during saving.
	Terrain::AreaGameState*	AquireAreaGamestate ( const Vector3d& n_approximatePosition, uint32_t& o_indexer );

	//		ReleaseAreaGamestate
	// Releases a given area gamestate and/or payload indexer from the locked list so that the terrain system can save it.
	// This absolutely must be called to accompany any AquireAreaGamestate() call.
	void					ReleaseAreaGamestate ( Terrain::AreaGameState* n_gameData, const uint32_t& n_indexer );

	//		GetAreaGamestateListCopy
	// Fills the given vector with a copy of the current gamestate, used for only reading.
	// AreaGameState is specifically for data that can change often and extremely quickly.
	// This includes items and NPCs in the area, as well as a reference to the grass list.
	void					GetAreaGamestateListCopy ( std::vector<Terrain::AreaGameState>& o_gamestatelist );

public:
	//=========================================//
	// System State

	//		SetSystemPaused
	// Set if the terrain should chew up all the engine resources or not.
	// When disabled, the processes of loading, saving, and simulation are all disabled.
	void					SetSystemPaused ( const bool n_pauseSystem );

	//		GetSystemPaused
	// Return if terrain system is paused or not.
	// This doesn't actually mean that all the worker threads are paused, only that current idle threads are not starting new jobs.
	bool					GetSystemPaused ( void ) const;

	//		ResetSystem
	// reinitializes the terrain information			
	// Used for when swapping between save files, and need to remove old savefile
	void					ResetSystem ( void );

	//		SetGenerator
	// gives the terrain a new generator to work with
	// Usually used before reinitialize/reset or between planets
	void					SetGenerator ( Terrain::CWorldGenerator* );

	//		GetGenerator
	// Returns the current generator being worked with
	Terrain::CWorldGenerator*	GetGenerator ( void );

	//		GetGenerator
	// Returns the current region generator being worked with
	Terrain::CRegionGenerator*	GetRegionGenerator ( void );

	//=========================================//
	// System Location State

	//		GetCenterSector
	// The current WorldVector that the terrain is centered around
	WorldVector				GetCenterSector ( void );
	//		GetCenterPosition
	// The current Vector3d_d that the terrain is centered around, in feet
	Vector3d_d				GetCenterPosition ( void );

	//		SetCenterSector
	// Set the terrain's center position using a 3d vector in sector space
	void					SetCenterSector ( const WorldVector& );
	//		SetCenterPosition
	// Set the terrain's center position using a 3d vector in feet
	void					SetCenterPosition ( const Vector3d_d& );

private:
	// Main systems

	Terrain::COctreeIO*		m_io;
	Terrain::MemoryManager*	m_memory;
	Terrain::DataSampler*	m_sampler;
	Terrain::TerrainRenderer* m_renderer;
	Terrain::JobHandler*	m_jobs;

	// Generation Triad

	Terrain::CWorldGenerator*	m_genny;
	Terrain::CPatternController*m_patterns;
	Terrain::CRegionGenerator*	m_regions;

	// Current state

	bool		m_system_active;

public:
	// Properties

	arproperty<Terrain::COctreeIO*>		IO;
	arproperty<Terrain::MemoryManager*>	Memory;
	arproperty<Terrain::DataSampler*>	Sampler;
	arproperty<Terrain::TerrainRenderer*>	Renderer;

private:

	//		_Init
	// Set up initial terrain state
	void					_Init ( void );
	//		_Free
	// Free up terrain state
	void					_Free ( void );

};
typedef CVoxelTerrain VoxelTerrain;

#endif//_AFTER_VOXEL_TERRAIN_H_