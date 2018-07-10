#include "UILightHandleRenderer.h"
#include "m04-editor/entities/UILightHandle.h"

#include "core/math/Math.h"
#include "renderer/texture/RrTexture.h"
#include "renderer/material/RrMaterial.h"
#include "renderer/system/glDrawing.h"

#include "core-ext/utils/MeshBuilder.h"

using namespace M04;

UILightHandleRenderer::UILightHandleRenderer ( UILightHandle* n_dragHandle )
	: CStreamedRenderable3D(), m_ui( n_dragHandle )
{
	memset( &m_modeldata, 0, sizeof(arModelData) );

	// Set the default white material
	RrMaterial* defaultMat = new RrMaterial;
	defaultMat->m_diffuse = Color( 1,1,1,1 );
	defaultMat->setTexture( TEX_MAIN, new RrTexture( "textures/white.jpg" ) );
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
	delete_safe_array(m_modeldata.vertices);
	delete_safe_array(m_modeldata.triangles);
}

bool UILightHandleRenderer::PreRender ( void )
{
	const uint kMaxVertices = 32;
	const uint kMaxTris		= kMaxVertices / 2; // We're doing 100% quads.

	// Allocate data for the streamed mesh
	if (m_modeldata.triangles == NULL)
	{
		delete[] m_modeldata.triangles;
		delete[] m_modeldata.vertices;

		m_modeldata.triangles = new arModelTriangle [kMaxTris];
		m_modeldata.vertices = new arModelVertex [kMaxVertices];

		memset(m_modeldata.vertices, 0, sizeof(arModelVertex) * kMaxVertices);
	}
	// Reset count
	m_modeldata.triangleNum = 0;
	m_modeldata.vertexNum = 0;

	Vector3d meshpoints [4];
	Color l_currentColor;

	Vector3d ui_position = m_ui->m_position;

	if ( m_ui->m_style == UILightHandle::DrawStyle::s2D )
	{
		// Draw box for range (on X)
		l_currentColor = Color( 0.0F, (m_ui->m_drag_axis==UILightHandle::Axis::Range) ? 1.0F : 0.75F, 0.0F, (m_ui->m_drag_axis_hover==UILightHandle::Axis::Range) ? 1.0F : 0.5F );

		meshpoints[0] = Vector2d( ui_position.x - m_ui->m_range - 4.0F, ui_position.y - 4.0F );
		meshpoints[1] = Vector2d( ui_position.x - m_ui->m_range + 4.0F, ui_position.y - 4.0F );
		meshpoints[2] = Vector2d( ui_position.x - m_ui->m_range + 4.0F, ui_position.y + 4.0F );
		meshpoints[3] = Vector2d( ui_position.x - m_ui->m_range - 4.0F, ui_position.y + 4.0F );
		core::meshbuilder::Quad(&m_modeldata, meshpoints, l_currentColor, Rect());
		meshpoints[0] = Vector2d( ui_position.x + m_ui->m_range - 4.0F, ui_position.y - 4.0F );
		meshpoints[1] = Vector2d( ui_position.x + m_ui->m_range + 4.0F, ui_position.y - 4.0F );
		meshpoints[2] = Vector2d( ui_position.x + m_ui->m_range + 4.0F, ui_position.y + 4.0F );
		meshpoints[3] = Vector2d( ui_position.x + m_ui->m_range - 4.0F, ui_position.y + 4.0F );
		core::meshbuilder::Quad(&m_modeldata, meshpoints, l_currentColor, Rect());

		// Draw box for power (on Y)
		l_currentColor = Color( (m_ui->m_drag_axis==UILightHandle::Axis::Power) ? 1.0F : 0.75F, 0.0F, 0.0F, (m_ui->m_drag_axis_hover==UILightHandle::Axis::Power) ? 1.0F : 0.5F );

		float power_pos = m_ui->m_range / m_ui->m_power;
		meshpoints[0] = Vector2d( ui_position.x - 4.0F, ui_position.y - power_pos - 4.0F );
		meshpoints[1] = Vector2d( ui_position.x + 4.0F, ui_position.y - power_pos - 4.0F );
		meshpoints[2] = Vector2d( ui_position.x + 4.0F, ui_position.y - power_pos + 4.0F );
		meshpoints[3] = Vector2d( ui_position.x - 4.0F, ui_position.y - power_pos + 4.0F );
		core::meshbuilder::Quad(&m_modeldata, meshpoints, l_currentColor, Rect());
		meshpoints[0] = Vector2d( ui_position.x - 4.0F, ui_position.y + power_pos - 4.0F );
		meshpoints[1] = Vector2d( ui_position.x + 4.0F, ui_position.y + power_pos - 4.0F );
		meshpoints[2] = Vector2d( ui_position.x + 4.0F, ui_position.y + power_pos + 4.0F );
		meshpoints[3] = Vector2d( ui_position.x - 4.0F, ui_position.y + power_pos + 4.0F );
		core::meshbuilder::Quad(&m_modeldata, meshpoints, l_currentColor, Rect());
	}

	// Now with the mesh built, push it to the modeldata
	StreamLockModelData();

	// Push at the end
	return CStreamedRenderable3D::PreRender();
}