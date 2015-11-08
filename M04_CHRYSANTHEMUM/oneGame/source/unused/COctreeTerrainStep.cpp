
#include "COctreeTerrain.h"
#include "COctreeRenderer.h"

#include "CInput.h"

#include "CCamera.h"

#include "CTerrainQueuedObjectLoader.h"
#include "CTerraGrass.h"
#include "CTerrainPropFactory.h"
#include "CTerraFoliageFactory.h"

void COctreeTerrain::Update ( void )
{
	// Step through the system, and draw a cube around areas with data
	//DebugDrawOctree();

	if ( !m_systemReady || m_systemPaused ) {
		//m_systemReady = true;
		//DebugCreateTestArea();
		// Nothing
	}
	else if ( m_systemRequestingShift )
	{
		Sectors_FollowTarget();
	}
	else
	{
		//m_renderer->Job_DequeueGenerationRequest();  // else where!
		m_renderer->UpdateRequests();
		if ( Input::Keydown('Y') ) {
			m_renderer->m_render_wireframe = !m_renderer->m_render_wireframe;
		}

		Sectors_FollowTarget();
		Sectors_GenerateJobs();

		// Loop through the sectors and update grass renderers
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

					// Generate generation area
					area_index = RangeVector(
						subX - tMaxX/2 + m_state_centerIndex.x,
						subY - tMaxY/2 + m_state_centerIndex.y,
						subZ - tMaxZ/2 + m_state_centerIndex.z );
					area_min = Vector3d( area_index.x-m_state_centerIndex.x, area_index.y-m_state_centerIndex.y, area_index.z-m_state_centerIndex.z ) * 32 * blockSize;
					area_max = area_min + (Vector3d( 32,32,32 ) * blockSize);

					// Create sector infos
					if ( tSector->m_lod_level <= m_subdivisionTarget )
					{
						if ( tSector->m_game_data_state.m_rendered_grass == NULL )
						{
							tSector->m_game_data_state.m_rendered_grass = new CTerraGrass( &(tSector->m_game_data.m_grass) );
							tSector->m_game_data_state.m_rendered_grass->transform.position = area_min;//(area_min+area_max)/2;
						}
						else
						{
							tSector->m_game_data_state.m_rendered_grass->pv_grasslist = &(tSector->m_game_data.m_grass);
							tSector->m_game_data_state.m_rendered_grass->transform.position = area_min;
							tSector->m_game_data_state.m_rendered_grass->Simulate();
						}
					}
					// Delete sector infos
					else
					{
						if ( tSector->m_game_data_state.m_rendered_grass != NULL )
						{
							delete tSector->m_game_data_state.m_rendered_grass;
							tSector->m_game_data_state.m_rendered_grass = NULL;
						}
					}
				}
			}
		}

		// Load all queued things
		{
			boost::mutex::scoped_lock dequeueLock ( m_queueloader->m_lock );
			while ( !m_queueloader->m_grassqueue.empty() )
			{
				Terrain::queuerequest_Grass& request = m_queueloader->m_grassqueue.back();

				sTerraGrass newGrass;
				newGrass.position = Vector3d( request.x, request.y, request.z );
				newGrass.type = request.grass_type;

				bool valid = true;
#ifdef _ENGINE_DEBUG
				for ( uint i = 0; i < request.target->m_grass.size(); ++i ) {
					if ( newGrass.position == request.target->m_grass[i].position ) {
						valid = false;
						break;
					}
				}
#endif//_ENGINE_DEBUG
				if ( valid ) {
					request.target->m_grass.push_back( newGrass );
				}
				else {
					Debug::Console->PrintWarning("BAD INFO\n");
				}
				m_queueloader->m_grassqueue.pop_back();
			}
			while ( !m_queueloader->m_foliagequeue.empty() )
			{
				Terrain::queuerequest_Foliage& request = m_queueloader->m_foliagequeue.back();
			
				Vector3d createPosition = request.position;
				createPosition.x += (request.index.x - m_state_centerIndex.x)*64;
				createPosition.y += (request.index.y - m_state_centerIndex.y)*64;
				createPosition.z += (request.index.z - m_state_centerIndex.z)*64;

				request.target->m_foliage.push_back( TerraFoliage::CreateFoliage( request.foliage_index, createPosition, request.userdata ) );

				m_queueloader->m_foliagequeue.pop_back();
			}
			while ( !m_queueloader->m_componentqueue.empty() )
			{
				Terrain::queuerequest_Component& request = m_queueloader->m_componentqueue.back();
			
				Vector3d createPosition = request.position;
				createPosition.x += (request.index.x - m_state_centerIndex.x)*64;
				createPosition.y += (request.index.y - m_state_centerIndex.y)*64;
				createPosition.z += (request.index.z - m_state_centerIndex.z)*64;

				BlockTrackInfo blockTracker;
				SampleBlockExpensive( createPosition, &blockTracker );

				CTerrainProp* newComponent = TerrainProp::Instantiate( request.component_index, blockTracker );
				if ( newComponent )
				{
					Debug::Console->PrintMessage( "LOADED COMPONENT AGAIN\n" );
					newComponent->transform.position = createPosition;
					newComponent->transform.rotation = request.rotation;
					newComponent->transform.scale = request.scale;
					newComponent->transform.SetDirty();
					request.target->m_components.push_back( newComponent );
				}

				m_queueloader->m_componentqueue.pop_back();
			}
		}
		//
	}

}

void COctreeTerrain::PostUpdate ( void )
{

}
