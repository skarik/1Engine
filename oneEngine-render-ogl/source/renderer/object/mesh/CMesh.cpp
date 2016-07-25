

#include "CMesh.h"
#include "renderer/logic/model/CModel.h"
#include "renderer/logic/model/CSkinnedModel.h"
#include "renderer/object/mesh/system/glMesh.h"
#include "renderer/object/mesh/system/glSkinnedMesh.h"
#include "renderer/camera/CCamera.h"
#include "renderer/material/glMaterial.h"
#include "renderer/system/glMainSystem.h"

CMesh::CMesh ( glMesh* nMesh, bool n_enableSkinning )
	: CRenderableObject(),
	m_glMesh( nMesh ), bUseSkinning(n_enableSkinning)
{
	SetMaterial( m_glMesh->pmMat );
	CalculateBoundingBox();
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
	if ( !bCanRender )
		return true;

	GL_ACCESS
	/*try
	{*/
		GL.Transform( &(m_parent->transform) );

	//glBindVertexArray( 0 );
		if ( bUseSkinning )
		{
			//if ( ((CSkinnedModel*)m_parent)->bPerfectReference )
			/*if ( false )
			{
				m_material->m_bufferSkeletonSize = ((glSkinnedMesh*)m_glMesh)->GetSkeleton()->size();
				CMesh* zeroMesh = m_parent->GetMesh(uint(0));
				if ( this == zeroMesh ) {
					// use my own skinning and main body
					//m_material->m_bufferMatricesSkinning = ((glSkinnedMesh*)m_glMesh)->getTextureBuffer();
					//m_material->m_bufferMatricesSoftbody = ((glSkinnedMesh*)m_glMesh)->getSoftTextureBuffer();
					m_material->m_bufferMatricesSkinning = ((glSkinnedMesh*)m_glMesh)->getBuffer();
					//m_material->m_bufferMatricesSoftbody = ((glSkinnedMesh*)m_glMesh)->getSoftBuffer();
				}
				else {
					// use zero mesh main body
					//m_material->m_bufferMatricesSkinning = ((glSkinnedMesh*)zeroMesh->m_glMesh)->getTextureBuffer();
					m_material->m_bufferMatricesSkinning = ((glSkinnedMesh*)zeroMesh->m_glMesh)->getBuffer();
					// use my own soft skinning
					//m_material->m_bufferMatricesSoftbody = ((glSkinnedMesh*)m_glMesh)->getSoftTextureBuffer();
					//m_material->m_bufferMatricesSoftbody = ((glSkinnedMesh*)m_glMesh)->getSoftBuffer();
				}
			}
			else
			{*/
				m_material->m_bufferSkeletonSize		= ((glSkinnedMesh*)m_glMesh)->skinning_data.bonecount;
				m_material->m_bufferMatricesSkinning	= ((glSkinnedMesh*)m_glMesh)->skinning_data.textureBufferData;
			//}
		}
		else
		{
			m_material->m_bufferSkeletonSize = 0;
			m_material->m_bufferMatricesSkinning = 0;
			//m_material->m_bufferMatricesSoftbody = 0;
		}
		GL.CheckError();
		m_material->bindPass(pass);
		GL.CheckError();
		m_parent->SendShaderUniforms(this);
		GL.CheckError();
		BindVAO( pass, m_glMesh->GetVBOverts(), m_glMesh->GetVBOfaces() );
		GL.CheckError();
		/*
		GLint arraybuf_0, arraybuf_1;
		glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING,&arraybuf_0);
		//glGetIntegerv(GL_ARRAY_BUFFER_BINDING,&arraybuf_1);
		if ( arraybuf_0 == 0 ) {
			throw std::exception();
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_glMesh->GetVBOfaces() );
		}*/
		glDrawElements( GL_TRIANGLES, m_glMesh->pmData->triangleNum*3, GL_UNSIGNED_INT, 0 );
		GL.CheckError();

		//GLenum error = glGetError();
	/*}
	catch ( const std::exception& e ) {
		printf( e.what() );
	}*/

	return true;
}