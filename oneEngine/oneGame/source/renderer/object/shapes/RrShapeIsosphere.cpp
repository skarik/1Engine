#include "RrShapeIsosphere.h"
#include "renderer/gpuw/Device.h"
#include "renderer/gpuw/GraphicsContext.h"
#include <math.h>

static const Real kIsoSphereX = .525731112119133606F;
static const Real kIsoSphereZ = .850650808352039932F;
static const Vector3f kIsoSphereVertexSeed[12] = {    
    {-kIsoSphereX, 0.0F, kIsoSphereZ}, {kIsoSphereX, 0.0F, kIsoSphereZ},  {-kIsoSphereX, 0.0F, -kIsoSphereZ}, {kIsoSphereX, 0.0F, -kIsoSphereZ},    
    {0.0F, kIsoSphereZ, kIsoSphereX},  {0.0F, kIsoSphereZ, -kIsoSphereX}, {0.0F, -kIsoSphereZ, kIsoSphereX},  {0.0F, -kIsoSphereZ, -kIsoSphereX},    
    {kIsoSphereZ, kIsoSphereX, 0.0F},  {-kIsoSphereZ, kIsoSphereX, 0.0F}, {kIsoSphereZ, -kIsoSphereX, 0.0F},  {-kIsoSphereZ, -kIsoSphereX, 0.0F} 
};
static const arModelTriangle kIsoSphereTriangles[20] = { 
    {0,4,1},  {0,9,4},  {9,5,4},  {4,5,8},  {4,8,1},    
    {8,10,1}, {8,3,10}, {5,3,8},  {5,2,3},  {2,7,3},    
    {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6}, 
    {6,1,10}, {9,0,11}, {9,11,2}, {9,2,5},  {7,2,11}
};

struct rrSphereBuildInfo
{
	int			iVertexCount;
	int			iIndexCount;
	arModelData* model;
};

static void rrAddTri ( rrSphereBuildInfo* info, Vector3f a, Vector3f b, Vector3f c, int div )
{
	if (div <= 0)
	{
		// Build tex-coord values:
		float ay = atan2(a[2], a[0]) / (float)M_PI;
		float by = atan2(b[2], b[0]) / (float)M_PI;
		float cy = atan2(c[2], c[0]) / (float)M_PI;

		if (ay > 0.8f)
		{
			if (by < 0.2f)
				by += 1.0F;
			if (cy < 0.2f)
				cy += 1.0F;
		}
		if (by > 0.8f)
		{
			if (ay < 0.2f)
				ay += 1.0F;
			if (cy < 0.2f)
				cy += 1.0F;
		}
		if (cy > 0.8f)
		{
			if (ay < 0.2f)
				ay += 1.0F;
			if (by < 0.2f)
				by += 1.0F;
		}

		// Add a triangle:
		info->model->position[info->iVertexCount + 0] = a;
		info->model->position[info->iVertexCount + 1] = b;
		info->model->position[info->iVertexCount + 2] = c;

		info->model->normal[info->iVertexCount + 0] = a;
		info->model->normal[info->iVertexCount + 1] = b;
		info->model->normal[info->iVertexCount + 2] = c;

		info->model->texcoord0[info->iVertexCount + 0] = Vector2f((a[1]+1)*0.5f, ay);
		info->model->texcoord0[info->iVertexCount + 1] = Vector2f((b[1]+1)*0.5f, by);
		info->model->texcoord0[info->iVertexCount + 2] = Vector2f((c[1]+1)*0.5f, cy);

		info->model->indices[info->iIndexCount + 0] = info->iVertexCount + 0;
		info->model->indices[info->iIndexCount + 1] = info->iVertexCount + 1;
		info->model->indices[info->iIndexCount + 2] = info->iVertexCount + 2;

		info->iVertexCount += 3;
		info->iIndexCount += 3;
	}
	else 
	{
		// We need to subdivide further.
		Vector3f ab = (a + b) / 2;
		Vector3f ac = (a + c) / 2;
		Vector3f bc = (b + c) / 2;

		ab.normalize(); ac.normalize(); bc.normalize();

		rrAddTri(info, a, ab, ac, div-1);
		rrAddTri(info, b, bc, ab, div-1);
		rrAddTri(info, c, ac, bc, div-1);
		rrAddTri(info, ab, bc, ac, div-1);
	}  
}

