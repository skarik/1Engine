#include "Mesh.h"
#include "renderer/logic/model/RrCModel.h"
//#include "renderer/logic/model/CSkinnedModel.h"
#include "renderer/object/mesh/system/rrMeshBuffer.h"
#include "renderer/object/mesh/system/rrSkinnedMesh.h"
#include "renderer/camera/RrCamera.h"
#include "renderer/types/shaders/sbuffers.h"
#include "gpuw/Device.h"
#include "gpuw/GraphicsContext.h"
#include "renderer/material/Material.h"

#include "renderer/state/RrRenderer.h"

renderer::Mesh::Mesh ( rrMeshBuffer* nMesh, bool n_enableSkinning )
	: RrRenderObject(),
	m_mesh(nMesh), /*m_parent(NULL),*/
	/*bUseFrustumCulling(true), */bCanRender(true), bUseSkinning(n_enableSkinning)
{
	/*if ( m_mesh != NULL )
	{
		//SetMaterial( m_glMesh->pmMat );
		CalculateBoundingBox();
	}*/
	//ARCORE_ERROR("Set up pipeline");
}

renderer::Mesh::~Mesh ( void )
{
	// CMesh does not manage the input meshes, only grabs buffers and renders them.
}

//void renderer::Mesh::CalculateBoundingBox ( void )
//{
//	//Vector3f minPos, maxPos;
//
//	//arModelData* modeldata = m_mesh->m_modeldata;
//	//for ( unsigned int v = 0; v < modeldata->vertexNum; v++ )
//	//{
//	//	//arModelVertex* vert = &(modeldata->vertices[v]);
//	//	Vector3f* vert = &(modeldata->position[v]);
//	//	minPos.x = std::min<Real>( minPos.x, vert->x );
//	//	minPos.y = std::min<Real>( minPos.y, vert->y );
//	//	minPos.z = std::min<Real>( minPos.z, vert->z );
//	//	maxPos.x = std::max<Real>( maxPos.x, vert->x );
//	//	maxPos.y = std::max<Real>( maxPos.y, vert->y );
//	//	maxPos.z = std::max<Real>( maxPos.z, vert->z );
//	//}
//
//	//vMinExtents = minPos;
//	//vMaxExtents = maxPos;
//	//vCheckRenderPos = (minPos+maxPos)/2;
//	//fCheckRenderDist = (( maxPos-minPos )/2).magnitude();
//	////bbCheckRenderBox.Set( transform.GetTransformMatrix(), vMinExtents, vMaxExtents );
//	//bbCheckRenderBox.Set( Matrix4x4(), vMinExtents, vMaxExtents );
//	////bbCheckRenderBox.Set( transform.GetTransformMatrix(), Vector3f( -0.1f,-0.1f,-0.1f ), Vector3f( 0.1f,0.1f,0.1f ) );
//}

// == RENDERABLE OBJECT INTERFACE ==

bool renderer::Mesh::CreateConstants ( rrCameraPass* cameraPass )
{
	// disable frustum culling:
	//bUseFrustumCulling = false;

	//// Culling check routine
	//if ( bUseFrustumCulling )
	//{
	//	bCanRender = false;

	//	if ( !bUseSkinning )
	//	{
	//		if ( cameraPass->m_frustum.SphereIsInside(
	//			transform.WorldMatrix() * vCheckRenderPos,
	//			fCheckRenderDist * (transform.world.scale.x+transform.world.scale.y+transform.world.scale.z) * 0.4f ) != core::math::kShapeCheckResultOutside )
	//		{
	//			bCanRender = true;
	//		}
	//	}
	//	else
	//	{
	//		Vector3f modelOrigin;
	//		//modelOrigin = ((CSkinnedModel*)m_parent)->GetSkeleton()->current_transform[1].WorldMatrix() * vCheckRenderPos;
	//		modelOrigin = transform.WorldMatrix() * modelOrigin;
	//		if ( cameraPass->m_frustum.SphereIsInside(
	//			modelOrigin, 
	//			fCheckRenderDist * (transform.world.scale.x+transform.world.scale.y+transform.world.scale.z) * 0.6f ) != core::math::kShapeCheckResultOutside )
	//		{
	//			bCanRender = true;
	//		}
	//	}
	//}
	//else
	//{
	//	bCanRender = true;
	//}

	//// If can render, then push the uniform buffers
	//if (bCanRender)
	//{
	//	// Set up transformation for the mesh
	//	if ( m_parent )
	//		PushCbufferPerObject(m_parent->transform, cameraPass);
	//	else
			PushCbufferPerObject(this->transform.world, cameraPass);
	//}

	// 
	return true;
}

