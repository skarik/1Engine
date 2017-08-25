
#ifndef _C_OCTREE_RENDERER_H_
#define _C_OCTREE_RENDERER_H_

#include "CRenderableObject.h"
#include "CModelData.h"
#include "RangeVector.h"

// Physics common
#include "CPhysics.h"

#include "boost/thread.hpp"
#include "system/threads/future.h"
#include "system/threads/shared_variable.h"

#include <forward_list>
#include <queue>

class COctreeTerrain;

class COctreeRenderer : public CRenderableObject
{
	friend COctreeTerrain;

	ClassName( "COctreeRenderer" );

public:

	explicit			COctreeRenderer ( COctreeTerrain* );
						~COctreeRenderer ( void );

	bool				PreRender( const char pass ) override;
	bool				Render ( const char pass ) override;
	//unsigned char		GetPassNumber ( void ) override;
	//glMaterial*			GetPassMaterial ( const char pass ) override;

public:
	void				ClearTerrainMesh ( void );

private:
		COctreeTerrain*	m_terrain;

		glMaterial*		m_material_land;
		glMaterial*		m_material_water;

		int		m_material_land_attrib_vertex;
		int		m_material_land_attrib_color;
		int		m_material_land_attrib_normal;
		int		m_material_land_attrib_texcoord;
		int		m_material_land_attrib_blend1;
		int		m_material_land_attrib_blend2;

		bool	m_render_wireframe;

private:
	void				OffsetSystem ( const Vector3d&, const RangeVector& );

private:
	struct AreaMesh
	{
		bool		m_visible;

		GLuint		m_buf_verts;
		GLuint		m_buf_faces;
		GLuint		m_buf_vao;
		GLuint		m_vertex_count;
		GLuint		m_triangle_count;

		GLuint		m_current_triangle_count;

		physRigidBody*	m_rigidbody;

		Vector3d	m_center;
		Vector3d	m_min;
		Vector3d	m_max;

		Vector3d	m_draw_offset;

		RangeVector	m_index;

		explicit AreaMesh ( void )
			: m_visible(true),
			m_buf_verts(0), m_buf_faces(0), m_buf_vao(0),
			m_vertex_count(0), m_triangle_count(0), m_current_triangle_count(0),
			m_rigidbody(NULL),
			m_draw_offset(0,0,0)
		{
			;
		}
	};

private:

	void		DrawMesh ( const AreaMesh& );

private:
	// Mesh regeneration

	enum eMeshGenerationState
	{
		MESH_GENERATION_IDLE = 0,
		MESH_GENERATION_MAKING_MESH,
		MESH_GENERATION_WAITING_FOR_UPLOAD
	};
	struct AreaUploadRequest
	{
		AreaMesh*										m_area;
		Threads::shared_variable<eMeshGenerationState>	m_meshgeneration_state;

		CTerrainVertex*	m_mesh_vertex_buffer;
		CModelTriangle* m_mesh_triangle_buffer;

		AreaUploadRequest ( void ) 
			: m_area(NULL), m_meshgeneration_state(MESH_GENERATION_IDLE),
			m_mesh_vertex_buffer(NULL), m_mesh_triangle_buffer(NULL)
		{
			;
		}
		/*~AreaUploadRequest ( void )
		{
			if ( m_mesh_vertex_buffer ) {
				delete [] m_mesh_vertex_buffer;
			}
			m_mesh_vertex_buffer = NULL;
			if ( m_mesh_triangle_buffer ) {
				delete [] m_mesh_triangle_buffer;
			}
			m_mesh_triangle_buffer = NULL;
		}*/
	};
	AreaUploadRequest m_upload_requests [3];
	//vector<AreaUploadRequest> m_upload_requests;

	void		GenerateMesh ( AreaUploadRequest& );
	void		CreateWorldMesh ( AreaMesh*, CTerrainVertex*, CModelTriangle* );
	void		CreateWorldCollision ( AreaMesh*, CTerrainVertex*, CModelTriangle* );

	void		UpdateRequests ( void );
	void		UploadMesh ( AreaMesh*, CTerrainVertex*, CModelTriangle* );

private:
	// Area tracking
	void		Job_QueueGenerationRequest ( const RangeVector n_index, const Vector3d& n_min, const Vector3d& n_max );
	void		Job_DequeueGenerationRequest ( AreaUploadRequest& n_request_slot );

	bool		m_forceCleared;

	struct TerrainGenerationJob {
		Vector3d	m_min;
		Vector3d	m_max;
		RangeVector	m_index;
	};
	std::list<TerrainGenerationJob>	m_generationjobs;
	boost::mutex	m_generationjobs_lock;
	
	std::forward_list<AreaMesh>	m_mesh_areas;
	boost::mutex		m_mesh_areas_lock;

	struct AreaMeshSorter {
		static Vector3d centerPosition;
		bool operator() (const AreaMesh*, const AreaMesh*);
	};
	std::priority_queue<AreaMesh*,std::vector<AreaMesh*>,AreaMeshSorter> m_render_areas;
};

#endif//_C_OCTREE_RENDERER_H_