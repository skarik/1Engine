// OctreeTerrain Sectors
// Sector management code

#include "COctreeTerrain.h"
#include "Zones.h"

#include "COctreeRenderer.h"

using std::cout;
using std::endl;

// ================================================
// Sector State

// State initializer
COctreeTerrain::SectorState::SectorState ( void )
	: m_area_busy(), m_area_gamedata_busy(), m_area_gamedata_used(),
	m_area_serviced(false), m_has_data(false), m_has_mesh(false), m_has_collision(false), m_lod_level(ushort(-1))
{
	m_area_busy.clear();
	m_area_gamedata_busy.clear();
	m_area_gamedata_used.clear();

	// Set initial load flags
	m_game_data.m_loaded_big = false;
	m_game_data.m_loaded_small = false;

	// Clear out the game data
	m_game_data.m_grass.clear();
	//m_game_data.m_foliage.clear(); // TODO: Better unload here
	for ( uint i = 0; i < m_game_data.m_foliage.size(); ++i ) {
		CGameObject::DeleteObject( m_game_data.m_foliage[i] );
	}
	m_game_data.m_foliage_queue.clear();
	//m_game_data.m_components.clear(); // TODO: Better unload here
	for ( uint i = 0; i < m_game_data.m_components.size(); ++i ) {
		CGameObject::DeleteObject( m_game_data.m_components[i] );
	}
	m_game_data.m_component_queue.clear();
}
// Information movement
COctreeTerrain::SectorState& COctreeTerrain::SectorState::operator= ( SectorState& right )
{
	m_has_data = right.m_has_data;
	m_lod_level = right.m_lod_level;
	m_has_mesh = right.m_has_mesh;
	m_has_collision = right.m_has_collision;

	// Swap around game data as things move
	std::swap( m_game_data, right.m_game_data );
	std::swap( m_game_data_state, right.m_game_data_state );
	return *this;
}
// ================================================

void COctreeTerrain::State_ClearInformation ( void )
{
	// Lock the state for write
	Terrain::write_lock stateLock(m_state_readlock);

	int32_t subX, subY, subZ;
	int32_t tMaxX, tMaxY, tMaxZ;

	tMaxX = (int32_t)m_treeStartResolution/32;
	tMaxY = (int32_t)m_treeStartResolution/32;
	tMaxZ = (int32_t)m_treeStartResolution/64;

	// Clear out all information out of the states
	for ( subX = 0; subX < tMaxX; ++subX ) {
		for ( subY = 0; subY < tMaxY; ++subY ) {
			for ( subZ = 0; subZ < tMaxZ; ++subZ ) {
				SectorState& currentSector = m_state_sectors[ subX + subY*tMaxX + subZ*tMaxX*tMaxY ];
				currentSector.m_area_serviced = false;
				currentSector.m_has_data = 0;
				currentSector.m_lod_level = ushort(-1);
				currentSector.m_has_mesh = false;
				currentSector.m_has_collision = false;
			}
		}
	}

	// Clear the job list
	Job_Clear();
}

bool COctreeTerrain::State_SectorHasMesh ( const Vector3d& n_worldPosition )
{
	if ( !PositionInOctree( n_worldPosition ) ) {
		return false;
	}

	// Lock the state for read
	Terrain::read_lock stateEasyLock(m_state_readlock);

	int32_t tMaxX, tMaxY, tMaxZ;
	tMaxX = (int32_t)m_treeStartResolution/32;
	tMaxY = (int32_t)m_treeStartResolution/32;
	tMaxZ = (int32_t)m_treeStartResolution/64;
	RangeVector indexingPosition = Zones.PositionToRV( n_worldPosition );
	/*indexingPosition.x -= m_state_centerIndex.x-tMaxX/2;
	indexingPosition.y -= m_state_centerIndex.y-tMaxY/2;
	indexingPosition.z -= m_state_centerIndex.z-tMaxZ/2;*/
	indexingPosition.x += tMaxX/2;
	indexingPosition.y += tMaxY/2;
	indexingPosition.z += tMaxZ/2;
	SectorState& currentSector = m_state_sectors[ indexingPosition.x + indexingPosition.y*tMaxX + indexingPosition.z*tMaxX*tMaxY ];

	return currentSector.m_has_mesh;
}

