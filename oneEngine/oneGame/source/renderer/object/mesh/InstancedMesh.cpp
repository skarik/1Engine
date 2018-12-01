#include "InstancedMesh.h"
#include "renderer/logic/model/RrCModel.h"
//#include "renderer/logic/model/CSkinnedModel.h"
#include "renderer/object/mesh/system/rrMeshBuffer.h"
#include "renderer/object/mesh/system/rrSkinnedMesh.h"
//#include "renderer/camera/RrCamera.h"
//#include "renderer/material/RrMaterial.h"
//#include "renderer/system/glMainSystem.h"
#include "renderer/types/shaders/sbuffers.h"
#include "renderer/gpuw/Device.h"
#include "renderer/gpuw/GraphicsContext.h"

renderer::InstancedMesh::InstancedMesh ( rrMeshBuffer* mesh, bool n_enableSkinning )
	: Mesh(mesh, n_enableSkinning),
	m_instanceCount(0), m_data(NULL), m_dataSize(0)
{
	bUseFrustumCulling = false; // Disable frustum culling

	m_buffer.initAsStructuredBuffer(NULL, sizeof(float) * 2048);
	/*GL_ACCESS
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
	}*/
}
renderer::InstancedMesh::~InstancedMesh ( void )
{
	m_buffer.free(NULL);
	/*GL_ACCESS
	if ( m_tbo_buffer != 0 )
	{
		glDeleteBuffers( 1, &m_tbo_buffer );
		m_tbo_buffer = 0;
	}
	if ( m_tex_buffer != 0 )
	{
		glDeleteTextures( 1, &m_tex_buffer );
		m_tex_buffer = 0;
	}*/
}
/*
void renderer::InstancedMesh::SetInstancePosition ( int instance_id, const Vector3f& position )
{
	data[instance_id].position = position;
}
void renderer::InstancedMesh::SetInstanceRotation ( int instance_id, const Quaternion& rotation )
{
	data[instance_id].rotation = Vector4f( &rotation.x );
}*/

void renderer::InstancedMesh::SetInstanceData ( void* instanceData, const size_t dataSize )
{
	m_data = instanceData;
	m_dataSize = dataSize;
	m_newData = true;
}


void renderer::InstancedMesh::SetInstanceCount ( int instance_count )
{
	/*if ( (signed)data.size() < instance_count )
	{
		data.resize( instance_count );
	}
	data_count = instance_count;*/
	m_instanceCount = instance_count;
	ARCORE_ASSERT_MSG((m_dataSize % m_instanceCount) == 0, "Bad alignment or size of input data.");
}

bool renderer::InstancedMesh::PreRender ( rrCameraPass* cameraPass ) 
{
	// disable frustum culling:
	bUseFrustumCulling = false;

	// update the instance information
	if ( m_newData && m_data != NULL && m_dataSize > 0 )
	{
		// Reallocate space if we don't have enough
		if (m_dataSizeAllocated < m_dataSize)
		{
			m_dataSizeAllocated = m_dataSize;
			m_buffer.free(NULL);
			m_buffer.initAsStructuredBuffer(NULL, m_dataSizeAllocated);
		}

		// Copy the data to the GPU.
		void* p = m_buffer.map(NULL, gpu::kTransferStream); // TODO: Synchronization bits.
		ARCORE_ASSERT(p != NULL);
		if ( p )
		{
			memcpy( p, &m_data, m_dataSize );
			m_buffer.unmap(NULL);
		}
		m_newData = false;
	}

	return renderer::Mesh::PreRender(cameraPass);
}

// Render the mesh
bool renderer::InstancedMesh::Render ( const rrRenderParams* params )
{
	//if ( !bCanRender || m_mesh == NULL || uploaded_data_count <= 0 )
	//	return true; // Only render when have a valid mesh and rendering enabled
	//GL_ACCESS;

	//// Get rid of any transforms
	//transform.world.position = Vector3f::zero;
	//transform.local.position = Vector3f::zero;

	//// Set up transformation for the mesh
	////if ( m_parent )
	////	GL.Transform( &m_parent->transform );
	////else
	////	GL.Transform( &this->transform.world );

	//// Set up material properties before mesh is bound
	//if ( bUseSkinning && false )
	//{	// Mesh MUST be a rrSkinnedMesh instance, otherwise crashes will result.
	//	m_material->m_bufferSkeletonSize		= ((rrSkinnedMesh*)m_mesh)->skinning_data.bonecount;
	//	m_material->m_bufferMatricesSkinning	= ((rrSkinnedMesh*)m_mesh)->skinning_data.textureBufferData;
	//}
	//else
	//{
	//	m_material->m_bufferSkeletonSize = 0;
	//	m_material->m_bufferMatricesSkinning = 0;
	//}
	//m_material->m_tex_instancedinfo = m_tex_buffer;
	//GL.CheckError();
	//
	//// Set up the material now
	//m_material->bindPass(pass);
	//GL.CheckError();
	//// Pass in shader constant now that the pass has been bound
	////RrMaterial::current->setShaderConstants( this );
	////if ( m_parent ) m_parent->SendShaderUniforms();
	////GL.CheckError();

	//// Bind the current mesh
	//BindVAO( pass, m_mesh->GetVBOverts(), m_mesh->GetVBOfaces() );
	//GL.CheckError();

	//// Render the mesh
	////glDrawElements( GL_TRIANGLES, m_glMesh->pmData->triangleNum*3, GL_UNSIGNED_INT, 0 );
	//glDrawElementsInstanced( GL_TRIANGLES, m_mesh->modeldata->triangleNum*3, GL_UNSIGNED_INT, 0, uploaded_data_count );
	//GL.CheckError();

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
		gfx->setIndexBuffer(&m_mesh->m_indexBuffer, gpu::kIndexFormatUnsigned16);
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
		gfx->setShaderSBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_PASS_INFORMATION + 1, &m_buffer);
		// draw now
		gfx->drawIndexedInstanced(m_mesh->m_modeldata->indexNum, m_instanceCount, 0);
	}

	// Successful rendering
	return true;
}
/*
bool renderer::InstancedMesh::EndRender ( void )
{
	if ( data_count > 0 )
	{
		size_t data_size = sizeof(instanceData_t)*data_count;
		//glBindBuffer( GL_TEXTURE_BUFFER, m_tbo_buffer );
		//glBufferData( GL_TEXTURE_BUFFER, data_size, NULL, GL_STREAM_DRAW ); //orphan old data

		//GLvoid* p = glMapBufferRange( GL_TEXTURE_BUFFER, 0, data_size, GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_BUFFER_BIT|GL_MAP_UNSYNCHRONIZED_BIT );
		//GLvoid* p = glMapBufferRange( GL_TEXTURE_BUFFER, 0, data_size, GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_BUFFER_BIT|GL_MAP_UNSYNCHRONIZED_BIT );
		void* p = m_buffer.map(NULL, gpu::kTransferStream); // TODO: Synchronization bits.
		if ( p )
		{
			memcpy( p, &data[0], data_size );
			//glUnmapBuffer( GL_TEXTURE_BUFFER );
			m_buffer.unmap(NULL);
		}
	}
	// Set amount of data count now
	uploaded_data_count = data_count;

	// Return success
	return true;
}*/