#include "core-ext/utils/MeshBuilder.h"
#include "renderer/material/RrShaderProgram.h"
#include "renderer/material/Material.h"
#include "renderer/texture/RrTexture.h"
#include "engine2d/entities/map/CollisionMap.h"
#include "gpuw/Device.h"
#include "CollisionMapRenderer.h"

using namespace M04;

CollisionMapRenderer::CollisionMapRenderer ( void )
	: CStreamedRenderable3D(), m_drawSolids(true), m_drawWireframe(true)
{
	memset( &m_modeldata, 0, sizeof(arModelData) );
	m_max_vertex = 0;
	m_max_index = 0;

	// Use a default white 2D material
	RrPass spritePass;
	spritePass.utilSetupAsDefault();
	spritePass.m_type = kPassTypeForward;
	spritePass.m_alphaMode = renderer::kAlphaModeTranslucent;
	spritePass.m_cullMode = gpu::kCullModeNone;
	spritePass.m_surface.diffuseColor = Color(1.0F, 1.0F, 1.0F, 1.0F);
	spritePass.setTexture( TEX_DIFFUSE, RrTexture::Load(renderer::kTextureWhite) );
	spritePass.setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/sys/fullbright_vv.spv", "shaders/sys/fullbright_p.spv"}) );
	renderer::shader::Location t_vspec[] = {renderer::shader::Location::kPosition,
											renderer::shader::Location::kUV0,
											renderer::shader::Location::kColor};
	spritePass.setVertexSpecificationByCommonList(t_vspec, 3);
	spritePass.m_primitiveType = gpu::kPrimitiveTopologyTriangleList;
	PassInitWithInput(0, &spritePass);
}

CollisionMapRenderer::~CollisionMapRenderer ( void )
{
	delete_safe_array(m_modeldata.position);
	delete_safe_array(m_modeldata.color);
	delete_safe_array(m_modeldata.texcoord0);
	delete_safe_array(m_modeldata.indices);
}

bool CollisionMapRenderer::EndRender ( void )
{
	// Ensure we're not hitting a null pointer already
	if (m_collision == NULL ||
		m_collision->m_mesh.vertexNum == 0 ||
		m_collision->m_mesh.indexNum == 0)
	{
		m_model_indexcount = 0; // Dont render any mesh.
		return true;
	}

	// Allocate data for the streamed mesh
	if (m_modeldata.indices == NULL ||
		m_max_vertex < m_collision->m_mesh.vertexNum ||
		m_max_index < m_collision->m_mesh.indexNum)
	{
		delete[] m_modeldata.indices;
		delete[] m_modeldata.position;
		delete[] m_modeldata.color;
		delete[] m_modeldata.texcoord0;

		m_max_vertex	= (uint)(m_collision->m_mesh.vertexNum * 1.5F);
		m_max_index		= (uint)(m_collision->m_mesh.indexNum * 1.5F);

		m_modeldata.indices = new uint16_t [m_max_index];
		m_modeldata.position = new Vector3f [m_max_vertex];
		m_modeldata.color = new Vector4f [m_max_vertex];
		m_modeldata.texcoord0 = new Vector3f [m_max_vertex];
	}

	// Copy over the mesh
	m_modeldata.indexNum = m_collision->m_mesh.indexNum;
	memcpy( m_modeldata.indices, m_collision->m_mesh.indices, sizeof(uint16_t) * m_modeldata.indexNum );
	m_modeldata.vertexNum = m_collision->m_mesh.vertexNum;
	for (uint i = 0; i < m_modeldata.vertexNum; ++i)
	{
		m_modeldata.position[i] = m_collision->m_mesh.position[i];
		m_modeldata.position[i].z = -50;
		m_modeldata.texcoord0[i] = Vector2f(0.5F, 0.5F);
		m_modeldata.color[i] = Vector4f(1.0F, 0.0F, 0.2F, 0.5F);
	}

	// Now with the mesh built, push it to the modeldata
	StreamLockModelData();

	// Push at the end
	return CStreamedRenderable3D::EndRender();
}

//		Render()
// Render the model using the 2D engine's style
bool CollisionMapRenderer::Render ( const rrRenderParams* params )
{
	if (m_drawWireframe)
	{
		gpu::GraphicsContext* gfx = params->context_graphics;

		m_postMaterialCb = [](renderer::Material* material)
		{
			gpu::RasterizerState rs;
			rs.cullmode = material->m_pass->m_cullMode;
			rs.fillmode = gpu::kFillModeWireframe;
			material->m_ctx->setRasterizerState(rs);
		};
		CStreamedRenderable3D::Render(params);
		m_postMaterialCb = NULL;
	}
	if (m_drawSolids)
	{
		CStreamedRenderable3D::Render(params);
	}
	return true;
}