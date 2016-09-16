
#ifndef _C_VOXEL_TERRAIN_2_H_
#define _C_VOXEL_TERRAIN_2_H_

#define TERRA_SYSTEM_VERSION 10

// Includes
#include "CGameObject.h"
#include "CRenderableObject.h"
#include "Vector3d.h"
#include "Ray.h"
#include "RaycastHit.h"

#include "TerrainSector.h"
#include "TerrainTypes.h"
#include "TerrainGameSector.h"

// compatibility
#include "CBoob.h"

//#include "Perlin.h"
//#include "CCamera.h"
//#include "CInput.h"
//#include "CRenderState.h"
#include "Cubic.h"
#include "Frustum.h"

#include "CDebugConsole.h"
#include "CBinaryFile.h"
class CWeaponItem;

#include "RangeVector.h"
#include "BlockInfo.h"

#include <bitset>
#include <math.h>
#include <algorithm>
#include <queue>
#include <atomic>

#include "boost/thread.hpp"

#include "Events.h"

#include "vect3d_template.h"


// Usings
using std::bitset;
//using boost::thread;
//using boost::mutex;

// IO prototype
namespace Terrain {
	class COctreeIO;
	class CWorldGenerator;
	class SidebufferAccessor;
	class CQueuedObjectLoader;

	class CPatternController;
	class CRegionController;
}

// Class Definition prototype
class COctreeRenderer;
class CTerrainAccessor;

// Class Definition
class COctreeTerrain : public CGameBehavior
{
	ClassName( "COctreeTerrain" );
	BaseClass( "COctreeTerrain" );

	friend COctreeRenderer;
	friend Terrain::COctreeIO;
	friend Terrain::SidebufferAccessor;
	friend CTerrainAccessor;

			typedef boost::thread	thread;
			typedef boost::mutex	mutex;
public:
			typedef Terrain::terra_t terra_t;
			//typedef Terrain::terra_t_lod terra_t_lod;

						COctreeTerrain ( void );
						~COctreeTerrain ( void );

	// Reinitialize ( ) : reinitializes the terrain information			
	// Used for when swapping between save files, and need to remove old savefile
	void				Reinitialize ( void );
	// SetGenerator ( ) : gives the terrain a new generator to work with
	// Usually used before reinitialize or between planets
	void				SetGenerator ( Terrain::CWorldGenerator* );

	void				Update ( void );
	void				PostUpdate ( void );

	// GetActive ( ) : returns the current active terrain. Generally, only want to use a single terrain and swap the
	// data set being used.
	static inline		COctreeTerrain* GetActive ( void );
	// GetGenerator ( ) : returns the current generator being worked with
	Terrain::CWorldGenerator*	GetGenerator ( void );
	// GetRenderer ( ) : return this terrain's renderer. This renderer can be edited.
	// Typically used for disabling the renderer or editing it for special effects.
	COctreeRenderer*	GetRenderer ( void );

	// GetPatternManager ( ) : return this terrain's pattern manager.
	Terrain::CPatternController*	GetPatternManager ( void );
	// GetRegionManager ( ) : return this terrain's region manager.
	Terrain::CRegionController*		GetRegionManager ( void );

	// GetIO ( ) : return this terrain's IO handler.
	const Terrain::COctreeIO*		GetIO ( void ) const;

private:
		static vector<COctreeTerrain*>	terrainList;	// Active terrain list

public:
		static const Real blockSize;

private:
	// Rendering
	COctreeRenderer*	m_renderer;
	// Pattern (town and castle) manager
	Terrain::CPatternController*m_patterns;
	// Region (provinces and area ownership) manager
	Terrain::CRegionController*	m_regions;		

private:
	// Memory manager
	bool				InitTerraMemory ( void );
	void				FreeTerraMemory ( void );

	void				NewDataBlock  ( Terrain::Sector** block );
	void				FreeDataBlock ( Terrain::Sector* block );

public:
	Real				GetMemoryUsage ( void );
private:
		static uint32_t	sectorMemorySize;

