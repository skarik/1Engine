
#include "UILightHandleRenderer.h"
#include "m04-editor/entities/UILightHandle.h"

#include "core/math/Math.h"
#include "renderer/texture/CTexture.h"
#include "renderer/material/RrMaterial.h"
#include "renderer/system/glDrawing.h"

using namespace M04;

UILightHandleRenderer::UILightHandleRenderer ( UILightHandle* n_dragHandle )
	: CRenderableObject(), m_ui( n_dragHandle )
{
	// Set the default white material
	RrMaterial* defaultMat = new RrMaterial;
	defaultMat->m_diffuse = Color( 1,1,1,1 );
	defaultMat->setTexture( TEX_MAIN, new CTexture( "textures/white.jpg" ) );
	defaultMat->passinfo.push_back( RrPassForward() );
	defaultMat->passinfo[0].shader = new RrShader( "shaders/sys/fullbright.glsl" );
	defaultMat->passinfo[0].m_lighting_mode = renderer::LI_NONE;
	defaultMat->passinfo[0].m_transparency_mode = renderer::ALPHAMODE_TRANSLUCENT;
	defaultMat->passinfo[0].m_face_mode = renderer::FM_FRONTANDBACK;
	defaultMat->removeReference();
	SetMaterial( defaultMat );
}

UILightHandleRenderer::~UILightHandleRenderer ( void )
{
	; // Nothing
}

bool UILightHandleRenderer::Render ( const char pass )
{
	GLd_ACCESS;

	// Bind wanted pass
	m_material->bindPass(pass);

	// Begin rendering the area rects
	GLd.BeginPrimitive( GL_TRIANGLES );

	GLd.P_PushTexcoord( 0,0 );
	GLd.P_PushNormal( 0,0,0 );

	// Delare the lambda to streamline the mesh creation
	Vector3d meshpoints [4];
	auto PushQuad = [&]() -> void
	{
		GLd.P_AddVertex( meshpoints[0] );
		GLd.P_AddVertex( meshpoints[1] );
		GLd.P_AddVertex( meshpoints[2] );
		GLd.P_AddVertex( meshpoints[0] );
		GLd.P_AddVertex( meshpoints[2] );
		GLd.P_AddVertex( meshpoints[3] );
	};

	Vector3d ui_position = m_ui->m_position;

	if ( m_ui->m_style == UILightHandle::DrawStyle::s2D )
	{
		// Draw box for range (on X)
		GLd.P_PushColor( 0.0F, (m_ui->m_drag_axis==UILightHandle::Axis::Range) ? 1.0F : 0.75F, 0.0F, (m_ui->m_drag_axis_hover==UILightHandle::Axis::Range) ? 1.0F : 0.5F );

		meshpoints[0] = Vector2d( ui_position.x - m_ui->m_range - 4.0F, ui_position.y - 4.0F );
		meshpoints[1] = Vector2d( ui_position.x - m_ui->m_range + 4.0F, ui_position.y - 4.0F );
		meshpoints[2] = Vector2d( ui_position.x - m_ui->m_range + 4.0F, ui_position.y + 4.0F );
		meshpoints[3] = Vector2d( ui_position.x - m_ui->m_range - 4.0F, ui_position.y + 4.0F );
		PushQuad();
		meshpoints[0] = Vector2d( ui_position.x + m_ui->m_range - 4.0F, ui_position.y - 4.0F );
		meshpoints[1] = Vector2d( ui_position.x + m_ui->m_range + 4.0F, ui_position.y - 4.0F );
		meshpoints[2] = Vector2d( ui_position.x + m_ui->m_range + 4.0F, ui_position.y + 4.0F );
		meshpoints[3] = Vector2d( ui_position.x + m_ui->m_range - 4.0F, ui_position.y + 4.0F );
		PushQuad();

		// Draw box for power (on Y)
		GLd.P_PushColor( (m_ui->m_drag_axis==UILightHandle::Axis::Power) ? 1.0F : 0.75F, 0.0F, 0.0F, (m_ui->m_drag_axis_hover==UILightHandle::Axis::Power) ? 1.0F : 0.5F );

		float power_pos = m_ui->m_range / m_ui->m_power;
		meshpoints[0] = Vector2d( ui_position.x - 4.0F, ui_position.y - power_pos - 4.0F );
		meshpoints[1] = Vector2d( ui_position.x + 4.0F, ui_position.y - power_pos - 4.0F );
		meshpoints[2] = Vector2d( ui_position.x + 4.0F, ui_position.y - power_pos + 4.0F );
		meshpoints[3] = Vector2d( ui_position.x - 4.0F, ui_position.y - power_pos + 4.0F );
		PushQuad();
		meshpoints[0] = Vector2d( ui_position.x - 4.0F, ui_position.y + power_pos - 4.0F );
		meshpoints[1] = Vector2d( ui_position.x + 4.0F, ui_position.y + power_pos - 4.0F );
		meshpoints[2] = Vector2d( ui_position.x + 4.0F, ui_position.y + power_pos + 4.0F );
		meshpoints[3] = Vector2d( ui_position.x - 4.0F, ui_position.y + power_pos + 4.0F );
		PushQuad();
	}

	GLd.EndPrimitive();

	return true;
}