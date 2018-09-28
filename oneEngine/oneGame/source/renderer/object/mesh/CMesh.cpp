#include "CMesh.h"
#include "renderer/logic/model/CModel.h"
#include "renderer/logic/model/CSkinnedModel.h"
#include "renderer/object/mesh/system/rrMeshBuffer.h"
#include "renderer/object/mesh/system/rrSkinnedMesh.h"
#include "renderer/camera/RrCamera.h"
//#include "renderer/material/RrMaterial.h"
//#include "renderer/system/glMainSystem.h"
#include "renderer/gpuw/Device.h"
#include "renderer/gpuw/GraphicsContext.h"

CMesh::CMesh ( rrMeshBuffer* nMesh, bool n_enableSkinning )
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

CMesh::~CMesh ( void )
{

}

void CMesh::CalculateBoundingBox ( void )
{
	Vector3d minPos, maxPos;

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
	//bbCheckRenderBox.Set( transform.GetTransformMatrix(), Vector3d( -0.1f,-0.1f,-0.1f ), Vector3d( 0.1f,0.1f,0.1f ) );
}

// == RENDERABLE OBJECT INTERFACE ==

bool CMesh::PreRender ( rrCameraPass* cameraPass )
{
	// disable frustum culling:
	bUseFrustumCulling = false;

	// Culling check routine
	if ( bUseFrustumCulling )
	{
		bCanRender = false;

		if ( !bUseSkinning )
		{
			if ( camera->SphereIsVisible(
				transform.WorldMatrix() * vCheckRenderPos,
				fCheckRenderDist * (transform.world.scale.x+transform.world.scale.y+transform.world.scale.z) * 0.4f ) )
			{
				bCanRender = true;
			}
		}
		else
		{
			Vector3d modelOrigin;
			//modelOrigin = ((CSkinnedModel*)m_parent)->GetSkeletonList()->at(1)->transform.WorldMatrix() * vCheckRenderPos;
			modelOrigin = ((CSkinnedModel*)m_parent)->GetSkeleton()->current_transform[1].WorldMatrix() * vCheckRenderPos;
			modelOrigin = transform.WorldMatrix() * modelOrigin;
			//modelOrigin += transform.GetTransformMatrix() * vCheckRenderPos
			if ( camera->SphereIsVisible(
				modelOrigin, 
				fCheckRenderDist * (transform.world.scale.x+transform.world.scale.y+transform.world.scale.z) * 0.6f ) )
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
			PushCbufferPerObject(m_parent->transform, camera);
		else
			PushCbufferPerObject(this->transform.world, camera);
	}

	// 
	return true;
}

// Render the mesh
bool CMesh::Render ( const char pass )
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

		gpu::Pipeline* pipeline = GetPipeline( pass );
		gfx->setPipeline(pipeline);
		// bind the vertex buffers
		for (int i = 0; i < renderer::kAttributeMaxCount; ++i)
			if (m_mesh->m_bufferEnabled[i])
				gfx->setVertexBuffer(i, &m_mesh->m_buffer[i], 0);
		// bind the index buffer
		gfx->setIndexBuffer(&m_mesh->m_indexBuffer, gpu::kFormatR16UInteger);
		// bind the cbuffers: TODO
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_OBJECT_MATRICES, &m_cbufPerObjectMatrices);
		if ( m_parent != NULL )
		{
			if ( m_parent->m_xbufSkinningMajor )
				gfx->setShaderXBuffer(gpu::kShaderStageVs, renderer::XBUFFER_SKINNING_MAJOR, &m_parent->m_xbufSkinningMajor);
		}
		// draw now
		gfx->drawIndexed(m_mesh->m_modeldata->indexNum, 0);
	}

	// Successful rendering
	return true;
}