#include "CRenderable3D.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"
#include "renderer/material/RrMaterial.h"

CRenderable3D::CRenderable3D ( void )
	: CRenderableObject()
{
	// Start with empty buffers
	m_buffer_verts = NIL;
	m_buffer_tris = NIL;

	// Start off with empty model data
	m_modeldata.triangleNum = 0;
	m_modeldata.triangles = NULL;
	m_modeldata.vertexNum = 0;
	m_modeldata.vertices = NULL;
}

CRenderable3D::~CRenderable3D ( void )
{ GL_ACCESS

	// Still have to release buffers
	if ( m_buffer_verts != NIL ) {
		GL.FreeBuffer( &m_buffer_verts );
		m_buffer_verts = NIL;
	}
	if ( m_buffer_tris != NIL ) {
		GL.FreeBuffer( &m_buffer_tris );
		m_buffer_tris = NIL;
	}
}

//		PushModelData()
// Takes the information inside of m_modeldata and pushes it to the GPU so that it may be rendered.
void CRenderable3D::PushModeldata ( void )
{ GL_ACCESS
	GL.BindVertexArray( 0 );

	// Create new buffers
	if ( m_buffer_verts == NIL )
		GL.CreateBuffer( &m_buffer_verts );
	if ( m_buffer_tris == NIL )
		GL.CreateBuffer( &m_buffer_tris );
	//bShaderSetup = false; // With making new buffers, shader is now not ready

	// Bind to some buffer objects
	GL.BindBuffer( GL_ARRAY_BUFFER,			m_buffer_verts ); // for vertex coordinates
	GL.BindBuffer( GL_ELEMENT_ARRAY_BUFFER,	m_buffer_tris ); // for face vertex indexes

	// Copy data to the buffer
	GL.UploadBuffer( GL_ARRAY_BUFFER,
		sizeof(arModelVertex) * (m_modeldata.vertexNum),
		m_modeldata.vertices,
		GL_STATIC_DRAW );
	GL.UploadBuffer( GL_ELEMENT_ARRAY_BUFFER,
		sizeof(arModelTriangle) * (m_modeldata.triangleNum),
		m_modeldata.triangles,
		GL_STATIC_DRAW );

	// bind with 0, so, switch back to normal pointer operation
	GL.UnbindBuffer( GL_ARRAY_BUFFER );
	GL.UnbindBuffer( GL_ELEMENT_ARRAY_BUFFER );
}

//		PreRender()
// Push the uniform properties
bool CRenderable3D::PreRender ( void )
{
	m_material->prepareShaderConstants(transform.world);
	return true;
}

//		Render()
// Render the model using the 2D engine's style
bool CRenderable3D::Render ( const char pass )
{ GL_ACCESS
	// Do not render if no buffer to render with
	if ( m_buffer_verts == 0 || m_buffer_tris == 0 )
	{
		return true;
	}

	// For now, we will render the same way as the 3d meshes render
	m_material->m_bufferSkeletonSize = 0;
	m_material->m_bufferMatricesSkinning = 0;
	m_material->bindPass(pass);
	//parent->SendShaderUniforms(this);
	BindVAO( pass, m_buffer_verts, m_buffer_tris );
	GL.DrawElements( GL_TRIANGLES, m_modeldata.triangleNum*3, GL_UNSIGNED_INT, 0 );

	//GL.endOrtho();
	// Success!
	return true;
}