
#include "CInstancedMesh.h"
#include "renderer/logic/model/CModel.h"
#include "renderer/logic/model/CSkinnedModel.h"
#include "renderer/object/mesh/system/rrMesh.h"
#include "renderer/object/mesh/system/rrSkinnedMesh.h"
#include "renderer/camera/CCamera.h"
#include "renderer/material/RrMaterial.h"
#include "renderer/system/glMainSystem.h"

CInstancedMesh::CInstancedMesh ( rrMesh* mesh )
	: CMesh(mesh, false),
	data_count(0), uploaded_data_count(0)
{
	bUseFrustumCulling = false; // Disable frustum culling

	GL_ACCESS
	{
		glGenBuffers( 1, &m_tbo_buffer );
		glBindBuffer( GL_TEXTURE_BUFFER, m_tbo_buffer );
		glBufferData( GL_TEXTURE_BUFFER, sizeof(float) * 2048, NULL, GL_STREAM_DRAW );
	}
	{
		glGenTextures( 1, &m_tex_buffer );
		glBindTexture( GL_TEXTURE_BUFFER, m_tex_buffer );
		glTexBuffer( GL_TEXTURE_BUFFER, GL_RGBA32F, m_tbo_buffer ); 
		glBindTexture( GL_TEXTURE_BUFFER, 0 );
	}
}
CInstancedMesh::~CInstancedMesh ( void )
{
	GL_ACCESS
	if ( m_tbo_buffer != 0 )
	{
		glDeleteBuffers( 1, &m_tbo_buffer );
		m_tbo_buffer = 0;
	}
	if ( m_tex_buffer != 0 )
	{
		glDeleteTextures( 1, &m_tex_buffer );
		m_tex_buffer = 0;
	}
}

void CInstancedMesh::SetInstancePosition ( int instance_id, const Vector3d& position )
{
	data[instance_id].position = position;
}
void CInstancedMesh::SetInstanceRotation ( int instance_id, const Quaternion& rotation )
{
	data[instance_id].rotation = Vector4d( &rotation.x );
}

void CInstancedMesh::SetInstanceCount ( int instance_count )
{
	if ( (signed)data.size() < instance_count )
	{
		data.resize( instance_count );
	}
	data_count = instance_count;
}

// Render the mesh
bool CInstancedMesh::Render ( const char pass )
{
	if ( !bCanRender || m_glMesh == NULL || uploaded_data_count <= 0 )
		return true; // Only render when have a valid mesh and rendering enabled
	GL_ACCESS;

	// Get rid of any transforms
	transform.world.position = Vector3d::zero;
	transform.local.position = Vector3d::zero;
	//transform.LateUpdate();

	// Set up transformation for the mesh
	if ( m_parent )
		GL.Transform( &m_parent->transform );
	else
		GL.Transform( &this->transform.world );

	// Set up material properties before mesh is bound
	if ( bUseSkinning && false )
	{	// Mesh MUST be a rrSkinnedMesh instance, otherwise crashes will result.
		m_material->m_bufferSkeletonSize		= ((rrSkinnedMesh*)m_glMesh)->skinning_data.bonecount;
		m_material->m_bufferMatricesSkinning	= ((rrSkinnedMesh*)m_glMesh)->skinning_data.textureBufferData;
	}
	else
	{
		m_material->m_bufferSkeletonSize = 0;
		m_material->m_bufferMatricesSkinning = 0;
	}
	m_material->m_tex_instancedinfo = m_tex_buffer;
	GL.CheckError();
	
	// Set up the material now
	m_material->bindPass(pass);
	GL.CheckError();
	// Pass in shader constant now that the pass has been bound
	RrMaterial::current->setShaderConstants( this );
	if ( m_parent ) m_parent->SendShaderUniforms();
	GL.CheckError();

	// Bind the current mesh
	BindVAO( pass, m_glMesh->GetVBOverts(), m_glMesh->GetVBOfaces() );
	GL.CheckError();

	// Render the mesh
	//glDrawElements( GL_TRIANGLES, m_glMesh->pmData->triangleNum*3, GL_UNSIGNED_INT, 0 );
	glDrawElementsInstanced( GL_TRIANGLES, m_glMesh->pmData->triangleNum*3, GL_UNSIGNED_INT, 0, uploaded_data_count );
	GL.CheckError();

	// Successful rendering
	return true;
}

bool CInstancedMesh::EndRender ( void )
{
	if ( data_count > 0 )
	{
		size_t data_size = sizeof(instanceData_t)*data_count;
		glBindBuffer( GL_TEXTURE_BUFFER, m_tbo_buffer );
		glBufferData( GL_TEXTURE_BUFFER, data_size, NULL, GL_STREAM_DRAW ); //orphan old data

		GLvoid* p = glMapBufferRange( GL_TEXTURE_BUFFER, 0, data_size, GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_BUFFER_BIT|GL_MAP_UNSYNCHRONIZED_BIT );
		if ( p )
		{
			memcpy( p, &data[0], data_size );
			glUnmapBuffer( GL_TEXTURE_BUFFER );
		}
	}
	// Set amount of data count now
	uploaded_data_count = data_count;

	// Return success
	return true;
}