		uint32_t	m_memoryMaxBlocks;
		char*		m_memoryData;
		char*		m_memoryUsage;

		mutex		m_memoryLock;

private:
	// Terrain tools
		Terrain::COctreeIO*	mIO;

		Terrain::CWorldGenerator*	mGenerator;
private:
	// Terrain management
	// Constants
		bool		m_generateTerrain;
		int64_t		m_terrainSeed;

		uint32_t	m_givenRange;
		uint32_t	m_treeStartResolution; // Muliply by "blockSize" to get the real world coordinates.
		uint32_t	m_subdivisionTarget;
		uint32_t	m_currentSubdivisionTarget;

		Terrain::Sector*	m_root;
		Terrain::Sector*	m_sidebuffer0;
		Terrain::Sector*	m_sidebuffer1;
		std::atomic_flag	m_sidebuffer0_open;
		std::atomic_flag	m_sidebuffer1_open;
		Terrain::SectorGameData	m_gamedata_sidebuffer0;
		Terrain::SectorGameData	m_gamedata_sidebuffer1;

	// Types
		struct SectorState {
			std::atomic_flag	m_area_busy;
			std::atomic_flag	m_area_gamedata_busy;
			std::atomic_flag	m_area_gamedata_used;
			bool	m_area_serviced;
			bool	m_has_data;
			bool	m_has_mesh;
			bool	m_has_collision;
			ushort	m_lod_level;
			Terrain::SectorGameData			m_game_data;
			Terrain::SectorGameDataState	m_game_data_state;

			SectorState ( void );
			SectorState& operator= ( SectorState& right );
		};

	// Moving system variables (Terrain state)

		// System pause states
		bool		m_systemReady;
		bool		m_systemPaused;
		bool		m_systemRequestingShift;

		// Lock used to edit state information.
		boost::shared_mutex	m_state_readlock;

		Vector3d	m_state_followTarget;
		//Vector3d	m_state_followOffset;

		Vector3d_d	m_state_centerPosition;
		RangeVector	m_state_centerIndex;
		SectorState*m_state_sectors;

		// Lock used to synchronize threads to the main thread.
		boost::shared_mutex	m_state_synchronizeLock;

		Terrain::CQueuedObjectLoader*	m_queueloader;

public:
	// State accessors
	void	SetSystemPaused ( const bool n_pauseSystem );
	bool	GetSystemPaused ( void ) const;
	void	SetStateFollowTarget ( const Vector3d& n_followTarget );
	void	SetStateWorldOffset ( const Vector3d_d& n_followTarget );
	Vector3d_d	GetStateWorldCenterPosition ( void ) const;
	//Vector3d	GetStateFollowOffset ( void ) const;
	RangeVector	GetStateWorldCenterIndex ( void ) const;

	void	State_ClearInformation ( void );

	bool	State_SectorHasMesh ( const Vector3d& n_worldPosition );

private:
	// System management
	uint32_t			GetResolutionAtPosition ( const Vector3d& approximatePosition ) const;
	bool				PositionInOctree ( const Vector3d& approximatePosition ) const;

	// Sector management

	// Have the root follow the camera
	void				Sectors_FollowTarget ( void );
	// Request jobs for sectors
	void				Sectors_GenerateJobs ( void );
	// Request regeneration on a position
	void				Sectors_Request_NewMesh ( const Vector3d& n_position );

	// Shift all sector data at the given subdivision to a direction
	void				ShiftSectors ( const ushort n_shiftDirection );
	// Shift all data at the given subdivision to a direction
	void				ShiftData ( const uint32_t n_subdivisionTarget, const ushort n_shiftDirection );