void COctreeTerrain::Sectors_FollowTarget ( void )
{
	// The following performs the world shifting code.
	Vector3d tFollowPosition = m_state_followTarget / 64.0f;
	Vector3d tDeltaMovement ( 0,0,0 );
	RangeVector tRangeVectorMotion ( 0,0,0 );
	// Reset movement
	//m_state_followOffset = Vector3d(0,0,0);

	if ( m_systemRequestingShift == false )
	{
		// If the follow position is out of range on any axis, immediately set it, and send a message to clear the world.
		if (( tFollowPosition.x > (m_treeStartResolution*blockSize)/64.0f )
			||( tFollowPosition.y > (m_treeStartResolution*blockSize)/64.0f )
			||( tFollowPosition.z > (m_treeStartResolution*blockSize)/64.0f ))
		{
			//m_state_centerIndex = RangeVector( (rangeint)tFollowPosition.x,
			m_state_centerIndex = RangeVector( (int32_t)floorf(m_state_followTarget.x/64 + 0.5),(int32_t)floorf(m_state_followTarget.y/64 + 0.5),(int32_t)floorf(m_state_followTarget.z/64 + 0.5) );
			//Zones.PositionToRV( m_state_followTarget );
			//m_state_centerIndex = RangeVector( tFollowPosition.x-0.5f,tFollowPosition.y-0.5f,tFollowPosition.z-0.5f );
			m_state_centerPosition.x = m_state_centerIndex.x * 64;
			m_state_centerPosition.y = m_state_centerIndex.y * 64;
			m_state_centerPosition.z = m_state_centerIndex.z * 64;
			// Reset the follow target
			m_state_followTarget -= Vector3d(m_state_centerPosition.x,m_state_centerPosition.y,m_state_centerPosition.z);
			// Clear all data.
			State_ClearInformation();
		}
		else if (( tFollowPosition.x > (tDeltaMovement.x+0.5)+0.1 ) || ( tFollowPosition.y > (tDeltaMovement.y+0.5)+0.1 ) || ( tFollowPosition.z > (tDeltaMovement.z+0.5)+0.1 )
			|| ( tFollowPosition.x < (tDeltaMovement.x-0.5)-0.1 ) || ( tFollowPosition.y < (tDeltaMovement.y-0.5)-0.1 ) || ( tFollowPosition.z < (tDeltaMovement.z-0.5)-0.1 ) )
		{
			//bool bRootUpdated = false;
			Job_Clear(); // System will be updated, so stop jobs
			// Call empty offset to empty the job table
			m_renderer->OffsetSystem( Vector3d::zero, RangeVector(0,0,0) );

			m_systemRequestingShift = true;

			cout << "Waiting for full terrain lock..." << endl;
			cout << "Follow target: " << m_state_followTarget.x << " " << m_state_followTarget.y << " " << m_state_followTarget.z << endl;
		}
	}

	if ( m_systemRequestingShift )
	{
		//Terrain::write_lock intensiveOperation( m_state_synchronizeLock );
		if ( m_state_synchronizeLock.try_lock() )
		{
			cout << "...got lock." << endl;

			// Clear the job list as everything will be regenerated.
			Job_Clear();

			// Debug output
			cout << "CENTER 0: " << m_state_centerIndex.x << " " << m_state_centerIndex.y << " " << m_state_centerIndex.z << endl;

			// Move the data
			while ( tFollowPosition.x > (tDeltaMovement.x+0.5)+0.1 ) {
				ShiftData( 32, X_POS );
				ShiftSectors( X_POS );
				m_state_centerPosition.x += 64;
				m_state_centerIndex.x += 1;
				tRangeVectorMotion.x += 1;
				tDeltaMovement.x += 1;
			}
			while ( tFollowPosition.y > (tDeltaMovement.y+0.5)+0.1 ) {
				ShiftData( 32, Y_POS );
				ShiftSectors( Y_POS );
				m_state_centerPosition.y += 64;
				m_state_centerIndex.y += 1;
				tRangeVectorMotion.y += 1;
				tDeltaMovement.y += 1;
			}
			while ( tFollowPosition.z > (tDeltaMovement.z+0.5)+0.1 ) {
				ShiftData( 32, Z_POS );
				ShiftSectors( Z_POS );
				m_state_centerPosition.z += 64;
				m_state_centerIndex.z += 1;
				tRangeVectorMotion.z += 1;
				tDeltaMovement.z += 1;
			}
			while ( tFollowPosition.x < (tDeltaMovement.x-0.5)-0.1 ) {
				ShiftData( 32, X_NEG );
				ShiftSectors( X_NEG );
				m_state_centerPosition.x -= 64;
				m_state_centerIndex.x -= 1;
				tRangeVectorMotion.x -= 1;
				tDeltaMovement.x -= 1;
			}
			while ( tFollowPosition.y < (tDeltaMovement.y-0.5)-0.1 ) {
				ShiftData( 32, Y_NEG );
				ShiftSectors( Y_NEG );
				m_state_centerPosition.y -= 64;
				m_state_centerIndex.y -= 1;
				tRangeVectorMotion.y -= 1;
				tDeltaMovement.y -= 1;
			}
			while ( tFollowPosition.z < (tDeltaMovement.z-0.5)-0.1 ) {
				ShiftData( 32, Z_NEG );
				ShiftSectors( Z_NEG );
				m_state_centerPosition.z -= 64;
				m_state_centerIndex.z -= 1;
				tRangeVectorMotion.z -= 1;
				tDeltaMovement.z -= 1;
			}

			bool resetRoot = true;
			if ( resetRoot )
			{
				// Also, the entire world must be shifted at this point. (with -tDeltaMovement)
				cout << "World(A) shifting by " << -tDeltaMovement*64 << endl;
				m_state_followTarget -= tDeltaMovement * 64;
				CTransform::root.position -= tDeltaMovement * 64;
				// Meshes in the renderer must also be shifted. (with -tDeltaMovement)
				m_renderer->OffsetSystem( -tDeltaMovement * 64, tRangeVectorMotion );
			}
			else
			{	// NO IDEA IF THIS WORKS. SHOULD CHECK LATER
				// Only the terrain is shifting here. (with -tDeltaMovement)
				cout << "Terrain(B) shifting by " << -tDeltaMovement*64 << endl;

				m_renderer->transform.position += tDeltaMovement;
				//m_state_followTarget -= tDeltaMovement * 64;
				//CTransform::root.position -= tDeltaMovement * 64;
				// Meshes in the renderer must also be shifted. (with -tDeltaMovement)
				//m_renderer->OffsetSystem( -tDeltaMovement * 64, tRangeVectorMotion );
			}

			// Debug output
			cout << "CENTER 1: " << m_state_centerIndex.x << " " << m_state_centerIndex.y << " " << m_state_centerIndex.z << endl;

			m_state_synchronizeLock.unlock();
			m_systemRequestingShift = false;
		}
	}
}

