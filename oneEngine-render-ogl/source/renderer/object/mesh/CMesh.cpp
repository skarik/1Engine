

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
	m_glMesh( nMesh ), m_parent(NULL), bUseSkinning(n_enableSkinning)
{
	if ( m_glMesh != NULL )
	{
		SetMaterial( m_glMesh->pmMat );
		CalculateBoundingBox();
	}
}

CMesh::~CMesh ( void )
{

}

void CMesh::CalculateBoundingBox ( void )
{
	Vector3d minPos, maxPos;

	//for ( unsigned int i = 0; i < vMeshes.size(); i++ )
	CModelData* pmData = m_glMesh->pmData;
	for ( unsigned int v = 0; v < pmData->vertexNum; v++ )
	{
		CModelVertex* vert = &(pmData->vertices[v]);
		minPos.x = std::min<ftype>( minPos.x, vert->x );
		minPos.y = std::min<ftype>( minPos.y, vert->y );
		minPos.z = std::min<ftype>( minPos.z, vert->z );
		maxPos.x = std::max<ftype>( maxPos.x, vert->x );
		maxPos.y = std::max<ftype>( maxPos.y, vert->y );
		maxPos.z = std::max<ftype>( maxPos.z, vert->z );
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

bool CMesh::PreRender ( const char pass )
{
	// Culling check routine
	if ( bUseFrustumCulling )
	{
		bCanRender = false;

		if ( !bUseSkinning )
		{
			if ( CCamera::activeCamera->SphereIsVisible( transform.WorldMatrix() * vCheckRenderPos, fCheckRenderDist * (transform.scale.x+transform.scale.y+transform.scale.z) * 0.4f ) )
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
			if ( CCamera::activeCamera->SphereIsVisible( modelOrigin, fCheckRenderDist * (transform.scale.x+transform.scale.y+transform.scale.z) * 0.6f ) ) {
				bCanRender = true;
			}
		}
	}
	else
	{
		bCanRender = true;
	}

	// 
	return true;
}

// Render the mesh
bool CMesh::Render ( const char pass )
{
	if ( !bCanRender || m_glMesh == NULL )
		return true; // Only render when have a valid mesh and rendering enabled
	GL_ACCESS;

	// Set up transformation for the mesh
	if ( m_parent )
		GL.Transform( &m_parent->transform );
	else
		GL.Transform( &this->transform );
	
	// Set up material properties before mesh is bound
	if ( bUseSkinning )
	{	// Mesh MUST be a rrSkinnedMesh instance, otherwise crashes will result.
		m_material->m_bufferSkeletonSize		= ((rrSkinnedMesh*)m_glMesh)->skinning_data.bonecount;
		m_material->m_bufferMatricesSkinning	= ((rrSkinnedMesh*)m_glMesh)->skinning_data.textureBufferData;
	}
	else
	{
		m_material->m_bufferSkeletonSize = 0;
		m_material->m_bufferMatricesSkinning = 0;
	}

	// Set up the material now
	m_material->bindPass(pass);

	// Pass in shader constant now that the pass has been bound
	RrMaterial::current->setShaderConstants( this );
	if ( m_parent ) m_parent->SendShaderUniforms();

	// Bind the current mesh
	BindVAO( pass, m_glMesh->GetVBOverts(), m_glMesh->GetVBOfaces() );

	// Render the mesh
	glDrawElements( GL_TRIANGLES, m_glMesh->pmData->triangleNum*3, GL_UNSIGNED_INT, 0 );


	// Successful rendering
	return true;
}