
#include "CBlockCursor.h"

#include "renderer/material/glMaterial.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

CBlockCursor::CBlockCursor ( void )
	: CRenderableObject()
{
	//renderType = Foreground;
	transform.scale = Vector3d( 1.04f,1.04f,1.04f );
	m_size = RangeVector( 1,1,1 );
	// TODO: Generate VBO
}

void CBlockCursor::SetSize ( const RangeVector& n_size )
{
	m_size = n_size;
	// Regenerate VBO?
}

bool CBlockCursor::Render ( const char pass )
{	GL_ACCESS GLd_ACCESS;

	glMaterial::Default->bindPass(0);

	GL.Transform( &transform );

	//glDisable( GL_LIGHTING );

	GLd.BeginPrimitive( GL_LINES );
		GLd.P_PushColor( 0.8f,0.8f,0.8f,0.7f );

		GLd.P_AddVertex( -1,-1,-1 );	GLd.P_AddVertex( 1,-1,-1 );
		GLd.P_AddVertex( 1,-1,-1 );	GLd.P_AddVertex( 1,1,-1 );
		GLd.P_AddVertex( 1,1,-1 );	GLd.P_AddVertex( -1,1,-1 );
		GLd.P_AddVertex( -1,1,-1 );	GLd.P_AddVertex( -1,-1,-1 );

		GLd.P_AddVertex( -1,-1,1 );	GLd.P_AddVertex( 1,-1,1 );
		GLd.P_AddVertex( 1,-1,1 );	GLd.P_AddVertex( 1,1,1 );
		GLd.P_AddVertex( 1,1,1 );	GLd.P_AddVertex( -1,1,1 );
		GLd.P_AddVertex( -1,1,1 );	GLd.P_AddVertex( -1,-1,1 );

		GLd.P_AddVertex( -1,-1,-1 );	GLd.P_AddVertex( -1,-1,1 );
		GLd.P_AddVertex( 1,-1,-1 );	GLd.P_AddVertex( 1,-1,1 );
		GLd.P_AddVertex( 1,1,-1 );	GLd.P_AddVertex( 1,1,1 );
		GLd.P_AddVertex( -1,1,-1 );	GLd.P_AddVertex( -1,1,1 );
	GLd.EndPrimitive();

	return true;
}