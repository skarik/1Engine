#include "Mesh.h"
#include "renderer/logic/model/RrCModel.h"
//#include "renderer/logic/model/CSkinnedModel.h"
#include "renderer/object/mesh/system/rrMeshBuffer.h"
#include "renderer/object/mesh/system/rrSkinnedMesh.h"
#include "renderer/camera/RrCamera.h"
#include "renderer/material/Material.h"
#include "renderer/types/shaders/sbuffers.h"
#include "renderer/gpuw/Device.h"
#include "renderer/gpuw/GraphicsContext.h"

renderer::Mesh::Mesh ( rrMeshBuffer* nMesh, bool n_enableSkinning )
	: CRenderableObject(),
	m_mesh(nMesh), m_parent(NULL),
	bUseFrustumCulling(true), bCanRender(true), bUseSkinning(n_enableSkinning)
{
	if ( m_mesh != NULL )
	{
		//SetMaterial( m_glMesh->pmMat );
		CalculateBoundingBox();
	}
}

renderer::Mesh::~Mesh ( void )
{
	// CMesh does not manage the input meshes, only wrangles buffers and renders them.
}

void renderer::Mesh::CalculateBoundingBox ( void )
{
	Vector3f minPos, maxPos;

	arModelData* modeldata = m_mesh->m_modeldata;
	for ( unsigned int v = 0; v < modeldata->vertexNum; v++ )
	{
		//arModelVertex* vert = &(modeldata->vertices[v]);
		Vector3f* vert = &(modeldata->position[v]);
		minPos.x = std::min<Real>( minPos.x, vert->x );
		minPos.y = std::min<Real>( minPos.y, vert->y );
		minPos.z = std::min<Real>( minPos.z, vert->z );
		maxPos.x = std::max<Real>( maxPos.x, vert->x );
		maxPos.y = std::max<Real>( maxPos.y, vert->y );
		maxPos.z = std::max<Real>( maxPos.z, vert->z );
	}

	vMinExtents = minPos;
	vMaxExtents = maxPos;
	vCheckRenderPos = (minPos+maxPos)/2;
	fCheckRenderDist = (( maxPos-minPos )/2).magnitude();
	//bbCheckRenderBox.Set( transform.GetTransformMatrix(), vMinExtents, vMaxExtents );
	bbCheckRenderBox.Set( Matrix4x4(), vMinExtents, vMaxExtents );
	//bbCheckRenderBox.Set( transform.GetTransformMatrix(), Vector3f( -0.1f,-0.1f,-0.1f ), Vector3f( 0.1f,0.1f,0.1f ) );
}

// == RENDERABLE OBJECT INTERFACE ==

bool renderer::Mesh::PreRender ( rrCameraPass* cameraPass )
{
	// disable frustum culling:
	bUseFrustumCulling = false;

	// Culling check routine
	if ( bUseFrustumCulling )
	{
		bCanRender = false;

		if ( !bUseSkinning )
		{
			if ( cameraPass->m_frustum.SphereIsInside(
				transform.WorldMatrix() * vCheckRenderPos,
				fCheckRenderDist * (transform.world.scale.x+transform.world.scale.y+transform.world.scale.z) * 0.4f ) != core::math::kShapeCheckResultOutside )
			{
				bCanRender = true;
			}
		}
		else
		{
			Vector3f modelOrigin;
			//modelOrigin = ((CSkinnedModel*)m_parent)->GetSkeleton()->current_transform[1].WorldMatrix() * vCheckRenderPos;
			modelOrigin = transform.WorldMatrix() * modelOrigin;
			if ( cameraPass->m_frustum.SphereIsInside(
				modelOrigin, 
				fCheckRenderDist * (transform.world.scale.x+transform.world.scale.y+transform.world.scale.z) * 0.6f ) != core::math::kShapeCheckResultOutside )
			{
				bCanRender = true;
			}
		}
	}
	else
	{
		bCanRender = true;
	}

	// If can render, then push the uniform buffers
	if (bCanRender)
	{
		// Set up transformation for the mesh
		if ( m_parent )
			PushCbufferPerObject(m_parent->transform, cameraPass);
		else
			PushCbufferPerObject(this->transform.world, cameraPass);
	}

	// 
	return true;
}

