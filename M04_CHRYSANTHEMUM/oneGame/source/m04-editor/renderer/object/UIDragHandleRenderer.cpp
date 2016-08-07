
#include "UIDragHandleRenderer.h"
#include "m04-editor/entities/UIDragHandle.h"

#include "core/math/Math.h"
#include "renderer/texture/CTexture.h"
#include "renderer/material/glMaterial.h"
#include "renderer/system/glDrawing.h"

using namespace M04;

UIDragHandleRenderer::UIDragHandleRenderer ( UIDragHandle* n_dragHandle )
	: CRenderableObject(), m_ui( n_dragHandle )
{
	// Set the default white material
	glMaterial* defaultMat = new glMaterial;
	defaultMat->m_diffuse = Color( 1,1,1,1 );
	defaultMat->setTexture( 0, new CTexture( "textures/white.jpg" ) );
	defaultMat->passinfo.push_back( glPass() );
	defaultMat->passinfo[0].shader = new glShader( "shaders/sys/fullbright.glsl" );
	defaultMat->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
	defaultMat->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
	defaultMat->passinfo[0].m_face_mode = Renderer::FM_FRONTANDBACK;
	defaultMat->removeReference();
	SetMaterial( defaultMat );
}

UIDragHandleRenderer::~UIDragHandleRenderer ( void )
{
	; // Nothing
}

bool UIDragHandleRenderer::Render ( const char pass )
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
	if ( m_ui->m_position_snap.x > 0.1F ) ui_position.x = Math.Round( ui_position.x / m_ui->m_position_snap.x ) * m_ui->m_position_snap.x;
	if ( m_ui->m_position_snap.y > 0.1F ) ui_position.y = Math.Round( ui_position.y / m_ui->m_position_snap.y ) * m_ui->m_position_snap.y;
	if ( m_ui->m_position_snap.z > 0.1F ) ui_position.z = Math.Round( ui_position.z / m_ui->m_position_snap.z ) * m_ui->m_position_snap.z;

	if ( m_ui->m_style == UIDragHandle::DrawStyle::s2D )
	{
		// Draw global motion area
		float _all_color = (m_ui->m_drag_axis==UIDragHandle::Axis::All) ? 0.75F : 0.5F;
		GLd.P_PushColor( _all_color, _all_color, _all_color*0.5F, (m_ui->m_drag_axis_hover==UIDragHandle::Axis::All) ? 0.75F : 0.35F );

		meshpoints[0] = Vector2d( ui_position.x - 1.0F, ui_position.y + 1.0F );
		meshpoints[1] = Vector2d( ui_position.x + 16.0F, ui_position.y + 1.0F );
		meshpoints[2] = Vector2d( ui_position.x + 16.0F, ui_position.y - 16.0F );
		meshpoints[3] = Vector2d( ui_position.x - 1.0F, ui_position.y - 16.0F );
		PushQuad();

		// Draw X arrow axis
		GLd.P_PushColor( (m_ui->m_drag_axis==UIDragHandle::Axis::X) ? 1.0F : 0.75F, 0.0F, 0.0F, (m_ui->m_drag_axis_hover==UIDragHandle::Axis::X) ? 1.0F : 0.5F );

		meshpoints[0] = Vector2d( ui_position.x, ui_position.y + 1.0F );
		meshpoints[1] = Vector2d( ui_position.x, ui_position.y - 1.0F );
		meshpoints[2] = Vector2d( ui_position.x + 64.0F, ui_position.y - 1.0F );
		meshpoints[3] = Vector2d( ui_position.x + 64.0F, ui_position.y + 1.0F );
		PushQuad();

		meshpoints[0] = Vector2d( ui_position.x + 64.0F, ui_position.y );
		meshpoints[1] = Vector2d( ui_position.x + 64.0F, ui_position.y + 4.0F );
		meshpoints[2] = Vector2d( ui_position.x + 80.0F, ui_position.y );
		meshpoints[3] = Vector2d( ui_position.x + 64.0F, ui_position.y - 4.0F );
		PushQuad();

		// Draw Y arrow axis
		GLd.P_PushColor( 0.0F, (m_ui->m_drag_axis==UIDragHandle::Axis::Y) ? 0.75F : 0.5F, 0.0F, (m_ui->m_drag_axis_hover==UIDragHandle::Axis::Y) ? 1.0F : 0.5F );

		meshpoints[0] = Vector2d( ui_position.x + 1.0F, ui_position.y );
		meshpoints[1] = Vector2d( ui_position.x - 1.0F, ui_position.y );
		meshpoints[2] = Vector2d( ui_position.x - 1.0F, ui_position.y - 64.0F );
		meshpoints[3] = Vector2d( ui_position.x + 1.0F, ui_position.y - 64.0F );
		PushQuad();

		meshpoints[0] = Vector2d( ui_position.x, ui_position.y - 64.0F );
		meshpoints[1] = Vector2d( ui_position.x + 4.0F, ui_position.y - 64.0F );
		meshpoints[2] = Vector2d( ui_position.x, ui_position.y - 80.0F );
		meshpoints[3] = Vector2d( ui_position.x - 4.0F, ui_position.y - 64.0F );
		PushQuad();
	}

	GLd.EndPrimitive();

	return true;
}