	// Sets data at position. Will try to minimize the octree after setting the data.
	void				SetDataAtPositionMinimal ( const uint64_t newData, const Vector3d& approximatePosition );
	// Returns true when it gets to the target position
	// Does not perform any locks. Locking is the responsibility of the caller.
	bool				SeekToPosition ( TrackInfo& n_currentArea, const Vector3d& approximateTarget, const uint32_t n_subdivisionTarget=2 );
	// Subdivides the area at the given data into copies.
	// Does not perform any locks. Locking is the responsibility of the caller.
	void				SectorSubdivide ( const TrackInfo& n_targetArea );
	// Returns true when it gets to the target position.
	// Differs from SeekTo in that it will traverse the heirarchy upwards.
	// Does not perform any locks. Locking is the responsibility of the caller.
	bool				TraverseToPosition ( TrackInfo& n_currentArea, const Vector3d& approximateTarget );
	// Recursively deletes the sector and its children
	// OLD: Will first attempt to the lock the target area. Children will be checked for a lock on a case-by-case basis.
	// CURRENT: Will not lock the target area, but children will be checked for a lock on a case-by-case basis.
	void				SectorDelete ( TrackInfo& n_targetArea );

	// Sets up octree to be moved around
	void				SubdivideStart ( Terrain::Sector* n_currentSector, const uint32_t n_currentSubdiv );

	// Sets data at side-buffer position
	void				SetDataAtSidebufferPosition ( Terrain::Sector* n_sideBuffer, const uint64_t newData, const RangeVector& absoluteIndex );
	// Gets data at side-buffer position
	void				GetDataAtSidebufferPosition ( Terrain::Sector* n_sideBuffer, uint64_t& o_data, const RangeVector& absoluteIndex );
	// Sets up a generation side-buffer
	void				SubdivideSideBuffer ( Terrain::Sector* n_currentSector, const uint32_t n_currentSubdiv );

public:
	// Terrain outside management
	Terrain::SectorGameData*	AquireGameDataForEdit ( const Vector3d& n_approximatePosition, uint32_t& o_indexer );
	void				ReleaseGameDataForEdit ( Terrain::SectorGameData* n_gameData, const uint32_t& n_indexer );

	Terrain::SectorGameDataState*	AquireGameDataStateForEdit ( const Vector3d& n_approximatePosition, uint32_t& o_indexer );
	void	ReleaseGameDataStateForEdit ( Terrain::SectorGameDataState* n_gameState, const uint32_t& n_indexer );

private:
	void				SectorGetAtWorldPosition ( const Vector3d& n_approximatePosition, int32_t& o_subx, int32_t& o_suby, int32_t& o_subz, uint32_t& o_indexer );

public:
	// Terrain samplers
	void				SampleBlock ( const Vector3d& n_approximatePosition, uint64_t& o_data );
	void				SampleNormal ( const Vector3d& n_approximatePosition, Vector3d& o_normal );

	void				SampleBlockExpensive ( const Vector3d& n_approximatePosition, BlockTrackInfo* o_tracker );

private:
	void				SampleSector ( const Vector3d& n_approximatePosition, Terrain::Sector*& o_sector, const uint32_t n_subdivisionTarget=32 );

public:
	// Terrain editors
	void				Generation_GenerateSubarea ( Terrain::Sector* n_sideBuffer, Terrain::SectorGameData* n_sideBufferGameData, const RangeVector& n_rangeIndex );

	bool				CSG_SetBlock ( const Vector3d& nCenter, const ushort blockType );

	void				CSG_AddBlock ( const Vector3d& nCenter, const Vector3d& nHalfExtents );
	void				CSG_RemoveBlock ( const Vector3d& nCenter, const Vector3d& nHalfExtents );

	void				CSG_RemoveSphere ( const Vector3d& nCenter, const Real& nRadius );

	void				TerrainUpdate ( void );


	void				Edit_EstimateNormals ( const Vector3d& nMin, const Vector3d& nMax, const Real& nStep );

private:
	// Job handling
		enum eTerrainJobType {
			TERRAJOB_INVALID = 255,
			TERRAJOB_GENERATE = 0,
			TERRAJOB_REMESH_AREA,
			TERRAJOB_RECALCULATE_NORMALS,
			
			TERRAJOB_LOAD_SECTOR,
			TERRAJOB_SAVE_SECTOR,

			TERRAJOB_CONDENSE_AREA
		};
		struct TerrainJob {
			eTerrainJobType	type;
			RangeVector	index;
			Vector3d	area_min;
			Vector3d	area_max;

