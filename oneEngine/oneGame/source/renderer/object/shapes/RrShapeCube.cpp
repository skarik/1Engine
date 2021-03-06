#include "RrShapeCube.h"
#include "gpuw/Device.h"
#include "gpuw/GraphicsContext.h"

rrMeshBuffer RrShapeCube::m_MeshBuffer;
void RrShapeCube::BuildMeshBuffer ( void )
{
	if (m_MeshBuffer.m_mesh_uploaded == false)
	{
		const Vector3f kSize ( 1.0F, 1.0F, 1.0F );
		// HS = Halfsize
		const Vector3f kHS ( kSize.x * 0.5F, kSize.y * 0.5F, kSize.z * 0.5F );
		const Vector3f kCorners [8] = {
			{-kHS.x, -kHS.y, -kHS.z}, {+kHS.x, -kHS.y, -kHS.z}, {+kHS.x, +kHS.y, -kHS.z}, {-kHS.x, +kHS.y, -kHS.z},
			{-kHS.x, -kHS.y, +kHS.z}, {+kHS.x, -kHS.y, +kHS.z}, {+kHS.x, +kHS.y, +kHS.z}, {-kHS.x, +kHS.y, +kHS.z}
		};
		const Vector3f kTexCoords [4] = {
			Vector3f(0,0,0), Vector3f(1,0,0), Vector3f(1,1,0), Vector3f(0,1,0)
		};

		arModelData model = {};

		/*m_modeldata.vertices = new arModelVertex [24];
		m_modeldata.vertexNum = 24;
		m_modeldata.triangles = new arModelTriangle [12];
		m_modeldata.triangleNum = 12;

		memset(m_modeldata.vertices, 0, sizeof(arModelVertex) * m_modeldata.vertexNum);*/

		model.position = new Vector3f [24];
		model.normal = new Vector3f [24];
		model.tangent = new Vector3f [24];
		model.color = new Vector4f [24];
		model.texcoord0 = new Vector3f [24];
		model.vertexNum = 24;

		model.indices = new uint16_t [36];
		model.indexNum = 36;

		// Bottom
		model.position[0 + 0] = kCorners[3];
		model.position[0 + 1] = kCorners[2];
		model.position[0 + 2] = kCorners[1];
		model.position[0 + 3] = kCorners[0];
		for (uint i = 0; i < 4; ++i)
		{
			model.normal[0 + i] = Vector3f(0.0F, 0.0F, -1.0F);
			model.tangent[0 + i] = Vector3f(0.0F, -1.0F, 0.0F);
		}
		// Top
		model.position[4 + 0] = kCorners[4];
		model.position[4 + 1] = kCorners[5];
		model.position[4 + 2] = kCorners[6];
		model.position[4 + 3] = kCorners[7];
		for (uint i = 0; i < 4; ++i)
		{
			model.normal[4 + i] = Vector3f(0.0F, 0.0F, 1.0F);
			model.tangent[4 + i] = Vector3f(0.0F, 1.0F, 0.0F);
		}

		// Front
		model.position[8 + 0] = kCorners[5];
		model.position[8 + 1] = kCorners[4];
		model.position[8 + 2] = kCorners[0];
		model.position[8 + 3] = kCorners[1];
		for (uint i = 0; i < 4; ++i)
		{
			model.normal[8 + i] = Vector3f(0.0F, -1.0F, 0.0F);
			model.tangent[8 + i] = Vector3f(1.0F, 0.0F, 0.0F);
		}
		// Back
		model.position[12 + 0] = kCorners[7];
		model.position[12 + 1] = kCorners[6];
		model.position[12 + 2] = kCorners[2];
		model.position[12 + 3] = kCorners[3];
		for (uint i = 0; i < 4; ++i)
		{
			model.normal[12 + i] = Vector3f(0.0F, 1.0F, 0.0F);
			model.tangent[12 + i] = Vector3f(1.0F, 0.0F, 0.0F);
		}

		// Left
		model.position[16 + 0] = kCorners[3];
		model.position[16 + 1] = kCorners[0];
		model.position[16 + 2] = kCorners[4];
		model.position[16 + 3] = kCorners[7];
		for (uint i = 0; i < 4; ++i)
		{
			model.normal[16 + i] = Vector3f(-1.0F, 0.0F, 0.0F);
			model.tangent[16 + i] = Vector3f(0.0F, 1.0F, 0.0F);
		}
		// Right
		model.position[20 + 0] = kCorners[6];
		model.position[20 + 1] = kCorners[5];
		model.position[20 + 2] = kCorners[1];
		model.position[20 + 3] = kCorners[2];
		for (uint i = 0; i < 4; ++i)
		{
			model.normal[20 + i] = Vector3f(1.0F, 0.0F, 0.0F);
			model.tangent[20 + i] = Vector3f(0.0F, 1.0F, 0.0F);
		}

		// Set color & texcoord:
		for (uint i = 0; i < 24; ++i)
		{
			model.color[i] = Vector4f(1.0F, 1.0F, 1.0F, 1.0F);
			model.texcoord0[i] = kTexCoords[i % 4];
		}

		// Set triangles:
		for (uint i = 0; i < 6; ++i)
		{
			/*model.triangles[i*2 + 0].vert[0] = i*4 + 0;
			model.triangles[i*2 + 0].vert[1] = i*4 + 1;
			model.triangles[i*2 + 0].vert[2] = i*4 + 2;
			model.triangles[i*2 + 1].vert[0] = i*4 + 2;
			model.triangles[i*2 + 1].vert[1] = i*4 + 3;
			model.triangles[i*2 + 1].vert[2] = i*4 + 0;*/
			model.indices[i * 6 + 0] = i * 4 + 0;
			model.indices[i * 6 + 1] = i * 4 + 1;
			model.indices[i * 6 + 2] = i * 4 + 2;
			model.indices[i * 6 + 3] = i * 4 + 2;
			model.indices[i * 6 + 4] = i * 4 + 3;
			model.indices[i * 6 + 5] = i * 4 + 0;
		}

		// Model is created, we upload:
		m_MeshBuffer.InitMeshBuffers(&model);
		m_MeshBuffer.m_modeldata = NULL;

		// Free the CPU model data:
		delete_safe_array(model.position);
		delete_safe_array(model.normal);
		delete_safe_array(model.tangent);
		delete_safe_array(model.color);
		delete_safe_array(model.texcoord0);

		delete_safe_array(model.indices);
	}
}

