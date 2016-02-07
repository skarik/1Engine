
#include "AreaRenderer.h"

#include "engine2d/entities/Area2DBase.h"

#include "renderer/texture/CTexture.h"
#include "renderer/material/glMaterial.h"
#include "renderer/system/glDrawing.h"

using namespace M04;

AreaRenderer::AreaRenderer ( void )
	: CRenderableObject()
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

AreaRenderer::~AreaRenderer ( void )
{
	; // Nothing
}

bool AreaRenderer::Render ( const char pass )
{
	GLd_ACCESS;

	// Bind wanted pass
	m_material->bindPass(pass);

	// Begin rendering the area rects
	GLd.BeginPrimitive( GL_TRIANGLES );

	GLd.P_PushTexcoord( 0,0 );
	GLd.P_PushNormal( 0,0,0 );

	const Color m_defaultColor	(0.6F,0.6F,0.6F,0.5F);
	const Color m_glowColor		(0.9F,0.9F,0.9F,0.5F);
	const Color m_selectColor	(1.0F,1.0F,1.0F,0.8F);
	Color m_typeColor;

	// Do a render of all areas
	for ( auto area = Engine2D::Area2D::Areas().begin(); area != Engine2D::Area2D::Areas().end(); ++area )
	{
		Rect rect = (*area)->m_rect;
		Vector3d points [4];
		points[0] = rect.pos + Vector3d( 0,0,transform.position.z );
		points[2] = rect.pos + rect.size + Vector3d( 0,0,transform.position.z );
		points[1] = Vector3d( points[2].x, points[0].y, points[0].z );
		points[3] = Vector3d( points[0].x, points[2].y, points[0].z );

		string type = (*area)->GetTypeName();
		if ( type == "Area2DBase" )
			m_typeColor = Color( 0.6F,0.7F,1.0F, 1.0F );
		else if ( type == "AreaTeleport" )
			m_typeColor = Color( 0.9F,1.0F,0.6F, 1.0F );
		else if ( type == "AreaTrigger" )
			m_typeColor = Color( 0.8F,0.4F,0.4F, 1.0F );
		else if ( type == "AreaPlayerSpawn" )
			m_typeColor = Color( 0.5F,1.0F,0.3F, 1.0F );
		else // Error type
			m_typeColor = Color( 1.0F,0.0F,1.0F, 1.0F );

		if ( *area == m_target_selection )
			GLd.P_PushColor( m_selectColor * m_typeColor );
		else if ( *area == m_target_glow )
			GLd.P_PushColor( m_glowColor * m_typeColor );
		else
			GLd.P_PushColor( m_defaultColor * m_typeColor );

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

		// Draw the four quads around the edge of the area
		meshpoints[0] = points[0];
		meshpoints[1] = points[1];
		meshpoints[2] = points[1]+Vector2d(0,4);
		meshpoints[3] = points[0]+Vector2d(0,4);
		PushQuad();

		meshpoints[0] = points[3]-Vector2d(0,4);
		meshpoints[1] = points[2]-Vector2d(0,4);
		meshpoints[2] = points[2];
		meshpoints[3] = points[3];
		PushQuad();

		meshpoints[0] = points[0];
		meshpoints[1] = points[3];
		meshpoints[2] = points[3]+Vector2d(4,0);
		meshpoints[3] = points[0]+Vector2d(4,0);
		PushQuad();

		meshpoints[0] = points[1]-Vector2d(4,0);
		meshpoints[1] = points[2]-Vector2d(4,0);
		meshpoints[2] = points[2];
		meshpoints[3] = points[1];
		PushQuad();

		// Draw the four corners
		const Vector2d offsets [4] = {
			Vector2d(-1,-1), Vector2d(-1,1), Vector2d(1,1), Vector2d(1,-1)
		};
		for ( int i = 0; i < 4; ++i )
		{
			// Build the corner quad
			for ( int n = 0; n < 4; ++n )
			{
				meshpoints[n] = points[i] + offsets[n] * 4.0F;
			}
			// If mouse over a corner, highlight the corner
			if ( (*area == m_target_selection || *area == m_target_glow) && i == m_target_corner ) 
				GLd.P_PushColor( m_selectColor );
			else 
				GLd.P_PushColor( m_defaultColor * m_typeColor );
			// Push the quad
			PushQuad();
		}
	}

	GLd.EndPrimitive();

	return true;
}