void RrShapeIsosphere::BuildMeshBuffer ( const int divisions )
{
	arModelData model;

	// Each division splits every triangle into 4 new triangles.
	model.vertexNum	= (uint16_t)(20 * 3 * (int)powf(4, (Real)divisions));
	model.indexNum = (uint16_t)(3 * 20 * (int)powf(4, (Real)divisions));

	// Create data arrays
	model.position = new Vector3f [model.vertexNum];
	model.normal = new Vector3f [model.vertexNum];
	model.tangent = new Vector3f [model.vertexNum];
	model.color = new Vector4f [model.vertexNum];
	model.texcoord0 = new Vector3f [model.vertexNum];

	model.indices = new uint16_t [model.indexNum];

	rrSphereBuildInfo info;
	info.model = &model;
	info.iVertexCount = 0;
	info.iIndexCount = 0;

	for (int i = 0; i < 20; i++)
	{
		rrAddTri(&info,
				 kIsoSphereVertexSeed[kIsoSphereTriangles[i].vert[0]],
				 kIsoSphereVertexSeed[kIsoSphereTriangles[i].vert[1]],
				 kIsoSphereVertexSeed[kIsoSphereTriangles[i].vert[2]],
				 divisions);
	}

	// Model is created, we upload:
	m_meshBuffer.InitMeshBuffers(&model);
	m_meshBuffer.m_modeldata = NULL;

	// Free the CPU model data:
	delete_safe_array(model.position);
	delete_safe_array(model.normal);
	delete_safe_array(model.tangent);
	delete_safe_array(model.color);
	delete_safe_array(model.texcoord0);

	delete_safe_array(model.indices);
}

RrShapeIsosphere::RrShapeIsosphere ( const int divisions )
	: CRenderableObject ()
{
	BuildMeshBuffer(divisions);
}

RrShapeIsosphere::~RrShapeIsosphere ( void )
{
	m_meshBuffer.FreeMeshBuffers();
}

bool RrShapeIsosphere::PreRender ( rrCameraPass* cameraPass )
{
	// Set up transformation for the mesh
	PushCbufferPerObject(this->transform.world, cameraPass);
	return true;
}

// Render the mesh
bool RrShapeIsosphere::Render ( const rrRenderParams* params )
{
	gpu::GraphicsContext* gfx = gpu::getDevice()->getContext();

	gpu::Pipeline* pipeline = GetPipeline( params->pass );
	gfx->setPipeline(pipeline);
	// bind the vertex buffers
	for (int i = 0; i < renderer::kAttributeMaxCount; ++i)
		if (m_meshBuffer.m_bufferEnabled[i])
			gfx->setVertexBuffer(i, &m_meshBuffer.m_buffer[i], 0);
	// bind the index buffer
	gfx->setIndexBuffer(&m_meshBuffer.m_indexBuffer, gpu::kFormatR16UInteger);
	// bind the cbuffers: TODO
	gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_OBJECT_MATRICES, &m_cbufPerObjectMatrices);
	gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_OBJECT_EXTENDED, &m_cbufPerObjectSurface);
	gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_CAMERA_INFORMATION, params->cbuf_perCamera);
	gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_PASS_INFORMATION, params->cbuf_perPass);
	gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_FRAME_INFORMATION, params->cbuf_perFrame);
	// draw now
	gfx->drawIndexed(m_meshBuffer.m_modeldata->indexNum, 0);

	return true;
}