RrShapeCube::RrShapeCube ( void )
	: CRenderableObject ()
{
	BuildMeshBuffer();
	//SetSize(Vector3f(1.0F, 1.0F, 1.0F));
}


bool RrShapeCube::PreRender ( rrCameraPass* cameraPass )
{
	// Set up transformation for the mesh
	PushCbufferPerObject(this->transform.world, cameraPass);
	return true;
}


// Render the mesh
bool RrShapeCube::Render ( const rrRenderParams* params )
{
	gpu::GraphicsContext* gfx = gpu::getDevice()->getContext();

	gpu::Pipeline* pipeline = GetPipeline( params->pass );
	gfx->setPipeline(pipeline);
	// bind the vertex buffers
	for (int i = 0; i < renderer::shader::kVBufferSlotMaxCount; ++i)
		if (m_MeshBuffer.m_bufferEnabled[i])
			gfx->setVertexBuffer(i, &m_MeshBuffer.m_buffer[i], 0);
	// bind the index buffer
	gfx->setIndexBuffer(&m_MeshBuffer.m_indexBuffer, gpu::kIndexFormatUnsigned16);
	// bind the cbuffers: TODO
	gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_OBJECT_MATRICES, &m_cbufPerObjectMatrices);
	gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_OBJECT_EXTENDED, &m_cbufPerObjectSurfaces[params->pass]);
	gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_CAMERA_INFORMATION, params->cbuf_perCamera);
	gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_PASS_INFORMATION, params->cbuf_perPass);
	gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_FRAME_INFORMATION, params->cbuf_perFrame);
	// draw now
	gfx->drawIndexed(m_MeshBuffer.m_modeldata->indexNum, 0, 0);
	
	return true;
}


/*RrShapeCube::RrShapeCube ( Vector3f const& size )
	: CRenderableObject ()
{
	SetSize(Vector3f(1.0F, 1.0F, 1.0F));
}

RrShapeCube::SetSize ( Vector3f const& size )
{
	transform.local.scale = size;
}*/