void COctreeTerrain::Sectors_GenerateJobs ( void )
{
	int32_t subX, subY, subZ;
	int32_t tMaxX, tMaxY, tMaxZ;

	tMaxX = (int32_t)m_treeStartResolution/32;
	tMaxY = (int32_t)m_treeStartResolution/32;
	tMaxZ = (int32_t)m_treeStartResolution/64;

	SectorState* tSector;
	Vector3d area_min;
	Vector3d area_max;
	RangeVector area_index;
	vector<TerrainJob> jobsToAdd;
	for ( subX = 0; subX < tMaxX; ++subX )
	{
		for ( subY = 0; subY < tMaxY; ++subY )
		{
			for ( subZ = 0; subZ < tMaxZ; ++subZ )
			{
				// Grab sector
				tSector = &(m_state_sectors[ subX + subY*tMaxX + subZ*tMaxX*tMaxY ]);
				if ( tSector->m_area_serviced ) {
					continue;
				}
				tSector->m_area_serviced = true; // Mark area as serviced (Job system will unmark when done)

				// Generate generation area
				area_index = RangeVector(
					subX - tMaxX/2 + m_state_centerIndex.x,
					subY - tMaxY/2 + m_state_centerIndex.y,
					subZ - tMaxZ/2 + m_state_centerIndex.z );
				area_min = Vector3d( area_index.x-m_state_centerIndex.x, area_index.y-m_state_centerIndex.y, area_index.z-m_state_centerIndex.z ) * 32 * blockSize;
				area_max = area_min + (Vector3d( 32,32,32 ) * blockSize);

				// If sector has no data, then load data.
				if ( !tSector->m_has_data ) {
					TerrainJob newJob ( area_index );
					newJob.area_min = area_min;
					newJob.area_max = area_max;
					// TERRAJOB_LOAD_SECTOR automatically generates a TERRAJOB_GENERATE on failure.
					newJob.type = TERRAJOB_LOAD_SECTOR;
					jobsToAdd.push_back( newJob );
					/*newJob.type = TERRAJOB_RECALCULATE_NORMALS;
					jobsToAdd.push_back( newJob );*/
				}
				// If sector has an invalid LOD level, then regenerate mesh
				uint32_t targetLOD = GetResolutionAtPosition( (area_min+area_max)/2 );
				if ( tSector->m_lod_level > m_treeStartResolution ) {
					tSector->m_lod_level = targetLOD;

					TerrainJob newJob( area_index );
					newJob.area_min = area_min;
					newJob.area_max = area_max;
					/*newJob.type = TERRAJOB_RECALCULATE_NORMALS;
					jobsToAdd.push_back( newJob );
					newJob.type = TERRAJOB_REMESH_AREA;
					jobsToAdd.push_back( newJob );*/
				}
				else if ( tSector->m_lod_level < targetLOD ) {
					// If sector's LOD level is smaller than target LoD, need to load higher resolution mesh
					tSector->m_lod_level = targetLOD;

					TerrainJob newJob( area_index );
					newJob.area_min = area_min;
					newJob.area_max = area_max;
					newJob.type = TERRAJOB_LOAD_SECTOR;
					jobsToAdd.push_back( newJob );
					/*newJob.type = TERRAJOB_RECALCULATE_NORMALS;
					jobsToAdd.push_back( newJob );
					newJob.type = TERRAJOB_REMESH_AREA;
					jobsToAdd.push_back( newJob );*/
				}
				else if ( tSector->m_lod_level > targetLOD ) {
					// If sector's LOD level is larger than target LoD, need to save and create lower resolution mesh
					TerrainJob newJob( area_index );
					newJob.area_min = area_min;
					newJob.area_max = area_max;
					if ( tSector->m_lod_level == 2 ) {
						newJob.type = TERRAJOB_SAVE_SECTOR;
						jobsToAdd.push_back( newJob );
					}
					newJob.type = TERRAJOB_CONDENSE_AREA;
					jobsToAdd.push_back( newJob );
					/*newJob.type = TERRAJOB_REMESH_AREA;
					jobsToAdd.push_back( newJob );
					newJob.type = TERRAJOB_RECALCULATE_NORMALS;
					jobsToAdd.push_back( newJob );*/

					tSector->m_lod_level = targetLOD;
				}
				{
					TerrainJob newJob( area_index );
					newJob.area_min = area_min;
					newJob.area_max = area_max;
					newJob.type = TERRAJOB_RECALCULATE_NORMALS;
					jobsToAdd.push_back( newJob );
					newJob.type = TERRAJOB_REMESH_AREA;
					jobsToAdd.push_back( newJob );
				}
				// End sector service
			}
		}
	}
	// Batch add jobs
	if ( !jobsToAdd.empty() ) {
		Job_Enqueue( jobsToAdd );
	}
}

