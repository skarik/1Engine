#include "render2d/object/sprite/CStreamedRenderable2D.h"

#include "gpuw/Device.h"
#include "renderer/texture/RrTexture.h"
#include "renderer/material/RrShaderProgram.h"
#include "renderer/material/RrPass.h"
#include "renderer/material/Material.h"

CStreamedRenderable2D::CStreamedRenderable2D ( void )
	: CRenderable2D(), m_wants_swap(false)
{
	// Start with empty buffers and vertex counts
	//m_model_tricount = 0;
	//m_buffer_verts = NIL;
	//m_buffer_tris = NIL;

	//m_next_model_tricount = 0;
	//m_next_buffer_verts = NIL;
	//m_next_buffer_tris = NIL;

	memset(m_meshData, 0, sizeof(m_meshData));

	for (int i = 0; i < m_meshBufferRing.size(); ++i)
	{
		m_meshBufferRing.m_ring.at(i).m_modeldata = &m_meshData[i];
	}
}
CStreamedRenderable2D::~CStreamedRenderable2D ( void )
{ 
	/*// Still have to release buffers
	if ( m_next_buffer_verts != NIL ) {
		GL.FreeBuffer( &m_next_buffer_verts );
		m_next_buffer_verts = NIL;
	}
	if ( m_next_buffer_tris != NIL ) {
		GL.FreeBuffer( &m_next_buffer_tris );
		m_next_buffer_tris = NIL;
	}*/
	//m_nextMeshBuffer.FreeMeshBuffers();
	m_meshBufferRing.freeMeshBuffers();
}

// Return access to model data
arModelData* CStreamedRenderable2D::GetModelData ( void )
{
	//return &m_modeldata;
	return m_meshBufferRing.getToEdit()->m_modeldata;
}

//		PreRender()
// Push the uniform properties
bool CStreamedRenderable2D::PreRender ( rrCameraPass* cameraPass )
{
	PushCbufferPerObject(transform.world, cameraPass);
	return true;
}

// Push the current stuff in model data to GPU.
void CStreamedRenderable2D::StreamLockModelData ( void )
{
	// Temporarily swap so we are working on the "next" buffer data
	/*std::swap( m_buffer_verts, m_next_buffer_verts );
	std::swap( m_buffer_tris, m_next_buffer_tris );
	std::swap( m_model_tricount, m_next_model_tricount );*/

	// Push the model data to the GPU
	/*PushModeldata();
	// Pull the new tricount
	m_model_tricount = m_modeldata.triangleNum;

	// Swap back to the previous frame's mesh
	std::swap( m_buffer_verts, m_next_buffer_verts );
	std::swap( m_buffer_tris, m_next_buffer_tris );
	std::swap( m_model_tricount, m_next_model_tricount );*/
	m_meshBufferRing.getToEdit()->StreamMeshBuffers(GetModelData());
	m_meshBufferRing.incrementAfterEdit();

	// We want to swap to new meshes next frame
	//m_wants_swap = true;
}

//		Render()
// Render the model using the 2D engine's style
bool CStreamedRenderable2D::Render ( const rrRenderParams* params )
{
	//// Do not render if no buffer to render with
	//if ( m_buffer_verts == 0 || m_buffer_tris == 0 )
	//{
	//	return true;
	//}// TODO: Double check that per-object uniforms are sent to the videocard.

	//// For now, we will render the same way as the 3d meshes render
	////GL.Transform( &(transform.world) );
	//m_material->m_bufferSkeletonSize = 0;
	//m_material->m_bufferMatricesSkinning = 0;
	//m_material->bindPass(pass);
	////parent->SendShaderUniforms(this);
	//BindVAO( pass, m_buffer_verts, m_buffer_tris );
	//{// TODO: fix this hot mess
	//	// Bind target buffers
	//	GL.BindBuffer( GL_ARRAY_BUFFER, m_buffer_verts );
	//	GL.BindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_buffer_tris );
	//	m_material->bindPassAtrribs();
	//}
	//GL.DrawElements( GL_TRIANGLES, m_model_tricount * 3, GL_UNSIGNED_INT, 0 );

	// otherwise we will render the same way 3d meshes render
	{
		auto meshBuffer = m_meshBufferRing.getToRender();
		if ( !meshBuffer->m_mesh_uploaded || meshBuffer->m_modeldata == NULL || meshBuffer->m_modeldata->indexNum == 0 )
			return true; // Only render when have a valid mesh and rendering enabled

		gpu::GraphicsContext* gfx = params->context_graphics;

		gpu::Pipeline* pipeline = GetPipeline( params->pass );
		gfx->setPipeline(pipeline);
		// Set up the material helper...
		renderer::Material(this, gfx, params->pass, pipeline)
			// set the depth & blend state registers
			.setDepthStencilState()
			.setRasterizerState()
			// bind the samplers & textures
			.setBlendState()
			.setTextures();
		// bind the vertex buffers
		for (int i = 0; i < renderer::shader::kVBufferSlotMaxCount; ++i)
			if (meshBuffer->m_bufferEnabled[i])
				gfx->setVertexBuffer(i, &meshBuffer->m_buffer[i], 0);
		// bind the index buffer
		gfx->setIndexBuffer(&meshBuffer->m_indexBuffer, gpu::kIndexFormatUnsigned16);
		// bind the cbuffers
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_OBJECT_MATRICES, &m_cbufPerObjectMatrices);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_OBJECT_EXTENDED, &m_cbufPerObjectSurfaces[params->pass]);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_CAMERA_INFORMATION, params->cbuf_perCamera);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_PASS_INFORMATION, params->cbuf_perPass);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_FRAME_INFORMATION, params->cbuf_perFrame);
		// draw now
		gfx->drawIndexed(meshBuffer->m_modeldata->indexNum, 0, 0);
	}

	// Success!
	return true;
}

//		EndRender()
// Called after the frame is being rendered. Swaps the currently used buffers
bool CStreamedRenderable2D::EndRender ( void )
{
	/*if ( m_wants_swap )
	{
		m_meshBufferRing.incrementAfterEdit();
		m_wants_swap = false;
	}*/

	return true;
}