			int priority;
			int delay;
			
			bool operator< ( const TerrainJob& right ) const {
				return priority > right.priority;
			}
			bool operator==( const TerrainJob& right ) const {
				return (type == right.type) && (index == right.index);
			}
			explicit TerrainJob ( const RangeVector& n_index )
				: type(TERRAJOB_INVALID), index(n_index), delay(0)
			{
				;
			}
			TerrainJob ( const TerrainJob& n_job )
				: type(n_job.type), index(n_job.index), area_min(n_job.area_min), area_max(n_job.area_max), priority(n_job.priority), delay(n_job.delay)
			{
				;
			}
		};

		mutex				m_joblist_lock;
		boost::shared_mutex	m_joblist_major_operation_lock;
		//priority_queue<TerrainJob>	m_joblist;
		std::deque<TerrainJob>	m_joblist;

	void				Job_CalculatePriority ( TerrainJob& n_jobToEdit );
	void				Job_Enqueue ( const TerrainJob& n_jobToAdd );
	void				Job_Enqueue ( vector<TerrainJob>& n_jobsToAdd );
	bool				Job_Dequeue ( TerrainJob& o_jobToUse );

	void				Job_Clear ( void );

	void				Job_Work_EnqueueToList( const TerrainJob& n_jobToAdd );

		struct TerrainThread {
			COctreeTerrain* m_terrain;
			int m_id;
			void operator() ( void ) {
				m_terrain->Job_Step(m_id);
			}
		};
		friend TerrainThread;
	void				Job_StartThreads ( void );
	void				Job_Step ( const int n_threadid );
	void				Job_StopThreads ( void );

		bool				m_threads_run;
		vector<thread*>		m_threads;

		// Thread properties
		vector<Real_d>		m_threadtimes;
public:
	const vector<Real_d>& GetThreadTimes ( void ) const {
		return m_threadtimes;
	}
private:
	// Debug help
	void				DebugDrawOctree ( void );
	void				DebugDrawData ( const TrackInfo& n_currentArea );

	void				DebugCreateTestArea ( void );

	void				DebugGenerateNormals ( void );

	void				DebugGenerateTestArea ( const Vector3d& nMin, const Vector3d& nMax, const Real& nStep );

	void				DebugGenerateTestSubarea ( Terrain::Sector* n_sideBuffer, const Vector3d& nMin, const Vector3d& nMax );
	void				DebugGenerateNormalsSubarea ( Terrain::Sector* n_sideBuffer );
public:

	// COmponents
		struct TerraFoiliage;
		struct TerraProp;
		struct sVTQueuedLoad;
		struct sVTQueuedSave;

		//struct sCallableThread;
		//struct sGenerationThread;
		//struct sGenerationRequest;
};

//#include "COctreeTerrain.hpp"
COctreeTerrain* COctreeTerrain::GetActive ( void )
{
	if ( terrainList.empty() ) {
		return NULL;
	}
	else {
		return terrainList[0];
	}
}

// Sidebuffer accessor
namespace Terrain
{
	class SidebufferAccessor
	{
	public:
		explicit	SidebufferAccessor ( COctreeTerrain* n_terrain, Terrain::Sector* n_sideBuffer )
			: m_terrain( n_terrain ), m_sideBuffer( n_sideBuffer )
		{
			;
		}

		// Sets data at side-buffer position
		void		SetDataAtSidebufferPosition ( const uint64_t newData, const RangeVector& absoluteIndex ) {
			m_terrain->SetDataAtSidebufferPosition( m_sideBuffer, newData, absoluteIndex );
		}
		// Gets data at side-buffer position
		void		GetDataAtSidebufferPosition ( uint64_t& o_data, const RangeVector& absoluteIndex ) {
			m_terrain->GetDataAtSidebufferPosition( m_sideBuffer, o_data, absoluteIndex );
		}

	private:
		COctreeTerrain*		m_terrain;
		Terrain::Sector*	m_sideBuffer;
	};
};

#endif//_C_VOXEL_TERRAIN_2_H_