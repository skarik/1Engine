
#include "COctreeRenderer.h"
#include "COctreeTerrain.h"
#include "Math.h"
#include "CTimeProfiler.h"

// Clear terrain mesh and the entire job queue
void COctreeRenderer::ClearTerrainMesh ( void )
{
	// Spinlock until out of job area
	bool forceSpinlock;
	do
	{
		forceSpinlock = false;
		for ( int i = 0; i < 3; ++i ) {
			if ( m_upload_requests[i].m_meshgeneration_state.get() == MESH_GENERATION_MAKING_MESH ) {
				forceSpinlock = true;
			}
		}
	} while ( forceSpinlock );

	// Now lock generation jobs
	m_generationjobs_lock.lock();

	// Clear after out of job area
	m_generationjobs.clear();

	// Clear off all data
	for ( auto itr = m_mesh_areas.begin(); itr != m_mesh_areas.end(); ++itr )
	{
		// Clear the OpenGL buffers
		if ( itr->m_buf_faces ) {
			glDeleteBuffers( 1, &itr->m_buf_faces );
			itr->m_buf_faces = 0;
			itr->m_triangle_count = 0;
			itr->m_current_triangle_count = 0;
		}
		if ( itr->m_buf_verts ) {
			glDeleteBuffers( 1, &itr->m_buf_verts );
			itr->m_buf_verts = 0;
			itr->m_vertex_count = 0;
		}
		// (Save the VAO though)

		// Remove the rigidbody
		if ( itr->m_rigidbody ) {
			Physics::FreeRigidBody(itr->m_rigidbody);
			itr->m_rigidbody = NULL;
		}
	}

	m_generationjobs_lock.unlock();
}


// Marching cubes test
void COctreeRenderer::GenerateMesh ( AreaUploadRequest& n_request )
{
	AreaMesh* tArea = n_request.m_area;
	tArea->m_vertex_count = 0;
	tArea->m_triangle_count = 0;

	// Create the memory for the mesh
	if ( n_request.m_mesh_vertex_buffer == NULL ) {
		n_request.m_mesh_vertex_buffer = new CTerrainVertex[1024*32*8];
	}
	if ( n_request.m_mesh_triangle_buffer == NULL ) {
		n_request.m_mesh_triangle_buffer = new CModelTriangle[1024*16*8];
	}

	//string profile = "meshgen"+std::to_string( (int)n_request.m_area );
	//TimeProfiler.BeginTimeProfile( profile );
	// Generate the mesh
	CreateWorldMesh(tArea,n_request.m_mesh_vertex_buffer,n_request.m_mesh_triangle_buffer);
	/*if ( n_request.m_area->m_vertex_count > 0 ) {
		TimeProfiler.EndPrintTimeProfile( profile );
	}*/

	// Now create or update the Havok mesh
	//CreateWorldCollision(vertices,triangles);

	if ( tArea->m_vertex_count > 0 ) {
		n_request.m_meshgeneration_state.set( MESH_GENERATION_WAITING_FOR_UPLOAD );
	}
	else {
		if ( tArea->m_buf_verts ) {
			n_request.m_meshgeneration_state.set( MESH_GENERATION_WAITING_FOR_UPLOAD ); // still need to run it to delete old meshes
		}
		else {
			n_request.m_meshgeneration_state.set( MESH_GENERATION_IDLE );
		}
	}
}

#include "COctreeMesher.h"
void COctreeRenderer::CreateWorldMesh ( AreaMesh* n_area, CTerrainVertex* vertices, CModelTriangle* triangles )
{
	//cout << " ...generating mesh...";

	uint vertexCount = 0;
	uint triangleCount = 0;

	{
		COctreeMesher mesher ( m_terrain, this );
		mesher.m_min = n_area->m_min;
		mesher.m_max = n_area->m_max;
		mesher.m_step = (Real)m_terrain->GetResolutionAtPosition( (n_area->m_min+n_area->m_max)/2 ); // TODO

		mesher.m_split_border[0] = 0.1f < fabs( mesher.m_step - m_terrain->GetResolutionAtPosition( (n_area->m_min+n_area->m_max)/2 - Vector3d(64,0,0) ) );
		mesher.m_split_border[1] = 0.1f < fabs( mesher.m_step - m_terrain->GetResolutionAtPosition( (n_area->m_min+n_area->m_max)/2 + Vector3d(64,0,0) ) );
		mesher.m_split_border[2] = 0.1f < fabs( mesher.m_step - m_terrain->GetResolutionAtPosition( (n_area->m_min+n_area->m_max)/2 - Vector3d(0,64,0) ) );
		mesher.m_split_border[3] = 0.1f < fabs( mesher.m_step - m_terrain->GetResolutionAtPosition( (n_area->m_min+n_area->m_max)/2 + Vector3d(0,64,0) ) );
		mesher.m_split_border[4] = 0.1f < fabs( mesher.m_step - m_terrain->GetResolutionAtPosition( (n_area->m_min+n_area->m_max)/2 - Vector3d(0,0,64) ) );
		mesher.m_split_border[5] = 0.1f < fabs( mesher.m_step - m_terrain->GetResolutionAtPosition( (n_area->m_min+n_area->m_max)/2 + Vector3d(0,0,64) ) );

		mesher.CreateMesh( vertices, triangles, vertexCount, triangleCount );
	}

	n_area->m_vertex_count = vertexCount;
	n_area->m_triangle_count = triangleCount;

	if ( n_area->m_vertex_count > 1024*32*8 || n_area->m_triangle_count > 1024*16*8 ) {
		throw std::out_of_range ( "Overflow on mesh vertex generation" );
	}
}