//void RrShapeCube::buildCube ( Real x, Real y, Real z )
//{
//	const Vector3f kHS ( x * 0.5F, y * 0.5F, z * 0.5F ); // Halfsize
//	const Vector3f kCorners [8] = {
//		{-kHS.x, -kHS.y, -kHS.z}, {+kHS.x, -kHS.y, -kHS.z}, {+kHS.x, +kHS.y, -kHS.z}, {-kHS.x, +kHS.y, -kHS.z},
//		{-kHS.x, -kHS.y, +kHS.z}, {+kHS.x, -kHS.y, +kHS.z}, {+kHS.x, +kHS.y, +kHS.z}, {-kHS.x, +kHS.y, +kHS.z}
//	};
//	const Vector3f kTexCoords [4] = {
//		Vector3f(0,0,0), Vector3f(1,0,0), Vector3f(1,1,0), Vector3f(0,1,0)
//	};
//
//	m_modeldata.vertices = new arModelVertex [24];
//	m_modeldata.vertexNum = 24;
//	m_modeldata.triangles = new arModelTriangle [12];
//	m_modeldata.triangleNum = 12;
//
//	memset(m_modeldata.vertices, 0, sizeof(arModelVertex) * m_modeldata.vertexNum);
//
//	// Bottom
//	m_modeldata.position[0 + 0] = kCorners[3];
//	m_modeldata.position[0 + 1] = kCorners[2];
//	m_modeldata.position[0 + 2] = kCorners[1];
//	m_modeldata.position[0 + 3] = kCorners[0];
//	for (uint i = 0; i < 4; ++i)
//	{
//		m_modeldata.normal[0 + i] = Vector3f(0.0F, 0.0F, -1.0F);
//		m_modeldata.tangent[0 + i] = Vector3f(0.0F, -1.0F, 0.0F);
//	}
//	// Top
//	m_modeldata.position[4 + 0] = kCorners[4];
//	m_modeldata.position[4 + 1] = kCorners[5];
//	m_modeldata.position[4 + 2] = kCorners[6];
//	m_modeldata.position[4 + 3] = kCorners[7];
//	for (uint i = 0; i < 4; ++i)
//	{
//		m_modeldata.normal[4 + i] = Vector3f(0.0F, 0.0F, 1.0F);
//		m_modeldata.tangent[4 + i] = Vector3f(0.0F, 1.0F, 0.0F);
//	}
//
//	// Front
//	m_modeldata.position[8 + 0] = kCorners[5];
//	m_modeldata.position[8 + 1] = kCorners[4];
//	m_modeldata.position[8 + 2] = kCorners[0];
//	m_modeldata.position[8 + 3] = kCorners[1];
//	for (uint i = 0; i < 4; ++i)
//	{
//		m_modeldata.normal[8 + i] = Vector3f(0.0F, -1.0F, 0.0F);
//		m_modeldata.tangent[8 + i] = Vector3f(1.0F, 0.0F, 0.0F);
//	}
//	// Back
//	m_modeldata.position[12 + 0] = kCorners[7];
//	m_modeldata.position[12 + 1] = kCorners[6];
//	m_modeldata.position[12 + 2] = kCorners[2];
//	m_modeldata.position[12 + 3] = kCorners[3];
//	for (uint i = 0; i < 4; ++i)
//	{
//		m_modeldata.normal[12 + i] = Vector3f(0.0F, 1.0F, 0.0F);
//		m_modeldata.tangent[12 + i] = Vector3f(1.0F, 0.0F, 0.0F);
//	}
//
//	// Left
//	m_modeldata.position[16 + 0] = kCorners[3];
//	m_modeldata.position[16 + 1] = kCorners[0];
//	m_modeldata.position[16 + 2] = kCorners[4];
//	m_modeldata.position[16 + 3] = kCorners[7];
//	for (uint i = 0; i < 4; ++i)
//	{
//		m_modeldata.normal[16 + i] = Vector3f(-1.0F, 0.0F, 0.0F);
//		m_modeldata.tangent[16 + i] = Vector3f(0.0F, 1.0F, 0.0F);
//	}
//	// Right
//	m_modeldata.position[20 + 0] = kCorners[6];
//	m_modeldata.position[20 + 1] = kCorners[5];
//	m_modeldata.position[20 + 2] = kCorners[1];
//	m_modeldata.position[20 + 3] = kCorners[2];
//	for (uint i = 0; i < 4; ++i)
//	{
//		m_modeldata.normal[20 + i] = Vector3f(1.0F, 0.0F, 0.0F);
//		m_modeldata.tangent[20 + i] = Vector3f(0.0F, 1.0F, 0.0F);
//	}
//
//	// Set color & texcoord:
//	for (uint i = 0; i < 24; ++i)
//	{
//		m_modeldata.color[i] = Vector4f(1.0F, 1.0F, 1.0F, 1.0F);
//		m_modeldata.texcoord0[i] = kTexCoords[i % 4];
//	}
//
//	// Set triangles:
//	for (uint i = 0; i < 6; ++i)
//	{
//		m_modeldata.triangles[i*2 + 0].vert[0] = i*4 + 0;
//		m_modeldata.triangles[i*2 + 0].vert[1] = i*4 + 1;
//		m_modeldata.triangles[i*2 + 0].vert[2] = i*4 + 2;
//		m_modeldata.triangles[i*2 + 1].vert[0] = i*4 + 2;
//		m_modeldata.triangles[i*2 + 1].vert[1] = i*4 + 3;
//		m_modeldata.triangles[i*2 + 1].vert[2] = i*4 + 0;
//	}
//}
//
//// Set a new size
//void RrShapeCube::setSize ( Real width, Real depth, Real height )
//{
//	buildCube(width, depth, height);
//
//	PushModeldata();
//	
//	delete[] m_modeldata.vertices;
//	delete[] m_modeldata.triangles;
//	m_modeldata.vertices = NULL;
//	m_modeldata.triangles= NULL;
//}