
#include "COctreeTerrain.h"
#include "COctreeIO.h"
#include <thread>

void COctreeTerrain::Job_CalculatePriority ( TerrainJob& n_jobToEdit )
{
	n_jobToEdit.priority = 9 * GetResolutionAtPosition( (n_jobToEdit.area_max+n_jobToEdit.area_min)*0.5 );
	n_jobToEdit.priority += (int)(Vector3d(n_jobToEdit.index.x-m_state_centerIndex.x,n_jobToEdit.index.y-m_state_centerIndex.y,n_jobToEdit.index.z-m_state_centerIndex.z).sqrMagnitude()*4);
	switch ( n_jobToEdit.type ) { // this switch case is ordered by priority to visualize job order
		case TERRAJOB_GENERATE:
			n_jobToEdit.priority -= 2;
			break;
		case TERRAJOB_LOAD_SECTOR:
			n_jobToEdit.priority -= 1;
			break;
		case TERRAJOB_SAVE_SECTOR:
			//n_jobToEdit.priority += 1;
			break;
		case TERRAJOB_RECALCULATE_NORMALS:
			n_jobToEdit.priority += 1;
			break;
		case TERRAJOB_CONDENSE_AREA:
			n_jobToEdit.priority += 2;
			break;
		case TERRAJOB_REMESH_AREA:
			n_jobToEdit.priority = n_jobToEdit.priority*4-1;
			break;
	}
	n_jobToEdit.priority += ((8*n_jobToEdit.delay+64)/(-8-n_jobToEdit.delay))+16;//n_jobToEdit.delay/2;
}

void COctreeTerrain::Job_Enqueue ( const TerrainJob& n_jobToAdd )
{
	// Calculate job priority
	TerrainJob editedJob ( n_jobToAdd );
	Job_CalculatePriority( editedJob );

	// Add new job to list
	mutex::scoped_lock enqueueLock ( m_joblist_lock );
	Job_Work_EnqueueToList( editedJob );
}
void COctreeTerrain::Job_Enqueue ( vector<TerrainJob>& n_jobsToAdd )
{
	// Edit in-place
	for ( uint i = 0; i < n_jobsToAdd.size(); ++i )
	{
		// Calculate job priority
		Job_CalculatePriority( n_jobsToAdd[i] );
	}

	mutex::scoped_lock enqueueLock ( m_joblist_lock );
	// Enqueue as a batch
	for ( uint i = 0; i < n_jobsToAdd.size(); ++i )
	{
		// Add new jobs to list
		Job_Work_EnqueueToList( n_jobsToAdd[i] );
	}
}
void COctreeTerrain::Job_Work_EnqueueToList( const TerrainJob& n_jobToAdd )
{
	auto findResult = std::find( m_joblist.begin(), m_joblist.end(), n_jobToAdd );
	if ( findResult == m_joblist.end() )
	{
		m_joblist.push_back( n_jobToAdd );
		std::push_heap( m_joblist.begin(), m_joblist.end() );
	}
	else
	{
		m_joblist.erase(findResult);
		std::make_heap( m_joblist.begin(), m_joblist.end() );
		m_joblist.push_back( n_jobToAdd );
		std::push_heap( m_joblist.begin(), m_joblist.end() );
	}
}


bool COctreeTerrain::Job_Dequeue ( TerrainJob& o_jobToUse )
{
	mutex::scoped_lock dequeueLock ( m_joblist_lock );
	if ( m_joblist.empty() ) {
		return false;
	}
	else {
		std::pop_heap( m_joblist.begin(), m_joblist.end() );
		o_jobToUse = m_joblist.back();
		m_joblist.pop_back();
		return true;
	}
}

