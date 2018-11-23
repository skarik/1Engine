#include "RrBillboard.h"
#include "renderer/camera/RrCamera.h"
#include "renderer/gpuw/Device.h"
#include "renderer/gpuw/GraphicsContext.h"

//#include "renderer/system/glMainSystem.h"

//GLuint CBillboard::m_vbo	=0;

rrMeshBuffer RrBillboard::m_MeshBuffer;
void RrBillboard::BuildMeshBuffer ( void )
{
	if (m_MeshBuffer.m_mesh_uploaded == false)
	{
		arModelData model;

		model.position = new Vector3f [4];
		model.normal = new Vector3f [4];
		model.color = new Vector4f [4];
		model.texcoord0 = new Vector3f [4];
		model.vertexNum = 4;
		model.indices = new uint16_t [4];
		model.indexNum = 4;

		model.color[0] = Vector4f(1, 1, 1, 1);
		model.position[0] = Vector3f(0, -1, +1);
		model.texcoord0[0] = Vector3f(0, 0, 0);
		model.normal[0] = Vector3f(1, 0, 0);

		model.color[1] = Vector4f(1, 1, 1, 1);
		model.position[1] = Vector3f(0, -1, -1);
		model.texcoord0[1] = Vector3f(0, 1, 0);
		model.normal[1] = Vector3f(1, 0, 0);

		model.color[3] = Vector4f(1, 1, 1, 1);
		model.position[3] = Vector3f(0, +1, -1);
		model.texcoord0[3] = Vector3f(1, 1, 0);
		model.normal[3] = Vector3f(1, 0, 0);

		model.color[2] = Vector4f(1, 1, 1, 1);
		model.position[2] = Vector3f(0, +1, +1);
		model.texcoord0[2] = Vector3f(1, 0, 0);
		model.normal[2] = Vector3f(1, 0, 0);

		model.indices[0] = 0;
		model.indices[1] = 1;
		model.indices[2] = 2;
		model.indices[3] = 3;

		// Model is created, we upload:
		m_MeshBuffer.InitMeshBuffers(&model);
		m_MeshBuffer.m_modeldata = NULL;

		// Free the CPU model data:
		delete_safe_array(model.position);
		delete_safe_array(model.normal);
		delete_safe_array(model.color);
		delete_safe_array(model.texcoord0);

		delete_safe_array(model.indices);
	}
}


RrBillboard::RrBillboard ( void )
	: CRenderableObject(),
	width(1.0F), pointAtCamera(false)
{
	BuildMeshBuffer();
}
RrBillboard::~RrBillboard ( void )
{
	
}

bool RrBillboard::PreRender ( rrCameraPass* cameraPass )
{
	// Get the active camera and take its transform
	Vector3d lookPos (0,0,0);
	Vector3d vRight (0,1,0), vUp(0,0,1);
	if ( RrCamera::activeCamera )
	{
		if ( pointAtCamera )
			lookPos = (RrCamera::activeCamera->transform.position - transform.world.position).normal();
		else
			lookPos = RrCamera::activeCamera->transform.rotation * Vector3d::forward;
		transform.world.rotation = Vector3d(0,0,0);
		vRight = RrCamera::activeCamera->GetUp().cross( lookPos );
		vUp = lookPos.cross( vRight );
	}
	vRight *= width;
	vUp *= width;
	lookPos *= width;

	// Set transform data
	{
		transform.world.rotation.pData[0] = lookPos.x;
		transform.world.rotation.pData[3] = lookPos.y;
		transform.world.rotation.pData[6] = lookPos.z;

		transform.world.rotation.pData[1] = vRight.x;
		transform.world.rotation.pData[4] = vRight.y;
		transform.world.rotation.pData[7] = vRight.z;

		transform.world.rotation.pData[2] = vUp.x;
		transform.world.rotation.pData[5] = vUp.y;
		transform.world.rotation.pData[8] = vUp.z;

		transform.world.scale = Vector3d( 1,1,1 );
	}

	// Set up transformation for the mesh
	PushCbufferPerObject(this->transform.world, cameraPass);
	return true;
}

