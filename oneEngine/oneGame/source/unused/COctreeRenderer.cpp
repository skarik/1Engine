
#include "COctreeRenderer.h"

#include "CTexture3D.h"
#include "CWorldState.h"

#include "TerrainTypes.h"

#include "CGameSettings.h"

COctreeRenderer::COctreeRenderer ( COctreeTerrain* nterrain )
	: CRenderableObject(), m_terrain(nterrain), m_render_wireframe(false)
{
	m_material_land = new glMaterial;
	m_material_land->setTexture( 0, new CTexture3D( ".res/textures/terraTexture.jpg", Texture3D, RGBA8, 4, 4 ) );
	m_material_land->setTexture( 1, new CTexture3D( ".res/textures/terraTextureLM.jpg", Texture3D, RGBA8, 4, 4 ) );

	m_material_land->passinfo.push_back( glPass() );
	m_material_land->passinfo[0].shader = new glShader( ".res/shaders/world/terrainDefault.glsl" );

	m_material_land->passinfo.push_back( glPass() );
	m_material_land->passinfo[1].shader = new glShader( ".res/shaders/world/terrainDefault.glsl" );
	m_material_land->passinfo[1].m_hint = RL_SKYGLOW;

	m_material_land->passinfo.push_back( glPass() );
	m_material_land->passinfo[2].shader = new glShader( ".res/shaders/world/terrainDefault.glsl" );
	m_material_land->passinfo[2].m_hint = RL_SHADOW_COLOR;


	m_material_land->deferredinfo.push_back( glPass_Deferred() );
	m_material_land->deferredinfo[0].m_diffuse_method = Renderer::Deferred::DIFFUSE_TERRAIN;

	this->SetMaterial( m_material_land );

	if ( CGameSettings::Active()->i_ro_RendererMode == RENDER_MODE_FORWARD )
	{
		m_material_land_attrib_vertex = m_material_land->passinfo[0].shader->get_attrib_location( "mdl_Vertex" );
		m_material_land_attrib_texcoord=m_material_land->passinfo[0].shader->get_attrib_location( "mdl_TexCoord" );
		m_material_land_attrib_color  = m_material_land->passinfo[0].shader->get_attrib_location( "mdl_Color" );
		m_material_land_attrib_normal = m_material_land->passinfo[0].shader->get_attrib_location( "mdl_Normal" );
		m_material_land_attrib_blend1 = m_material_land->passinfo[0].shader->get_attrib_location( "ter_Blends1" );
		m_material_land_attrib_blend2 = m_material_land->passinfo[0].shader->get_attrib_location( "ter_Blends2" );
	} 
	else if ( CGameSettings::Active()->i_ro_RendererMode == RENDER_MODE_DEFERRED )
	{
		m_material_land->bindPassDeferred(0); // This will build the shader

		m_material_land_attrib_vertex = m_material_land->deferredinfo[0].GetShader()->get_attrib_location( "mdl_Vertex" );
		m_material_land_attrib_texcoord=m_material_land->deferredinfo[0].GetShader()->get_attrib_location( "mdl_TexCoord" );
		m_material_land_attrib_color  = m_material_land->deferredinfo[0].GetShader()->get_attrib_location( "mdl_Color" );
		m_material_land_attrib_normal = m_material_land->deferredinfo[0].GetShader()->get_attrib_location( "mdl_Normal" );
		m_material_land_attrib_blend1 = m_material_land->deferredinfo[0].GetShader()->get_attrib_location( "ter_Blends1" );
		m_material_land_attrib_blend2 = m_material_land->deferredinfo[0].GetShader()->get_attrib_location( "ter_Blends2" );
	}

	m_forceCleared = false;
}

COctreeRenderer::~COctreeRenderer ( void )
{
	//delete m_material_land;
	for ( int i = 0; i < 3; ++i ) {
		delete [] m_upload_requests[i].m_mesh_triangle_buffer;
		m_upload_requests[i].m_mesh_triangle_buffer = NULL;

		delete [] m_upload_requests[i].m_mesh_vertex_buffer;
		m_upload_requests[i].m_mesh_vertex_buffer = NULL;
	}
	// Delete the meshes
	for ( auto itr = m_mesh_areas.begin(); itr != m_mesh_areas.end(); ++itr )
	{
		if ( itr->m_buf_faces ) {
			glDeleteBuffers( 1, &itr->m_buf_faces );
		}
		if ( itr->m_buf_verts ) {
			glDeleteBuffers( 1, &itr->m_buf_verts );
		}
		if ( itr->m_buf_vao ) {
			glDeleteVertexArrays( 1, &itr->m_buf_faces );
		}
	}
}