void COctreeTerrain::ShiftSectors ( const ushort n_shiftDirection )
{
	// Lock the state for write
	Terrain::write_lock stateLock(m_state_readlock);

	// This routine is extremely similar to the octree shift routine, but it is vastly more simple in its lookups.

	int32_t *trSubmajor0, *trSubmajor1;
	int32_t *tSubmajorMax0, *tSubmajorMax1;
	int32_t *trMajor0;
	int32_t *tMajorMax0;
	int32_t subX, subY, subZ;
	int32_t tMaxX, tMaxY, tMaxZ;

	tMaxX = (int32_t)m_treeStartResolution/32;
	tMaxY = (int32_t)m_treeStartResolution/32;
	tMaxZ = (int32_t)m_treeStartResolution/64;

	// Set the direction based on the axes
#define SET_AXES(MINOR0,MINORmax0,MINOR1,MINORmax1,MAJOR,MAJORmax)\
	trSubmajor0		= &MINOR0;		\
	tSubmajorMax0	= &MINORmax0;	\
	trSubmajor1		= &MINOR1;		\
	tSubmajorMax1	= &MINORmax1;	\
	trMajor0		= &MAJOR;		\
	tMajorMax0		= &MAJORmax;

	if ( n_shiftDirection == X_POS || n_shiftDirection == X_NEG ) {
		SET_AXES(subZ,tMaxZ,subY,tMaxY,subX,tMaxX);
	}
	else if ( n_shiftDirection == Y_POS || n_shiftDirection == Y_NEG ) {
		SET_AXES(subZ,tMaxZ,subX,tMaxX,subY,tMaxY);
	}
	else if ( n_shiftDirection == Z_POS || n_shiftDirection == Z_NEG ) {
		SET_AXES(subX,tMaxX,subY,tMaxY,subZ,tMaxZ);
	}
#undef SET_AXES


	if ( n_shiftDirection == X_POS || n_shiftDirection == Y_POS || n_shiftDirection == Z_POS )
	{
		// This shifts the data using pointers to coordinates, only on the negative direction.
		for ( *trSubmajor0 = 0; *trSubmajor0 < *tSubmajorMax0; *trSubmajor0 += 1 )
		{
			for ( *trSubmajor1 = 0; *trSubmajor1 < *tSubmajorMax1; *trSubmajor1 += 1 )
			{
				SectorState* tSectorCurrent = NULL;
				SectorState* tSectorPrevious;
				for ( *trMajor0 = 0; *trMajor0 < *tMajorMax0; *trMajor0 += 1 )
				{
					tSectorPrevious = tSectorCurrent; // Set pointers
					tSectorCurrent = &(m_state_sectors[ subX + subY*tMaxX + subZ*tMaxX*tMaxY ]);
					if ( *trMajor0 == 0 ) {
						continue;
					}
					// Move tracker current's data to tracker previous
					*tSectorPrevious = *tSectorCurrent; // Performs swaps on the lists
					tSectorPrevious->m_area_serviced = false; // Clear service flag.
					tSectorPrevious->m_area_busy.clear();
					tSectorPrevious->m_area_gamedata_busy.clear();
					tSectorPrevious->m_area_gamedata_used.clear();
				}
				// And clear out tTrackerCurrent's information
				//(*tSectorCurrent) = SectorState();
				tSectorCurrent->m_has_data = false;
				tSectorCurrent->m_lod_level = ushort(-1);
				tSectorCurrent->m_area_serviced = false;
				tSectorCurrent->m_has_mesh = false;
				tSectorCurrent->m_has_collision = false;
			}
		}
	}
	else
	{
		// This shifts the data using pointers to coordinates, only on the positive direction.
		for ( *trSubmajor0 = 0; *trSubmajor0 < *tSubmajorMax0; *trSubmajor0 += 1 )
		{
			for ( *trSubmajor1 = 0; *trSubmajor1 < *tSubmajorMax1; *trSubmajor1 += 1 )
			{
				SectorState* tSectorCurrent = NULL;
				SectorState* tSectorPrevious;
				for ( *trMajor0 = *tMajorMax0-1; *trMajor0 >= 0; *trMajor0 -= 1 )
				{
					tSectorPrevious = tSectorCurrent;
					tSectorCurrent = &(m_state_sectors[ subX + subY*tMaxX + subZ*tMaxX*tMaxY ]);
					if ( *trMajor0 == *tMajorMax0-1 ) {
						continue;
					}
					// Move tracker current's data to tracker previous
					*tSectorPrevious = *tSectorCurrent;
					tSectorPrevious->m_area_serviced = false; // Clear service flag.
					tSectorPrevious->m_area_busy.clear();
					tSectorPrevious->m_area_gamedata_busy.clear();
					tSectorPrevious->m_area_gamedata_used.clear();
				}
				// And clear out tTrackerCurrent's information
				//(*tSectorCurrent) = SectorState();
				tSectorCurrent->m_has_data = false;
				tSectorCurrent->m_lod_level = ushort(-1);
				tSectorCurrent->m_area_serviced = false;
				tSectorCurrent->m_has_mesh = false;
				tSectorCurrent->m_has_collision = false;
			}
		}
	}
}


