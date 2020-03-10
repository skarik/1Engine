#include "core/math/Math.h"
#include "core-ext/utils/MeshBuilder.h"
#include "renderer/material/RrShaderProgram.h"
#include "renderer/texture/RrTexture.h"
#include "m04-editor/entities/UILightHandle.h"
#include "UILightHandleRenderer.h"

using namespace M04;

UILightHandleRenderer::UILightHandleRenderer ( UILightHandle* n_dragHandle )
	: CStreamedRenderable3D(), m_ui( n_dragHandle )
{
	memset( &m_modeldata, 0, sizeof(arModelData) );

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

UILightHandleRenderer::~UILightHandleRenderer ( void )
{
	delete_safe_array(m_modeldata.position);
	delete_safe_array(m_modeldata.color);
	delete_safe_array(m_modeldata.texcoord0);
	delete_safe_array(m_modeldata.indices);
}

bool UILightHandleRenderer::EndRender ( void )
{
	const uint kMaxVertices = 32;
	const uint kMaxIndices	= kMaxVertices / 2 * 3; // We're doing 100% quads.

	// Allocate data for the streamed mesh
	if (m_modeldata.indices == NULL)
	{
		delete[] m_modeldata.indices;
		delete[] m_modeldata.position;
		delete[] m_modeldata.color;
		delete[] m_modeldata.texcoord0;

		m_modeldata.indices = new uint16_t [kMaxIndices];
		m_modeldata.position = new Vector3f [kMaxVertices];
		m_modeldata.color = new Vector4f [kMaxVertices];
		m_modeldata.texcoord0 = new Vector3f [kMaxVertices];

		memset(m_modeldata.position, 0, sizeof(Vector3f) * kMaxVertices);
	}
	// Reset count
	m_modeldata.indexNum = 0;
	m_modeldata.vertexNum = 0;

	Vector3f meshpoints [4];
	Color l_currentColor;

	Vector3f ui_position = m_ui->m_position;

	if ( m_ui->m_style == UILightHandle::DrawStyle::s2D )
	{
		// Draw box for range (on X)
		l_currentColor = Color( 0.0F, (m_ui->m_drag_axis==UILightHandle::Axis::Range) ? 1.0F : 0.75F, 0.0F, (m_ui->m_drag_axis_hover==UILightHandle::Axis::Range) ? 1.0F : 0.5F );

		meshpoints[0] = Vector2f( ui_position.x - m_ui->m_range - 4.0F, ui_position.y - 4.0F );
		meshpoints[1] = Vector2f( ui_position.x - m_ui->m_range + 4.0F, ui_position.y - 4.0F );
		meshpoints[2] = Vector2f( ui_position.x - m_ui->m_range + 4.0F, ui_position.y + 4.0F );
		meshpoints[3] = Vector2f( ui_position.x - m_ui->m_range - 4.0F, ui_position.y + 4.0F );
		core::meshbuilder::Quad(&m_modeldata, meshpoints, l_currentColor, Rect());
		meshpoints[0] = Vector2f( ui_position.x + m_ui->m_range - 4.0F, ui_position.y - 4.0F );
		meshpoints[1] = Vector2f( ui_position.x + m_ui->m_range + 4.0F, ui_position.y - 4.0F );
		meshpoints[2] = Vector2f( ui_position.x + m_ui->m_range + 4.0F, ui_position.y + 4.0F );
		meshpoints[3] = Vector2f( ui_position.x + m_ui->m_range - 4.0F, ui_position.y + 4.0F );
		core::meshbuilder::Quad(&m_modeldata, meshpoints, l_currentColor, Rect());

		// Draw box for power (on Y)
		l_currentColor = Color( (m_ui->m_drag_axis==UILightHandle::Axis::Power) ? 1.0F : 0.75F, 0.0F, 0.0F, (m_ui->m_drag_axis_hover==UILightHandle::Axis::Power) ? 1.0F : 0.5F );

		float power_pos = m_ui->m_range / m_ui->m_power;
		meshpoints[0] = Vector2f( ui_position.x - 4.0F, ui_position.y - power_pos - 4.0F );
		meshpoints[1] = Vector2f( ui_position.x + 4.0F, ui_position.y - power_pos - 4.0F );
		meshpoints[2] = Vector2f( ui_position.x + 4.0F, ui_position.y - power_pos + 4.0F );
		meshpoints[3] = Vector2f( ui_position.x - 4.0F, ui_position.y - power_pos + 4.0F );
		core::meshbuilder::Quad(&m_modeldata, meshpoints, l_currentColor, Rect());
		meshpoints[0] = Vector2f( ui_position.x - 4.0F, ui_position.y + power_pos - 4.0F );
		meshpoints[1] = Vector2f( ui_position.x + 4.0F, ui_position.y + power_pos - 4.0F );
		meshpoints[2] = Vector2f( ui_position.x + 4.0F, ui_position.y + power_pos + 4.0F );
		meshpoints[3] = Vector2f( ui_position.x - 4.0F, ui_position.y + power_pos + 4.0F );
		core::meshbuilder::Quad(&m_modeldata, meshpoints, l_currentColor, Rect());
	}

	// Now with the mesh built, push it to the modeldata
	StreamLockModelData();

	// Push at the end
	return CStreamedRenderable3D::EndRender();
}