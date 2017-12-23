#include "CMesh.h"
#include "renderer/logic/model/CModel.h"
#include "renderer/logic/model/CSkinnedModel.h"
#include "renderer/object/mesh/system/rrMesh.h"
#include "renderer/object/mesh/system/rrSkinnedMesh.h"
#include "renderer/camera/CCamera.h"
#include "renderer/material/RrMaterial.h"
#include "renderer/system/glMainSystem.h"

CMesh::CMesh ( rrMesh* nMesh, bool n_enableSkinning )
	: CRenderableObject(),
	m_mesh( nMesh ), m_parent(NULL),
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

	arModelData* modeldata = m_mesh->modeldata;
	for ( unsigned int v = 0; v < modeldata->vertexNum; v++ )
	{
		arModelVertex* vert = &(modeldata->vertices[v]);
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

bool CMesh::PreRender ( void )
{
	// disable frustum culling:
	bUseFrustumCulling = false;

	// Culling check routine
	if ( bUseFrustumCulling )
	{
		bCanRender = false;

		if ( !bUseSkinning )
		{
			if ( CCamera::activeCamera->SphereIsVisible(
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
			if ( CCamera::activeCamera->SphereIsVisible(
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
			m_material->prepareShaderConstants(m_parent->transform);
		else
			m_material->prepareShaderConstants(this->transform.world);
	}

	// 
	return true;
}

// Render the mesh
bool CMesh::Render ( const char pass )
{
	if ( !bCanRender || m_mesh == NULL )
		return true; // Only render when have a valid mesh and rendering enabled
	GL_ACCESS;

	// Set up transformation for the mesh
	//if ( m_parent )
	//	GL.Transform( &m_parent->transform );
	//else
	//	GL.Transform( &this->transform.world );
	
	// Set up material properties before mesh is bound
	if ( bUseSkinning )
	{	// Mesh MUST be a rrSkinnedMesh instance, otherwise crashes will result.
		m_material->m_bufferSkeletonSize		= ((rrSkinnedMesh*)m_mesh)->skinning_data.bonecount;
		m_material->m_bufferMatricesSkinning	= ((rrSkinnedMesh*)m_mesh)->skinning_data.textureBufferData;
	}
	else
	{
		m_material->m_bufferSkeletonSize = 0;
		m_material->m_bufferMatricesSkinning = 0;
	}

	// Set up the material now
	m_material->bindPass(pass);

	// Pass in shader constant now that the pass has been bound
	//RrMaterial::current->setShaderConstants( this );
	//if ( m_parent ) m_parent->SendShaderUniforms();

	// Bind the current mesh
	BindVAO( pass, m_mesh->GetVBOverts(), m_mesh->GetVBOfaces() );

	// Render the mesh
	glDrawElements( GL_TRIANGLES, m_mesh->modeldata->triangleNum*3, GL_UNSIGNED_INT, 0 );


	// Successful rendering
	return true;
}