// Creates indexer information for the given position
void COctreeTerrain::SectorGetAtWorldPosition ( const Vector3d& n_approximatePosition, int32_t& o_subx, int32_t& o_suby, int32_t& o_subz, uint32_t& o_indexer )
{
	int32_t subX, subY, subZ;
	int32_t tMaxX, tMaxY, tMaxZ;

	tMaxX = (int32_t)m_treeStartResolution/32;
	tMaxY = (int32_t)m_treeStartResolution/32;
	tMaxZ = (int32_t)m_treeStartResolution/64;

	SectorState* tSector;
	Vector3d area_min;
	Vector3d area_max;
	RangeVector area_index;
	vector<TerrainJob> jobsToAdd;

	// Get the sector
	subX = (int32_t)floorf(n_approximatePosition.x/64.0f);
	subY = (int32_t)floorf(n_approximatePosition.y/64.0f);
	subZ = (int32_t)floorf(n_approximatePosition.z/64.0f);

	// Grab the sector indexer
	o_subx = subX + tMaxX/2;
	o_suby = subY + tMaxY/2;
	o_subz = subZ + tMaxZ/2;
	o_indexer = o_subx + o_suby*tMaxX + o_subz*tMaxX*tMaxY;

	if ( (o_subx < 0 || o_suby < 0 || o_subz < 0) || (o_subx >= tMaxX || o_suby >= tMaxY || o_subz >= tMaxZ) )
	{
		throw std::out_of_range("Position out of range of terrain");
	}
}