void COctreeTerrain::Job_StartThreads ( void )
{
	m_threads_run = false;
	// Generate thread count to spread work on.
	uint threadCount = std::min<uint>( std::thread::hardware_concurrency() - 1, 5 );
	if ( threadCount == 0 ) {
		threadCount = 2; // Default to 2 side threads for tasks
	}

	// Create the threads now
	for ( uint i = 0; i < threadCount; ++i )
	{
		TerrainThread threadCaller;
		threadCaller.m_terrain = this;
		threadCaller.m_id = i;
		m_threads.push_back( new thread( threadCaller ) );
	}
	
	// Set the thread time tracker size to the threads
	m_threadtimes.resize(m_threads.size());

	// Mark threads can now run.
	m_threads_run = true;
}
void COctreeTerrain::Job_StopThreads ( void )
{
	m_threads_run = false;
	for ( uint i = 0; i < m_threads.size(); ++i ) 
	{
		std::cout << "Closing thread " << i << std::endl;
		m_threads[i]->join();
		delete m_threads[i];
		std::cout << "  closed thread." << std::endl;
	}
}
void COctreeTerrain::Job_Clear ( void )
{
	mutex::scoped_lock clearLock ( m_joblist_lock );
	m_joblist.clear();
}

#include "COctreeRenderer.h"
#include "Zones.h"

