
#include "COctreeTerrain.h"
#include "COctreeRenderer.h"

void COctreeRenderer::Job_QueueGenerationRequest ( const RangeVector n_index, const Vector3d& n_min, const Vector3d& n_max )
{
	boost::mutex::scoped_lock enqueueLock ( m_generationjobs_lock );
	TerrainGenerationJob newJob;
	newJob.m_index = n_index;
	newJob.m_min = n_min;
	newJob.m_max = n_max;
	m_generationjobs.push_back( newJob );
}
void COctreeRenderer::Job_DequeueGenerationRequest ( AreaUploadRequest& n_request_slot )
{
	m_generationjobs_lock.lock();
	if ( m_generationjobs.empty() ) {
		m_generationjobs_lock.unlock();
		throw std::exception ( "Should not happen with this version of the engine" );
		// No jobs to perform, so exit.
		return;
	}
	else {
		// Dequeue job.
		TerrainGenerationJob job;
		job = m_generationjobs.front();
		m_generationjobs.pop_front();

		// New mesh, not forcing clear
		m_forceCleared = false;
		
		// Unlock list, don't need to read it anymore.
		m_generationjobs_lock.unlock();

		// Work on the job.
		AreaMesh* t_areaTarget;
		// First look through the current areas to see if there's something to edit.
		m_mesh_areas_lock.lock(); // First lock over area list
		int foundIndex = -1;
		/*for ( uint i = 0; i < m_mesh_areas.size(); ++i )
		{
			if ( m_mesh_areas[i].m_index == job.m_index ) {
				foundIndex = i;
				break;
			}
		}*/
		for ( auto itr = m_mesh_areas.begin(); itr != m_mesh_areas.end(); ++itr )
		{
			if ( (*itr).m_index == job.m_index ) {
				foundIndex = 0;
				t_areaTarget = &(*itr);
				break;
			}
		}
		// Grab the target area
		if ( foundIndex != -1 ) {
			//t_areaTarget = &m_mesh_areas[foundIndex];
		}
		else {
			// If the size is at capacity
			m_mesh_areas.push_front( AreaMesh() );
			//t_areaTarget = &m_mesh_areas[m_mesh_areas.size()-1];//m_mesh_areas.back();
			t_areaTarget = &m_mesh_areas.front();

			t_areaTarget->m_index = job.m_index;
			t_areaTarget->m_min = job.m_min;
			t_areaTarget->m_max = job.m_max;
			t_areaTarget->m_center = (job.m_min+job.m_max) * 0.5f;
		}
		m_mesh_areas_lock.unlock(); // unlock area list. We're not editing it, only reading from it.

		// Generate the target area
		n_request_slot.m_area			= t_areaTarget;

		GenerateMesh(n_request_slot);
	}
}