void COctreeRenderer::UpdateRequests ( void )
{
	if ( m_forceCleared ) {
		for ( int i = 0; i < 3; ++i )
		{
			if ( m_upload_requests[i].m_meshgeneration_state.get() == MESH_GENERATION_WAITING_FOR_UPLOAD )
			{
				m_upload_requests[i].m_meshgeneration_state.set( MESH_GENERATION_IDLE );
			}
		}
		return;
	}
	for ( int i = 0; i < 3; ++i )
	{
		if ( m_upload_requests[i].m_meshgeneration_state.get() == MESH_GENERATION_WAITING_FOR_UPLOAD )
		{
			UploadMesh( m_upload_requests[i].m_area, m_upload_requests[i].m_mesh_vertex_buffer, m_upload_requests[i].m_mesh_triangle_buffer );
			m_upload_requests[i].m_meshgeneration_state.set( MESH_GENERATION_IDLE );
			break; // Only one upload per step
		}
	}
	/*if ( m_meshgeneration_state.get() == MESH_GENERATION_WAITING_FOR_UPLOAD )
	{
		UploadMesh(m_current_area);
		m_meshgeneration_state.set( MESH_GENERATION_IDLE );
	}*/
}

void COctreeRenderer::UploadMesh ( AreaMesh* n_area, CTerrainVertex* n_vertex_buffer, CModelTriangle* n_triangle_buffer )
{
	n_area->m_draw_offset = Vector3d(0,0,0); // Reset draw offset (Havok uses this offset)

	// Now create or update the Havok mesh
	CreateWorldCollision(n_area,n_vertex_buffer,n_triangle_buffer);

	if ( n_area->m_vertex_count > 0 )
	{
		glBindVertexArray(0);

		if ( n_area->m_buf_verts == 0 ) {
			glGenBuffers( 1, &(n_area->m_buf_verts) );
		}
		if ( n_area->m_buf_faces == 0 ) {
			glGenBuffers( 1, &(n_area->m_buf_faces) );
		}

		glBindBuffer( GL_ARRAY_BUFFER, n_area->m_buf_verts );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, n_area->m_buf_faces );

		glBufferData( GL_ARRAY_BUFFER, sizeof(CTerrainVertex)*(n_area->m_vertex_count), NULL, GL_DYNAMIC_DRAW );
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(CModelTriangle)*(n_area->m_triangle_count), NULL, GL_DYNAMIC_DRAW );

		glBufferSubData( GL_ARRAY_BUFFER, 0,(n_area->m_vertex_count)*sizeof(CTerrainVertex), n_vertex_buffer );
		glBufferSubData( GL_ELEMENT_ARRAY_BUFFER, 0, (n_area->m_triangle_count)*sizeof(CModelTriangle), n_triangle_buffer );

		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

		// Set the VAO options
		{
			bool enableAttrib = false;
			if ( n_area->m_buf_vao == 0 )
			{
				glGenVertexArrays( 1, &(n_area->m_buf_vao) );
				enableAttrib = true;
			}

			glBindVertexArray( n_area->m_buf_vao );
			// Bind target buffers
			glBindBuffer( GL_ARRAY_BUFFER, n_area->m_buf_verts );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, n_area->m_buf_faces );

			if ( enableAttrib )
			{
				// Enabled array attributes
				glEnableVertexAttribArray( m_material_land_attrib_vertex );
				glEnableVertexAttribArray( m_material_land_attrib_texcoord );
				glEnableVertexAttribArray( m_material_land_attrib_color );
				glEnableVertexAttribArray( m_material_land_attrib_normal );
				glEnableVertexAttribArray( m_material_land_attrib_blend1 );
				glEnableVertexAttribArray( m_material_land_attrib_blend2 );
			}

			// Set attribute pointers
			glVertexAttribPointer( m_material_land_attrib_vertex, 3, GL_FLOAT, false, sizeof(CTerrainVertex), ((char*)0) + (sizeof(float)*0) );
			glVertexAttribPointer( m_material_land_attrib_texcoord,3,GL_FLOAT, false, sizeof(CTerrainVertex), ((char*)0) + (sizeof(float)*3) );
			glVertexAttribPointer( m_material_land_attrib_color,  4, GL_FLOAT, false, sizeof(CTerrainVertex), ((char*)0) + (sizeof(float)*12) );
			glVertexAttribPointer( m_material_land_attrib_normal, 3, GL_FLOAT, true,  sizeof(CTerrainVertex), ((char*)0) + (sizeof(float)*6) );
			glVertexAttribPointer( m_material_land_attrib_blend1, 4, GL_FLOAT, false, sizeof(CTerrainVertex), ((char*)0) + (sizeof(float)*16) );
			glVertexAttribPointer( m_material_land_attrib_blend2, 4, GL_FLOAT, false, sizeof(CTerrainVertex), ((char*)0) + (sizeof(float)*20) );

			glBindVertexArray(0);
		}

		//glBindBuffer( GL_ARRAY_BUFFER, 0 );
		//glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

		n_area->m_current_triangle_count = n_area->m_triangle_count;

		//cout << "done." << endl;

		GL.CheckError();
	}
	else
	{
		if ( n_area->m_buf_verts != 0 ) {
			glDeleteBuffers( 1, &(n_area->m_buf_verts) );
			n_area->m_buf_verts = 0;
		}
		if ( n_area->m_buf_faces != 0 ) {
			glDeleteBuffers( 1, &(n_area->m_buf_faces) );
			n_area->m_buf_faces = 0;
		}

		n_area->m_current_triangle_count = 0;

		//cout << "no mesh." << endl;
	}
}


