#include "CRenderable3D.h"
#include "gpuw/Device.h"
#include "renderer/material/RrPass.h"
#include "renderer/material/Material.h"

CRenderable3D::CRenderable3D ( void )
	: RrRenderObject()
{
	// Start with empty buffers
	//m_buffer_verts = NIL;
	//m_buffer_tris = NIL;

	// Start off with empty model data
	memset(&m_modeldata, 0, sizeof(m_modeldata));
	m_modeldata.indexNum = 0;
	m_modeldata.vertexNum = 0;
}

CRenderable3D::~CRenderable3D ( void )
{ 
	m_meshBuffer.FreeMeshBuffers();
}

//		PushModelData()
// Takes the information inside of m_modeldata and pushes it to the GPU so that it may be rendered.
void CRenderable3D::PushModeldata ( void )
{
	m_meshBuffer.InitMeshBuffers(&m_modeldata);
}

//		CreateConstants()
// Push the uniform properties
bool CRenderable3D::CreateConstants ( rrCameraPass* cameraPass )
{
	PushCbufferPerObject(transform.world, cameraPass);
	return true;
}

//		Render()
// Render the model using the 2D engine's style
bool CRenderable3D::Render ( const rrRenderParams* params )
{ 
	// otherwise we will render the same way 3d meshes render
	{
		if ( !m_meshBuffer.m_mesh_uploaded )
			return true; // Only render when have a valid mesh and rendering enabled

		gpu::GraphicsContext* gfx = params->context->context_graphics;

		gpu::Pipeline* pipeline = GetPipeline( params->pass );
		gfx->setPipeline(pipeline);
		// Set up the material helper...
		renderer::Material(this, params->context, params, pipeline)
			// set the depth & blend state registers
			.setDepthStencilState()
			.setRasterizerState()
			// bind the samplers & textures
			.setBlendState()
			.setTextures();
		// bind the vertex buffers
		for (int i = 0; i < renderer::shader::kVBufferSlotMaxCount; ++i)
			if (m_meshBuffer.m_bufferEnabled[i])
				gfx->setVertexBuffer(i, &m_meshBuffer.m_buffer[i], 0);
		// bind the index buffer
		gfx->setIndexBuffer(&m_meshBuffer.m_indexBuffer, gpu::kIndexFormatUnsigned16);
		// bind the cbuffers
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_OBJECT_MATRICES, &m_cbufPerObjectMatrices);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_OBJECT_EXTENDED, &m_cbufPerObjectSurfaces[params->pass]);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_CAMERA_INFORMATION, params->cbuf_perCamera);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_PASS_INFORMATION, params->cbuf_perPass);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_FRAME_INFORMATION, params->cbuf_perFrame);
		// draw now
		gfx->drawIndexed(m_modeldata.indexNum, 0, 0);
	}

	// Success!
	return true;
}