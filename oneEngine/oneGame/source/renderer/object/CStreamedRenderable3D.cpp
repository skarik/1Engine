#include "renderer/object/CStreamedRenderable3D.h"
#include "renderer/gpuw/Device.h"

CStreamedRenderable3D::CStreamedRenderable3D ( void )
	: CRenderable3D(), m_wants_swap(false)
{
	// Start with empty buffers and vertex counts
	m_currentMeshBuffer = NULL;

	// Start off with empty model data
	m_model_indexcount = 0;
	m_next_model_indexcount = 0;
}
CStreamedRenderable3D::~CStreamedRenderable3D ( void )
{ 
	// Still have to release buffers
	m_meshBufferAux.FreeMeshBuffers();
}

// Return access to model data
arModelData* CStreamedRenderable3D::GetModelData ( void )
{
	return &m_modeldata;
}

//		PreRender()
// Push the uniform properties
bool CStreamedRenderable3D::PreRender ( rrCameraPass* cameraPass )
{
	PushCbufferPerObject(transform.world, cameraPass);
	return true;
}

// Push the current stuff in model data to GPU.
void CStreamedRenderable3D::StreamLockModelData ( void )
{
	// Push the model data to the GPU
	if (m_modeldata.vertexNum > 0 || m_modeldata.indexNum > 0)
	{
		if (m_currentMeshBuffer != &m_meshBuffer)
			m_meshBuffer.InitMeshBuffers(&m_modeldata);
		else
			m_meshBufferAux.InitMeshBuffers(&m_modeldata);
	}

	// Pull the new index count
	m_next_model_indexcount = m_modeldata.indexNum;

	// We want to swap to new meshes next frame
	m_wants_swap = true;
}

//		Render()
// Render the model using the 2D engine's style
bool CStreamedRenderable3D::Render ( const rrRenderParams* params )
{ 
	// otherwise we will render the same way 3d meshes render
	{
		if ( m_model_indexcount == 0 || m_currentMeshBuffer == NULL || !m_currentMeshBuffer->m_mesh_uploaded )
			return true; // Only render when have a valid mesh and rendering enabled

		gpu::GraphicsContext* gfx = gpu::getDevice()->getContext();

		gpu::Pipeline* pipeline = GetPipeline( params->pass );
		gfx->setPipeline(pipeline);
		// bind the vertex buffers
		for (int i = 0; i < renderer::shader::kVBufferSlotMaxCount; ++i)
			if (m_currentMeshBuffer->m_bufferEnabled[i])
				gfx->setVertexBuffer(i, &m_currentMeshBuffer->m_buffer[i], 0);
		// bind the index buffer
		gfx->setIndexBuffer(&m_currentMeshBuffer->m_indexBuffer, gpu::kIndexFormatUnsigned16);
		// bind the cbuffers
		// TODO:
		// draw now
		gfx->drawIndexed(m_model_indexcount, 0);
	}

	//// Do not render if no buffer to render with
	//if ( m_buffer_verts == 0 || m_buffer_tris == 0 )
	//{
	//	return true;
	//}// TODO: Double check that per-object uniforms are sent to the videocard.

	// // For now, we will render the same way as the 3d meshes render
	//m_material->m_bufferSkeletonSize = 0;
	//m_material->m_bufferMatricesSkinning = 0;
	//m_material->bindPass(pass);
	//BindVAO( pass, m_buffer_verts, m_buffer_tris );
	//{// TODO: fix this hot mess
	// // Bind target buffers
	//	GL.BindBuffer( GL_ARRAY_BUFFER, m_buffer_verts );
	//	GL.BindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_buffer_tris );
	//	m_material->bindPassAtrribs();
	//}
	//GL.DrawElements( GL_TRIANGLES, m_model_tricount * 3, GL_UNSIGNED_INT, 0 );

	// Success!
	return true;
}

//		EndRender()
// Called after the frame is being rendered. Swaps the currently used buffers
bool CStreamedRenderable3D::EndRender ( void )
{
	if ( m_wants_swap )
	{
		//std::swap( m_buffer_verts, m_next_buffer_verts );
		//std::swap( m_buffer_tris, m_next_buffer_tris );
		//std::swap( m_model_tricount, m_next_model_tricount );

		// swap the targeted mesh
		if (m_currentMeshBuffer != &m_meshBuffer)
			m_currentMeshBuffer = &m_meshBuffer;
		else
			m_currentMeshBuffer = &m_meshBufferAux;

		// update the index count
		m_model_indexcount = m_next_model_indexcount;

		m_wants_swap = false;
	}

	return true;
}