void COctreeRenderer::CreateWorldCollision ( AreaMesh* n_area, CTerrainVertex* vertices, CModelTriangle* triangles )
{
	//static physRigidBody* m_rigidbody = NULL;
	if ( n_area->m_vertex_count > 0 )
	{
		hkGeometry geometry; 
		geometry.m_triangles.clear();
		geometry.m_vertices.clear();

		geometry.m_vertices.setSize( n_area->m_vertex_count );
		geometry.m_triangles.setSize( n_area->m_triangle_count );

		for ( uint vert = 0; vert < n_area->m_vertex_count; ++vert )
		{
			geometry.m_vertices[vert].setComponent<0>( vertices[vert].x );
			geometry.m_vertices[vert].setComponent<1>( vertices[vert].y );
			geometry.m_vertices[vert].setComponent<2>( vertices[vert].z );
		}
		for ( uint tri = 0; tri < n_area->m_triangle_count; ++tri )
		{
			geometry.m_triangles[tri].m_a = triangles[tri].vert[0];
			geometry.m_triangles[tri].m_b = triangles[tri].vert[1];
			geometry.m_triangles[tri].m_c = triangles[tri].vert[2];
		}

		// Geometry has been created
		hkpDefaultBvCompressedMeshShapeCinfo cInfo( &geometry );
		physShape* currentShape = new hkpBvCompressedMeshShape( cInfo );

		if ( n_area->m_rigidbody != NULL )
		{
			// Set to movable mode
			n_area->m_rigidbody->setMotionType( physMotion::MOTION_KEYFRAMED );
			n_area->m_rigidbody->setShape( currentShape );
			n_area->m_rigidbody->setPosition( hkVector4( n_area->m_draw_offset.x,n_area->m_draw_offset.y,n_area->m_draw_offset.z ) );
			n_area->m_rigidbody->setUserData( hkLong(COctreeTerrain::GetActive()->GetId()) );
			// Set to fixed mode
			n_area->m_rigidbody->setMotionType( physMotion::MOTION_FIXED );
		}
		else
		{
			physRigidBodyInfo info;
			info.m_shape = currentShape;									// Set the collision shape to the collider's
			info.m_motionType = physMotion::MOTION_KEYFRAMED;		// Set the motion to static
			// Create a rigidbody and assign it to the body variable.
			n_area->m_rigidbody = Physics::CreateRigidBody( &info, false );
			n_area->m_rigidbody->setUserData( hkLong(COctreeTerrain::GetActive()->GetId()) );
			n_area->m_rigidbody->setPosition( hkVector4( 0,0,0 ) );
			// Set body as fixed
			n_area->m_rigidbody->setMotionType( physMotion::MOTION_FIXED );
		}
		Physics::FreeShape(currentShape);
	}
	else
	{
		if ( n_area->m_rigidbody ) 
		{
			Physics::FreeRigidBody( n_area->m_rigidbody );
			n_area->m_rigidbody = NULL;
		}
	}
}