// Render the mesh
bool renderer::Mesh::Render ( const rrRenderParams* params )
{
	if (m_mesh->m_modeldata->indexNum)
	{
		if ( !bCanRender || m_mesh == NULL )
			return true; // Only render when have a valid mesh and rendering enabled

		gpu::GraphicsContext* gfx = params->context->context_graphics;

		gpu::Pipeline* pipeline = GetPipeline( params->pass, m_mesh->m_bufferEnabled );
		gfx->setPipeline(pipeline);

		// Set up the material helper...
		renderer::Material(this, params->context, params, pipeline)
			// set the depth & blend state registers
			.setDepthStencilState()
			.setRasterizerState()
			// bind the samplers & textures
			.setBlendState()
			.setTextures()
			// execute callback
			.executePassCallback();

		// bind the vertex buffers
		auto passAccess = PassAccess(params->pass);
		for (int i = 0; i < passAccess.getVertexSpecificationCount(); ++i)
		{
			int buffer_index = (int)passAccess.getVertexSpecification()[i].location;
			int buffer_binding = (int)passAccess.getVertexSpecification()[i].binding;
			if (m_mesh->m_bufferEnabled[buffer_index])
				gfx->setVertexBuffer(buffer_binding, &m_mesh->m_buffer[buffer_index], 0);
			else
				gfx->setVertexBuffer(buffer_binding, &RrRenderer::Active->GetDefaultVertexBuffer(), 0);
		}

		// bind the index buffer
		gfx->setIndexBuffer(&m_mesh->m_indexBuffer, gpu::kIndexFormatUnsigned16);

		// bind the cbuffers
#if 0
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_OBJECT_MATRICES, &m_cbufPerObjectMatrices);
		gfx->setShaderCBuffer(gpu::kShaderStagePs, renderer::CBUFFER_PER_OBJECT_MATRICES, &m_cbufPerObjectMatrices);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_OBJECT_EXTENDED, &m_cbufPerObjectSurfaces[params->pass]);
		gfx->setShaderCBuffer(gpu::kShaderStagePs, renderer::CBUFFER_PER_OBJECT_EXTENDED, &m_cbufPerObjectSurfaces[params->pass]);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_CAMERA_INFORMATION, params->cbuf_perCamera);
		gfx->setShaderCBuffer(gpu::kShaderStagePs, renderer::CBUFFER_PER_CAMERA_INFORMATION, params->cbuf_perCamera);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_FRAME_INFORMATION, params->cbuf_perFrame);
		gfx->setShaderCBuffer(gpu::kShaderStagePs, renderer::CBUFFER_PER_FRAME_INFORMATION, params->cbuf_perFrame);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_PASS_INFORMATION, params->cbuf_perPass);
		gfx->setShaderCBuffer(gpu::kShaderStagePs, renderer::CBUFFER_PER_PASS_INFORMATION, params->cbuf_perPass);
#else
		gpu::Buffer* cbuffers [5] = {
			&m_cbufPerObjectMatrices,
			&m_cbufPerObjectSurfaces[params->pass],
			params->cbuf_perCamera,
			params->cbuf_perFrame,
			params->cbuf_perPass};
		gfx->setShaderCBuffers(gpu::kShaderStageVs, 0, 5, cbuffers);
		gfx->setShaderCBuffers(gpu::kShaderStagePs, 0, 5, cbuffers);
#endif

		// draw now
		gfx->drawIndexed(m_mesh->m_modeldata->indexNum, 0, 0);
	}

	// Successful rendering
	return true;
}