void COctreeRenderer::OffsetSystem ( const Vector3d& n_offset, const RangeVector& n_index_offset )
{
	m_generationjobs_lock.lock();
	m_generationjobs.clear();
	m_generationjobs_lock.unlock();

	m_forceCleared = true;

	for ( auto itr = m_mesh_areas.begin(); itr != m_mesh_areas.end(); ++itr )
	{
		itr->m_min += n_offset;
		itr->m_max += n_offset;
		itr->m_center += n_offset;

		itr->m_draw_offset += n_offset;
		if ( itr->m_rigidbody )
		{
			hkVector4 rigidbodyPosition = itr->m_rigidbody->getPosition();
			rigidbodyPosition.add( hkVector4( n_offset.x, n_offset.y, n_offset.z ) );
			itr->m_rigidbody->setPosition( rigidbodyPosition );
		}
		/*itr->m_index.x += n_index_offset.x;
		itr->m_index.y += n_index_offset.y;
		itr->m_index.z += n_index_offset.z;*/
		//std::cout << "INDEX: " << itr->m_index.x << std::endl;
	}
}

Vector3d COctreeRenderer::AreaMeshSorter::centerPosition = Vector3d::zero;
bool COctreeRenderer::AreaMeshSorter::operator() ( const AreaMesh* meshA, const AreaMesh* meshB )
{
	// Front-to-back rendering, return TRUE to place A first.
	if ( (meshA->m_center-centerPosition).sqrMagnitude() < (meshB->m_center-centerPosition).sqrMagnitude() ) {
		return true;
	}
	return false;
}

bool COctreeRenderer::PreRender( const char pass )
{
	// Perform culling over areas
	for ( auto itr = m_mesh_areas.begin(); itr != m_mesh_areas.end(); ++itr )
	{
		itr->m_visible = CCamera::activeCamera->SphereIsVisible( itr->m_center, 56.0f );
	}

	// Create the rendering list
	AreaMeshSorter::centerPosition = CCamera::activeCamera->transform.position;
	for ( auto itr = m_mesh_areas.begin(); itr != m_mesh_areas.end(); ++itr )
	{
		if ( itr->m_visible && itr->m_current_triangle_count > 0 )
		{
			if ( itr->m_buf_verts && itr->m_buf_faces ) {
				m_render_areas.push( &(*itr) );
			}
			else {
				std::cout << "Area with triangles has no buffer! Maybe trying to draw before upload done?" << std::endl;
			}
		}
	}

	return true;
}
bool COctreeRenderer::Render ( const char pass )
{
	transform.position = Vector3d(0,0,0);

	int diffusePass = 0;

	// If in shadow mode, only cast from the terrain, not the water
	if ( CCamera::activeCamera->shadowCamera )
		if ( pass != diffusePass )
			return true;

	// Disable culling for shadow camera
	if ( CCamera::activeCamera->shadowCamera )
		glDisable( GL_CULL_FACE );

	m_material->bindPass(pass);
	m_material->setShaderConstants( this );
	if ( ActiveGameWorld ) {
		m_material->setUniform( "terra_BaseAmbient", ActiveGameWorld->cBaseAmbient );
	}
	else {
		m_material->setUniform( "terra_BaseAmbient", Color(0,0,0,1) );
	}

	// Set MISC render modes
	if ( m_render_wireframe ) {
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	}
	// Loop through all areas
	/*for ( auto itr = m_mesh_areas.begin(); itr != m_mesh_areas.end(); ++itr )
	{
		if ( itr->m_visible && itr->m_current_triangle_count > 0 )
		{
			if ( itr->m_buf_verts && itr->m_buf_faces ) {
				GL.prepareDraw();
				GL.Translate( itr->m_draw_offset );
				DrawMesh(*itr);
				GL.cleanupDraw();
			}
			else {
				std::cout << "Area with triangles has no buffer! Maybe trying to draw before upload done?" << std::endl;
			}
		}
	}*/
	while ( !m_render_areas.empty() )
	{
		GL.prepareDraw();
		GL.Translate( m_render_areas.top()->m_draw_offset );
		DrawMesh( *m_render_areas.top() );
		GL.cleanupDraw();
		m_render_areas.pop();
	}
	// Reset MISC render modes
	if ( m_render_wireframe ) {
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	}

	// Re-enable culling
	if ( CCamera::activeCamera->shadowCamera )
		glEnable( GL_CULL_FACE );

	return true;
}

void COctreeRenderer::DrawMesh ( const AreaMesh& n_area )
{
	// Draw current area
	m_material->setShaderConstants( this );
	GL.CheckError();

	glBindVertexArray( n_area.m_buf_vao );
	GL.CheckError();
	
	// Draw the stuff
	glDrawElements( GL_TRIANGLES, n_area.m_current_triangle_count*3, GL_UNSIGNED_INT, 0 );
	GL.CheckError();
}

