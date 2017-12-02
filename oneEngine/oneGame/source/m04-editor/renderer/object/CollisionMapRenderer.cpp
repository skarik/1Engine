#include "CollisionMapRenderer.h"

#include "engine2d/entities/map/CollisionMap.h"

#include "renderer/texture/CTexture.h"
#include "renderer/material/RrMaterial.h"
#include "renderer/system/glDrawing.h"

#include "core-ext/utils/MeshBuilder.h"

#include "renderer/gpuw/Device.h"

using namespace M04;

CollisionMapRenderer::CollisionMapRenderer ( void )
	: CStreamedRenderable3D()
{
	memset( &m_modeldata, 0, sizeof(arModelData) );
	m_max_vertex = 0;
	m_max_triangle = 0;

	// Set the default white material
	RrMaterial* defaultMat = new RrMaterial;
	defaultMat->m_diffuse = Color( 1,1,1,1 );
	defaultMat->setTexture( TEX_MAIN, new CTexture( "textures/white.jpg" ) );
	defaultMat->passinfo.push_back( RrPassForward() );
	defaultMat->passinfo[0].shader = new RrShader( "shaders/sys/fullbright.glsl" );
	defaultMat->passinfo[0].m_lighting_mode = renderer::LI_NONE;
	defaultMat->passinfo[0].m_transparency_mode = renderer::ALPHAMODE_TRANSLUCENT;
	defaultMat->passinfo[0].m_face_mode = renderer::FM_FRONTANDBACK;
	SetMaterial( defaultMat );
	defaultMat->removeReference();
}

CollisionMapRenderer::~CollisionMapRenderer ( void )
{
	delete_safe_array(m_modeldata.vertices);
	delete_safe_array(m_modeldata.triangles);
}

bool CollisionMapRenderer::PreRender ( void )
{
	// Ensure we're not hitting a null pointer already
	if (m_collision == NULL ||
		m_collision->m_mesh.vertexNum == 0 ||
		m_collision->m_mesh.triangleNum == 0)
	{
		return true;
	}

	// Allocate data for the streamed mesh
	if (m_modeldata.triangles == NULL ||
		m_max_vertex < m_collision->m_mesh.vertexNum ||
		m_max_triangle < m_collision->m_mesh.triangleNum)
	{
		delete[] m_modeldata.triangles;
		delete[] m_modeldata.vertices;

		m_max_vertex	= (uint)(m_collision->m_mesh.vertexNum * 1.5F);
		m_max_triangle	= (uint)(m_collision->m_mesh.triangleNum * 1.5F);

		m_modeldata.triangles = new arModelTriangle [m_max_triangle];
		m_modeldata.vertices  = new arModelVertex [m_max_vertex];
	}

	// Copy over the mesh
	m_modeldata.triangleNum = m_collision->m_mesh.triangleNum;
	memcpy( m_modeldata.triangles, m_collision->m_mesh.triangles, sizeof(arModelTriangle) * m_modeldata.triangleNum );
	m_modeldata.vertexNum = m_collision->m_mesh.vertexNum;
	for (uint i = 0; i < m_modeldata.vertexNum; ++i)
	{
		m_modeldata.vertices[i].position = m_collision->m_mesh.vertices[i].position;
		m_modeldata.vertices[i].position.z = -50;
		m_modeldata.vertices[i].texcoord0 = Vector2f(0.5F, 0.5F);
		m_modeldata.vertices[i].color = Vector4f(1.0F, 0.0F, 0.2F, 0.5F);
	}

	// Now with the mesh built, push it to the modeldata
	StreamLockModelData();

	// Push at the end
	return CStreamedRenderable3D::PreRender();
}

//		Render()
// Render the model using the 2D engine's style
bool CollisionMapRenderer::Render ( const char pass )
{
	gpu::DeviceSetFillMode( NULL, gpu::kFillModeWireframe );
	CStreamedRenderable3D::Render(pass);
	gpu::DeviceSetFillMode( NULL, gpu::kFillModeSolid );
	return CStreamedRenderable3D::Render(pass);
}