bool RrBillboard::Render ( const rrRenderParams* params )
{
	gpu::GraphicsContext* gfx = gpu::getDevice()->getContext();

	gpu::Pipeline* pipeline = GetPipeline( params->pass );
	gfx->setPipeline(pipeline);
	// bind the vertex buffers
	for (int i = 0; i < renderer::kAttributeMaxCount; ++i)
		if (m_MeshBuffer.m_bufferEnabled[i])
			gfx->setVertexBuffer(i, &m_MeshBuffer.m_buffer[i], 0);
	// bind the index buffer
	gfx->setIndexBuffer(&m_MeshBuffer.m_indexBuffer, gpu::kFormatR16UInteger);
	// bind the cbuffers: TODO
	gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_OBJECT_MATRICES, &m_cbufPerObjectMatrices);
	gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_OBJECT_EXTENDED, &m_cbufPerObjectSurface);
	gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_CAMERA_INFORMATION, params->cbuf_perCamera);
	gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_PASS_INFORMATION, params->cbuf_perPass);
	gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_FRAME_INFORMATION, params->cbuf_perFrame);
	// draw now
	gfx->drawIndexed(m_MeshBuffer.m_modeldata->indexNum, 0);

	return true;

	//GL_ACCESS;
	//// Get the active camera and take its transform
	//Vector3d lookPos (0,0,0);
	//Vector3d vRight (0,1,0), vUp(0,0,1);
	//if ( RrCamera::activeCamera )
	//{
	//	if ( m_curvetowards ) {
	//		lookPos = (RrCamera::activeCamera->transform.position - transform.world.position).normal();
	//	}
	//	else {
	//		lookPos = RrCamera::activeCamera->transform.rotation * Vector3d::forward;
	//	}
	//	transform.world.rotation = Vector3d(0,0,0);
	//	vRight = RrCamera::activeCamera->GetUp().cross( lookPos );
	//	vUp = lookPos.cross( vRight );
	//}
	//vRight *= m_size;
	//vUp *= m_size;
	//lookPos *= m_size;

	//// Set transform data
	//{
	//	transform.world.rotation.pData[0] = lookPos.x;
	//	transform.world.rotation.pData[3] = lookPos.y;
	//	transform.world.rotation.pData[6] = lookPos.z;

	//	transform.world.rotation.pData[1] = vRight.x;
	//	transform.world.rotation.pData[4] = vRight.y;
	//	transform.world.rotation.pData[7] = vRight.z;

	//	transform.world.rotation.pData[2] = vUp.x;
	//	transform.world.rotation.pData[5] = vUp.y;
	//	transform.world.rotation.pData[8] = vUp.z;

	//	transform.world.scale = Vector3d( 1,1,1 );
	//}

	//// Genereate the mesh if it doesn't exist
	//if ( m_vbo == 0 )
	//{
	//	glGenBuffers( 1, &m_vbo );
	//	glBindBuffer( GL_ARRAY_BUFFER, m_vbo );

	//	arModelVertex model [4];
	//	model[0].r = 1;		model[0].g = 1;		model[0].b = 1;		model[0].a = 1;
	//	model[0].x = 0;		model[0].y = -1;	model[0].z = +1;
	//	model[0].u = 0;		model[0].v = 0;

	//	model[1].r = 1;		model[1].g = 1;		model[1].b = 1;		model[1].a = 1;
	//	model[1].x = 0;		model[1].y = -1;	model[1].z = -1;
	//	model[1].u = 0;		model[1].v = 1;

	//	model[3].r = 1;		model[3].g = 1;		model[3].b = 1;		model[3].a = 1;
	//	model[3].x = 0;		model[3].y = +1;	model[3].z = -1;
	//	model[3].u = 1;		model[3].v = 1;

	//	model[2].r = 1;		model[2].g = 1;		model[2].b = 1;		model[2].a = 1;
	//	model[2].x = 0;		model[2].y = +1;	model[2].z = +1;
	//	model[2].u = 1;		model[2].v = 0;

	//	glBufferData( GL_ARRAY_BUFFER, sizeof(arModelVertex)*(4), model, GL_STATIC_DRAW );
	//}


	////GL.Transform( &transform.world );

	//m_material->bindPass(pass);
	////m_material->setShaderConstants(this);
	//BindVAO( pass, m_vbo );
	//glDisable( GL_CULL_FACE );
	//glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

	//GL.CheckError();

	/*glBindBuffer( GL_ARRAY_BUFFER, m_vbo );

	vMaterials[pass]->bind();
	vMaterials[pass]->setShaderConstants( this );
	vMaterials[pass]->forwardMeshAttributes();
	glDisable( GL_CULL_FACE );
	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
	glEnable( GL_CULL_FACE );
	
	glBindBuffer( GL_ARRAY_BUFFER, 0 );*/

	return true;
}

// Set the material, clearing the list of materials
/*void CBillboard::SetMaterial ( RrMaterial* pNewMaterial )
{
	ClearMaterialList();
	vMaterials.push_back( pNewMaterial );
}

// Clear up the material list.
void CBillboard::ClearMaterialList ( void )
{
	for ( unsigned int i = 0; i < vMaterials.size(); i += 1 )
	{
		// If the material has no external owner, free it.
		if ( vMaterials[i]->canFree() )
			delete vMaterials[i];
	}
	vMaterials.clear();
}*/