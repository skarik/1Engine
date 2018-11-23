#include "CRenderable3D.h"
#include "renderer/material/RrPass.h"
#include "renderer/gpuw/Device.h"

CRenderable3D::CRenderable3D ( void )
	: CRenderableObject()
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
	// Still have to release buffers
	/*if ( m_buffer_verts != NIL ) {
		GL.FreeBuffer( &m_buffer_verts );
		m_buffer_verts = NIL;
	}
	if ( m_buffer_tris != NIL ) {
		GL.FreeBuffer( &m_buffer_tris );
		m_buffer_tris = NIL;
	}*/
	m_meshBuffer.FreeMeshBuffers();
}

//		PushModelData()
// Takes the information inside of m_modeldata and pushes it to the GPU so that it may be rendered.
void CRenderable3D::PushModeldata ( void )
{
	m_meshBuffer.InitMeshBuffers(&m_modeldata);
}

//		PreRender()
// Push the uniform properties
bool CRenderable3D::PreRender ( rrCameraPass* cameraPass )
{
	PushCbufferPerObject(transform.world, cameraPass);
	return true;
}

//		Render()
// Render the model using the 2D engine's style
bool CRenderable3D::Render ( const rrRenderParams* params )
{ 
	//// Do not render if no buffer to render with
	//if ( m_buffer_verts == 0 || m_buffer_tris == 0 )
	//{
	//	return true;
	//}

	//// For now, we will render the same way as the 3d meshes render
	//m_material->m_bufferSkeletonSize = 0;
	//m_material->m_bufferMatricesSkinning = 0;
	//m_material->bindPass(pass);
	//BindVAO( pass, m_buffer_verts, m_buffer_tris );
	//GL.DrawElements( GL_TRIANGLES, m_modeldata.triangleNum*3, GL_UNSIGNED_INT, 0 );

	// otherwise we will render the same way 3d meshes render
	{
		if ( !m_meshBuffer.m_mesh_uploaded )
			return true; // Only render when have a valid mesh and rendering enabled

		gpu::GraphicsContext* gfx = gpu::getDevice()->getContext();

		gpu::Pipeline* pipeline = GetPipeline( params->pass );
		gfx->setPipeline(pipeline);
		// bind the vertex buffers
		for (int i = 0; i < renderer::kAttributeMaxCount; ++i)
			if (m_meshBuffer.m_bufferEnabled[i])
				gfx->setVertexBuffer(i, &m_meshBuffer.m_buffer[i], 0);
		// bind the index buffer
		gfx->setIndexBuffer(&m_meshBuffer.m_indexBuffer, gpu::kFormatR16UInteger);
		// bind the cbuffers: TODO
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_OBJECT_MATRICES, &m_cbufPerObjectMatrices);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_OBJECT_EXTENDED, &m_cbufPerObjectSurface);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_CAMERA_INFORMATION, params->cbuf_perCamera);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_PASS_INFORMATION, params->cbuf_perPass);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_FRAME_INFORMATION, params->cbuf_perFrame);
		// draw now
		gfx->drawIndexed(m_modeldata.indexNum, 0);
	}

	// Success!
	return true;
}