void COctreeTerrain::Job_Step ( const int n_threadid )
{
	const char tJobnames [6][32] = {
		"TERRAJOB_GENERATE        ",
		">TERRAJOB_REMESH_AREA    ",
		"TERRAJOB_RECALCULATE_NORMALS ",
		"TERRAJOB_LOAD_SECTOR     ",
		"TERRAJOB_SAVE_SECTOR     ",
		"TERRAJOB_CONDENSE_AREA   "
	};

	std::list<TerrainJob>	m_job_delay_list;

	while ( !m_threads_run ) {
		// Spin until threads ready
		std::this_thread::yield();
	}

	// Spin while threads keep going. (Is this the right way? Shouldn't we have a queue, and a side thread that dispatches jobs?)
	while ( m_threads_run )
	{
		m_threadtimes[n_threadid] = Time::GetCurrentCPUTime();
		if ( m_systemPaused || (mIO == NULL) ) {
			std::this_thread::yield();
			continue; // Do no work if system is paused or held in an invalid state.
		}
		TerrainJob job ( RangeVector(0,0,0) );
		if ( Job_Dequeue( job ) )
		{
			Terrain::read_lock jobLock( m_state_synchronizeLock );

			Vector3d job_center = (job.area_min+job.area_max)/2;
			RangeVector index = job.index;//= Zones.PositionToRV( job_center );

			//cout << n_threadid << " Performing job " << tJobnames[job.type] << " at " << index.x << " " << index.y << " " << index.z << endl;
			// Check index
			try
			{
				if ( abs(index.x) > 26000 || abs(index.y) > 26000 || abs(index.z) > 13500 ) {
					throw std::out_of_range( "Bad index" );
				}
			}
			catch ( const std::exception& e )
			{
				std::cout << "Index issues: " << e.what() << std::endl;
				std::cout << "  index: " << index.x << " " << index.y << " " << index.z << std::endl;
				continue;
			}

			SectorState* m_sector = NULL;
			{	// Find target sector that currently working on
				const int tMaxX = m_treeStartResolution/32;
				const int tMaxY = m_treeStartResolution/32;
				const int tMaxZ = m_treeStartResolution/64;
				int sectorIndex =
					  (index.x-m_state_centerIndex.x + tMaxX/2)
					+ (index.y-m_state_centerIndex.y + tMaxY/2)*(tMaxX)
					+ (index.z-m_state_centerIndex.z + tMaxZ/2)*(tMaxX*tMaxY);
				if ( sectorIndex >= 0 && sectorIndex < (tMaxX*tMaxY*tMaxZ) ) {
					m_sector = &(m_state_sectors[sectorIndex]);
				}
				if ( m_sector ) {
					while ( m_sector->m_area_busy.test_and_set() ) {
						std::this_thread::yield();
					}
				}
			}
			switch ( job.type )
			{
				case TERRAJOB_REMESH_AREA:
					{
						//Terrain::read_lock infolock( m_joblist_major_operation_lock );
						bool foundSlot = false;
						for ( int i = 0; i < 3; ++i )
						{
							if ( m_renderer->m_upload_requests[i].m_meshgeneration_state.set_if_same_as(COctreeRenderer::MESH_GENERATION_IDLE, COctreeRenderer::MESH_GENERATION_MAKING_MESH) )
							{
								//Terrain::write_lock speedlock( m_joblist_major_operation_lock );
								m_renderer->Job_QueueGenerationRequest( index, job.area_min, job.area_max );
								m_renderer->Job_DequeueGenerationRequest( m_renderer->m_upload_requests[i] );
								// Got a slot, so don't need to work this
								foundSlot = true;
								// Mark sector as having data
								if ( m_sector ) {
									m_sector->m_has_mesh = true;
								}
								break; // Break out of loop.
							}
						}
						if ( !foundSlot )
						{
							job.delay += 1;
							//Job_Enqueue( job );
							m_job_delay_list.push_back( job );
						}
					}
					break;
				case TERRAJOB_RECALCULATE_NORMALS:
					{
						//Terrain::read_lock infolock( m_joblist_major_operation_lock );
						//Edit_EstimateNormals( job.area_min, job.area_max, (Real)GetResolutionAtPosition( job_center ) );
					}
					break;
				case TERRAJOB_GENERATE:
					{
						int checkFlag = 0;
						while ( m_threads_run )
						{
							if ( checkFlag == 0 && !m_sidebuffer0_open.test_and_set() )
							{
								// Generate into subbuffer0
								//DebugGenerateTestSubarea( m_sidebuffer0, job.area_min, job.area_max );
								//Generation_GenerateSubarea( m_sidebuffer0, job.area_min, job.area_max );
								m_gamedata_sidebuffer0.m_grass.clear();
								m_gamedata_sidebuffer0.m_foliage_queue.clear();
								m_gamedata_sidebuffer0.m_component_queue.clear();
								Generation_GenerateSubarea( m_sidebuffer0, &m_gamedata_sidebuffer0, index );
								if ( mIO->SaveSector( m_sidebuffer0, &m_gamedata_sidebuffer0, index ) ) {
									// saved the data
								}
								else {
									// save failed
								}
								m_sidebuffer0_open.clear();
								break;
							}
							else if ( checkFlag == 1 && !m_sidebuffer1_open.test_and_set() )
							{
								// Generate into subbuffer0
								//DebugGenerateTestSubarea( m_sidebuffer1, job.area_min, job.area_max );
								//Generation_GenerateSubarea( m_sidebuffer1, job.area_min, job.area_max );
								m_gamedata_sidebuffer1.m_grass.clear();
								m_gamedata_sidebuffer1.m_foliage_queue.clear();
								m_gamedata_sidebuffer1.m_component_queue.clear();
								Generation_GenerateSubarea( m_sidebuffer1, &m_gamedata_sidebuffer1, index );
								if ( mIO->SaveSector( m_sidebuffer1, &m_gamedata_sidebuffer1, index ) ) {
									// saved the data
								}
								else {
									// save failed
								}
								m_sidebuffer1_open.clear();
								break;
							}
							checkFlag = (checkFlag+1)%2;
							// While looking for a generation space, yield.
							std::this_thread::yield();
						}
					}
					break;
				case TERRAJOB_LOAD_SECTOR:
					{
						// Lock gamedata
						if ( m_sector ) {
							while ( m_sector->m_area_gamedata_busy.test_and_set() ) {
								std::this_thread::yield();
							}
						}
						// Get the sector to load to
						Terrain::Sector* tTargetSector;
						SampleSector( job_center, tTargetSector, 64 );
						if ( tTargetSector == NULL ) {
							Terrain::write_lock infolock( m_joblist_major_operation_lock );
							// Subdivide the entire terrain first to allow for tighter movement
							SubdivideStart( m_root, m_treeStartResolution );
							SampleSector( job_center, tTargetSector, 64 );
						}
						Terrain::read_lock infolock( m_joblist_major_operation_lock );
						if ( tTargetSector != NULL )
						{
							// Clear data
							/*m_sector->m_game_data.m_loaded_small = false;
							m_sector->m_game_data.m_loaded_big = false;
							m_sector->m_game_data.m_grass.clear();
							m_sector->m_game_data.m_component_queue.clear();
							m_sector->m_game_data.m_foliage_queue.clear();*/
							// Perform load
							if ( mIO->LoadSector( tTargetSector, &(m_sector->m_game_data), index, GetResolutionAtPosition( job_center ) ) ) {
								// All's good! :D
							}
							else {
								// Else, need to make two more jobs: a generation and a load.
								Job_Enqueue( job );
								job.type = TERRAJOB_GENERATE;
								Job_Enqueue( job );
								job.type = TERRAJOB_RECALCULATE_NORMALS;
								Job_Enqueue( job );
							}
						}
						else {
							std::cout << "invalid area (" << index.x << "," << index.y << "," << index.z << ")was still in job list" << std::endl;
						}
						// Unlock gamedata
						if ( m_sector ) {
							m_sector->m_area_gamedata_busy.clear();
						}
					}
					break;
				case TERRAJOB_SAVE_SECTOR:
					{
						// Lock gamedata
						if ( m_sector ) {
							while ( m_sector->m_area_gamedata_busy.test_and_set() ) {
								std::this_thread::yield();
							}
						}
						// Get the sector to save
						Terrain::Sector* tTargetSector;
						SampleSector( job_center, tTargetSector, 64 );
						if ( tTargetSector == NULL ) {
							Terrain::write_lock infolock( m_joblist_major_operation_lock );
							// Subdivide the entire terrain first to allow for tighter movement
							SubdivideStart( m_root, m_treeStartResolution );
							SampleSector( job_center, tTargetSector, 64 );
						}
						Terrain::read_lock infolock( m_joblist_major_operation_lock );
						if ( tTargetSector != NULL )
						{
							if ( mIO->SaveSector( tTargetSector, &(m_sector->m_game_data), index ) ) {
								// All's good! :D
							}
							else {
								// If the saving fails, not sure what to do, actually.
							}
						}
						else {
							std::cout << "invalid area (" << index.x << "," << index.y << "," << index.z << ")was still in job list" << std::endl;
						}
						// Unlock gamedata
						if ( m_sector ) {
							m_sector->m_area_gamedata_busy.clear();
						}
					}
					break;
			} // End switch case
			// Clear sector usage
			if ( m_sector ) {
				m_sector->m_area_busy.clear();
			}
			// Sleep for a short time to give space to main thread (yay).
			//std::this_thread::sleep_for( std::chrono::milliseconds(5) );
		}
		// Yield after done with job
		std::this_thread::yield();
		// If can, add the target jobs back into the list
		if ( !m_job_delay_list.empty() )
		{
			// Check types of jobs that can be re-added
			bool renderer_foundSlot = false;
			for ( int i = 0; i < 3; ++i )
			{
				if ( m_renderer->m_upload_requests[i].m_meshgeneration_state.get() == COctreeRenderer::MESH_GENERATION_IDLE )
				{
					renderer_foundSlot = true;
				}
			}

			// Iterate through the job list (TODO: change to a for loop)
			for ( auto itr = m_job_delay_list.begin(); itr != m_job_delay_list.end(); )
			{
				TerrainJob& job = *itr;
				if ( job.type == TERRAJOB_REMESH_AREA )
				{
					if ( renderer_foundSlot ) {
						Job_Enqueue( job );
						itr = m_job_delay_list.erase(itr);
						continue;
					}
				}
				++itr;
			}
			// End of add back in
		}
		// End of job loop
	}
}