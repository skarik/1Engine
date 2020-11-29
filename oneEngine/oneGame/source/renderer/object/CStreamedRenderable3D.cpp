#include "renderer/object/CStreamedRenderable3D.h"
#include "gpuw/Device.h"
#include "renderer/material/Material.h"

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
		// Set up the material helper...
		renderer::Material(this, gfx, params->pass, pipeline)
		// set the depth & blend state registers
			.setDepthStencilState()
			.setRasterizerState()
		// bind the samplers & textures
			.setBlendState()
			.setTextures()
		// post-material cb
			.execute(m_postMaterialCb);
		// bind the vertex buffers
		//for (int i = 0; i < renderer::shader::kVBufferSlotMaxCount; ++i)
		//	if (m_currentMeshBuffer->m_bufferEnabled[i])
		//		gfx->setVertexBuffer(this->PassAccess(params->pass). m_slotbindings[i], &m_currentMeshBuffer->m_buffer[i], 0);
		auto passAccess = PassAccess(params->pass);
		for (int i = 0; i < passAccess.getVertexSpecificationCount(); ++i)
		{
			int buffer_index = (int)passAccess.getVertexSpecification()[i].location;
			int buffer_binding = (int)passAccess.getVertexSpecification()[i].binding;
			if (m_currentMeshBuffer->m_bufferEnabled[buffer_index])
				gfx->setVertexBuffer(buffer_binding, &m_currentMeshBuffer->m_buffer[buffer_index], 0);
		}
		// bind the index buffer
		gfx->setIndexBuffer(&m_currentMeshBuffer->m_indexBuffer, gpu::kIndexFormatUnsigned16);
		// bind the cbuffers
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_OBJECT_MATRICES, &m_cbufPerObjectMatrices);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_OBJECT_EXTENDED, &m_cbufPerObjectSurfaces[params->pass]);
		gfx->setShaderCBuffer(gpu::kShaderStagePs, renderer::CBUFFER_PER_OBJECT_EXTENDED, &m_cbufPerObjectSurfaces[params->pass]);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_CAMERA_INFORMATION, params->cbuf_perCamera);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_PASS_INFORMATION, params->cbuf_perPass);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_FRAME_INFORMATION, params->cbuf_perFrame);
		// draw now
		gfx->drawIndexed(m_model_indexcount, 0, 0);
	}

	// Success!
	return true;
}

//		EndRender()
// Called after the frame is being rendered. Swaps the currently used buffers
bool CStreamedRenderable3D::EndRender ( void )
{
	if ( m_wants_swap )
	{
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