// Render the mesh
bool renderer::Mesh::Render ( const rrRenderParams* params )
{
	//{
	//	if ( !bCanRender || m_mesh == NULL )
	//		return true; // Only render when have a valid mesh and rendering enabled
	//	GL_ACCESS;

	//	// Set up material properties before mesh is bound
	//	if ( bUseSkinning )
	//	{	// Mesh MUST be a rrSkinnedMesh instance, otherwise crashes will result.
	//		m_material->m_bufferSkeletonSize		= ((rrSkinnedMesh*)m_mesh)->skinning_data.bonecount;
	//		m_material->m_bufferMatricesSkinning	= ((rrSkinnedMesh*)m_mesh)->skinning_data.textureBufferData;
	//	}
	//	else
	//	{
	//		m_material->m_bufferSkeletonSize = 0;
	//		m_material->m_bufferMatricesSkinning = 0;
	//	}

	//	// Set up the material now
	//	m_material->bindPass(pass);

	//	// Pass in shader constant now that the pass has been bound
	//	//RrMaterial::current->setShaderConstants( this );
	//	//if ( m_parent ) m_parent->SendShaderUniforms();

	//	// Bind the current mesh
	//	BindVAO( pass, m_mesh->GetVBOverts(), m_mesh->GetVBOfaces() );

	//	// Render the mesh
	//	glDrawElements( GL_TRIANGLES, m_mesh->modeldata->triangleNum*3, GL_UNSIGNED_INT, 0 );
	//}

	{
		if ( !bCanRender || m_mesh == NULL )
			return true; // Only render when have a valid mesh and rendering enabled

		gpu::GraphicsContext* gfx = gpu::getDevice()->getContext();

		gpu::Pipeline* pipeline = GetPipeline( params->pass );
		gfx->setPipeline(pipeline);
		// bind the vertex buffers
		for (int i = 0; i < renderer::shader::kVBufferSlotMaxCount; ++i)
			if (m_mesh->m_bufferEnabled[i])
				gfx->setVertexBuffer(i, &m_mesh->m_buffer[i], 0);
		// bind the vertex buffers for the morpher: TODO
		// bind the index buffer
		gfx->setIndexBuffer(&m_mesh->m_indexBuffer, gpu::kFormatR16UInteger);
		// bind the cbuffers: TODO
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_OBJECT_MATRICES, &m_cbufPerObjectMatrices);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_OBJECT_EXTENDED, &m_cbufPerObjectSurfaces[params->pass]);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_CAMERA_INFORMATION, params->cbuf_perCamera);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_PASS_INFORMATION, params->cbuf_perPass);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_FRAME_INFORMATION, params->cbuf_perFrame);
		if ( m_parent != NULL )
		{
			if ( m_parent->GetBuffers().m_sbufSkinningMajorValid )
				gfx->setShaderSBuffer(gpu::kShaderStageVs, renderer::SBUFFER_SKINNING_MAJOR, &m_parent->GetBuffers().m_sbufSkinningMajor);
		}
		// draw now
		gfx->drawIndexed(m_mesh->m_modeldata->indexNum, 0);
	}

	/*{
		if ( !bCanRender || m_mesh == NULL )
			return true; // Only render when have a valid mesh and rendering enabled

		gpu::GraphicsContext* gfx = gpu::getDevice()->getContext();

		renderer::Material* material = renderer::Material::Begin(gfx, params, this);
		// bind the vertex buffers
		for (int i = 0; i < renderer::kAttributeMaxCount; ++i)
			if (m_mesh->m_bufferEnabled[i])
				gfx->setVertexBuffer(i, &m_mesh->m_buffer[i], 0);
		// bind the index buffer
		gfx->setIndexBuffer(&m_mesh->m_indexBuffer, gpu::kFormatR16UInteger);
		// bind the cbuffers: TODO
		material->SetShaderCBuffers(gfx, params, this);
		if ( m_parent != NULL )
		{
			if ( m_parent->m_xbufSkinningMajor )
				gfx->setShaderXBuffer(gpu::kShaderStageVs, renderer::XBUFFER_SKINNING_MAJOR, &m_parent->m_xbufSkinningMajor);
		}
		// draw now
		gfx->drawIndexed(m_mesh->m_modeldata->indexNum, 0);
	}*/

	// Successful rendering
	return true;
}