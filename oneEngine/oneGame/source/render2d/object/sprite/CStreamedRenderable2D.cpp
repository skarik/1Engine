#include "render2d/object/sprite/CStreamedRenderable2D.h"

#include "renderer/material/RrMaterial.h"
#include "renderer/system/glMainSystem.h"

CStreamedRenderable2D::CStreamedRenderable2D ( void )
	: CRenderable2D(), m_wants_swap(false)
{
	// Start with empty buffers and vertex counts
	m_model_tricount = 0;
	m_buffer_verts = NIL;
	m_buffer_tris = NIL;

	m_next_model_tricount = 0;
	m_next_buffer_verts = NIL;
	m_next_buffer_tris = NIL;
}
CStreamedRenderable2D::~CStreamedRenderable2D ( void )
{ GL_ACCESS

	// Still have to release buffers
	if ( m_next_buffer_verts != NIL ) {
		GL.FreeBuffer( &m_next_buffer_verts );
		m_next_buffer_verts = NIL;
	}
	if ( m_next_buffer_tris != NIL ) {
		GL.FreeBuffer( &m_next_buffer_tris );
		m_next_buffer_tris = NIL;
	}
}

// Return access to model data
arModelData* CStreamedRenderable2D::GetModelData ( void )
{
	return &m_modeldata;
}

// Push the current stuff in model data to GPU.
void CStreamedRenderable2D::StreamLockModelData ( void )
{
	// Temporarily swap so we are working on the "next" buffer data
	std::swap( m_buffer_verts, m_next_buffer_verts );
	std::swap( m_buffer_tris, m_next_buffer_tris );
	std::swap( m_model_tricount, m_next_model_tricount );

	// Push the model data to the GPU
	PushModeldata();
	// Pull the new tricount
	m_model_tricount = m_modeldata.triangleNum;

	// Swap back to the previous frame's mesh
	std::swap( m_buffer_verts, m_next_buffer_verts );
	std::swap( m_buffer_tris, m_next_buffer_tris );
	std::swap( m_model_tricount, m_next_model_tricount );

	// We want to swap to new meshes next frame
	m_wants_swap = true;
}

//		Render()
// Render the model using the 2D engine's style
bool CStreamedRenderable2D::Render ( const char pass )
{ GL_ACCESS
	// Do not render if no buffer to render with
	if ( m_buffer_verts == 0 || m_buffer_tris == 0 )
	{
		return true;
	}

	// For now, we will render the same way as the 3d meshes render
	GL.Transform( &(transform.world) );
	m_material->m_bufferSkeletonSize = 0;
	m_material->m_bufferMatricesSkinning = 0;
	m_material->bindPass(pass);
	//parent->SendShaderUniforms(this);
	BindVAO( pass, m_buffer_verts, m_buffer_tris );
	{// TODO: fix this hot mess
		// Bind target buffers
		GL.BindBuffer( GL_ARRAY_BUFFER, m_buffer_verts );
		GL.BindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_buffer_tris );
		m_material->bindPassAtrribs( pass );
	}
	GL.DrawElements( GL_TRIANGLES, m_model_tricount * 3, GL_UNSIGNED_INT, 0 );

	// Success!
	return true;
}

//		EndRender()
// Called after the frame is being rendered. Swaps the currently used buffers
bool CStreamedRenderable2D::EndRender ( void )
{
	if ( m_wants_swap )
	{
		std::swap( m_buffer_verts, m_next_buffer_verts );
		std::swap( m_buffer_tris, m_next_buffer_tris );
		std::swap( m_model_tricount, m_next_model_tricount );
		m_wants_swap = false;
	}

	return true;
}