// Request regeneration on a position
void COctreeTerrain::Sectors_Request_NewMesh ( const Vector3d& n_position )
{
	int32_t subX, subY, subZ;

	Vector3d area_min;
	Vector3d area_max;
	RangeVector area_index;
	vector<TerrainJob> jobsToAdd;

	// Get the sector
	subX = (int32_t)floorf(n_position.x/64.0f);
	subY = (int32_t)floorf(n_position.y/64.0f);
	subZ = (int32_t)floorf(n_position.z/64.0f);

	// Generate generation area
	area_index = RangeVector(
		subX + m_state_centerIndex.x,
		subY + m_state_centerIndex.y,
		subZ + m_state_centerIndex.z );
	area_min = Vector3d( area_index.x-m_state_centerIndex.x, area_index.y-m_state_centerIndex.y, area_index.z-m_state_centerIndex.z ) * 32 * blockSize;
	area_max = area_min + (Vector3d( 32,32,32 ) * blockSize);

	// Add generation job
	TerrainJob newJob( area_index );
	newJob.area_min = area_min;
	newJob.area_max = area_max;
	newJob.type = TERRAJOB_RECALCULATE_NORMALS;
	jobsToAdd.push_back( newJob );
	newJob.type = TERRAJOB_REMESH_AREA;
	jobsToAdd.push_back( newJob );

	// Batch add jobs
	Job_Enqueue( jobsToAdd );
}


// Terrain outside management
Terrain::SectorGameData* COctreeTerrain::AquireGameDataForEdit ( const Vector3d& n_approximatePosition, uint32_t& o_indexer )
{
	int32_t subX, subY, subZ;
	uint32_t subW;
	SectorState* tSector;

	SectorGetAtWorldPosition( n_approximatePosition, subX, subY, subZ, subW );
	tSector = &(m_state_sectors[subW]);

	if ( tSector->m_area_gamedata_used.test_and_set() )
	{
		throw std::exception("E_THREAD: Attempting to aquire sector for gamedata edit when previous reference not done");
	}
	else
	{
		o_indexer = subW;
		return &(tSector->m_game_data);
	}
}
void COctreeTerrain::ReleaseGameDataForEdit ( Terrain::SectorGameData* n_gameData, const uint32_t& n_indexer )
{
	SectorState* tSector;
	tSector = &(m_state_sectors[n_indexer]);

	if ( &(tSector->m_game_data) == n_gameData )
	{
		tSector->m_area_gamedata_used.clear();
	}
	else
	{
		// Need to search the state sector data

		// for now, just throw an error
		throw std::exception("E_THREAD: Have not implemented moving gamedata reference");
	}
}


Terrain::SectorGameDataState*	COctreeTerrain::AquireGameDataStateForEdit ( const Vector3d& n_approximatePosition, uint32_t& o_indexer )
{
	int32_t subX, subY, subZ;
	uint32_t subW;
	SectorState* tSector;

	SectorGetAtWorldPosition( n_approximatePosition, subX, subY, subZ, subW );
	tSector = &(m_state_sectors[subW]);

	if ( tSector->m_area_gamedata_busy.test_and_set() )
	{
		throw std::exception("E_THREAD: Attempting to aquire sector for gamedata edit when previous reference not done");
	}
	else
	{
		o_indexer = subW;
		return &(tSector->m_game_data_state);
	}
}
void	COctreeTerrain::ReleaseGameDataStateForEdit ( Terrain::SectorGameDataState* n_gameState, const uint32_t& n_indexer )
{
	SectorState* tSector;
	tSector = &(m_state_sectors[n_indexer]);

	if ( &(tSector->m_game_data_state) == n_gameState )
	{
		tSector->m_area_gamedata_busy.clear();
	}
	else
	{
		// Need to search the state sector data

		// for now, just throw an error
		throw std::exception("E_THREAD: Have not implemented moving gamedata reference");
	}
}
