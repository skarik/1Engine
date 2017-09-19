
// Includes
#include "CCubeRenderablePrimitive.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

// Constructor
//  sets values to unit cube, generate vertices
CCubeRenderPrim::CCubeRenderablePrimitive ( void )
	: CRenderablePrimitive ()
{
	fWidth = 1.0;
	fDepth = 1.0;
	fHeight = 1.0;

	GenerateVertices();

}
//  sets values to user-defined, then creates vertices
CCubeRenderPrim::CCubeRenderablePrimitive ( Real width, Real depth, Real height )
	: CRenderablePrimitive ()
{
	fWidth = width;
	fDepth = depth;
	fHeight = height;

	GenerateVertices();
}

// GenerateVertices
//  protected function called to generate cube's vertex list
void CCubeRenderPrim::GenerateVertices ( void )
{
	vertexNum = 8;
	if ( vertexData == NULL )
	{
		vertexData = new arModelVertex [8];
	}
	vertexData[0].x = 0;
	vertexData[0].y = 0;
	vertexData[0].z = 0;

	vertexData[1].x = fWidth;
	vertexData[1].y = 0;
	vertexData[1].z = 0;

	vertexData[2].x = fWidth;
	vertexData[2].y = fDepth;
	vertexData[2].z = 0;

	vertexData[3].x = 0;
	vertexData[3].y = fDepth;
	vertexData[3].z = 0;

	vertexData[4].x = 0;
	vertexData[4].y = 0;
	vertexData[4].z = fHeight;

	vertexData[5].x = fWidth;
	vertexData[5].y = 0;
	vertexData[5].z = fHeight;

	vertexData[6].x = fWidth;
	vertexData[6].y = fDepth;
	vertexData[6].z = fHeight;

	vertexData[7].x = 0;
	vertexData[7].y = fDepth;
	vertexData[7].z = fHeight;

	// Makes the object centered on its local origin.
	for ( int i = 0; i < 8; i += 1 )
	{
		vertexData[i].x -= fWidth*0.5f;
		vertexData[i].y -= fDepth*0.5f;
		vertexData[i].z -= fHeight*0.5f;
	}
}

// Set a new size
void CCubeRenderPrim::SetSize ( Real width, Real depth, Real height )
{
	fWidth = width;
	fDepth = depth;
	fHeight = height;

	GenerateVertices();
}

// Render object
bool CCubeRenderPrim::PreRender ( void )
{
	m_material->prepareShaderConstants(this);
	return true;
}

bool CCubeRenderPrim::Render ( const char pass )
{
	GL_ACCESS GLd_ACCESS
	//glLoadIdentity();	
	//glTranslatef( transform.position.x, transform.position.y, transform.position.z );
	//GL.Transform( &transform );

	//GLdefaultMaterial.bind();
	//RrMaterial aMat;
	//aMat.useColors = true;
	//aMat.useLighting = false;
	//aMat.bindPass(pass);
	m_material->bindPass(pass);
	//m_material->setShaderConstants(this);

	auto lPrim = GLd.BeginPrimitive( GL_QUADS, m_material );
		GLd.P_PushTexcoord( 0.5F,0.5F,0.5F );
		// Bottom
		GLd.P_PushColor(1.0f,1.0f,1.0f);
		GLd.P_PushNormal( 0,0,-1.0f );
		GLd.P_PushTangent( 0,-1.0f,0 );
		GLd.P_AddVertex( vertexData[3].x,vertexData[3].y,vertexData[3].z );
		GLd.P_AddVertex( vertexData[2].x,vertexData[2].y,vertexData[2].z );
		GLd.P_AddVertex( vertexData[1].x,vertexData[1].y,vertexData[1].z );
		GLd.P_AddVertex( vertexData[0].x,vertexData[0].y,vertexData[0].z );
		// Top
		//glColor3f(0.0f,1.0f,0.0f);
		GLd.P_PushNormal( 0,0,1.0f );
		GLd.P_PushTangent( 0,1.0f,0 );
		GLd.P_AddVertex( vertexData[4].x,vertexData[4].y,vertexData[4].z );
		GLd.P_AddVertex( vertexData[5].x,vertexData[5].y,vertexData[5].z );
		GLd.P_AddVertex( vertexData[6].x,vertexData[6].y,vertexData[6].z );
		GLd.P_AddVertex( vertexData[7].x,vertexData[7].y,vertexData[7].z );
		// Front
		//glColor3f(0.0f,0.0f,1.0f);
		GLd.P_PushNormal( 0,-1.0f,0 );
		GLd.P_PushTangent( 1.0f,0,0 );
		GLd.P_AddVertex( vertexData[5].x,vertexData[5].y,vertexData[5].z );
		GLd.P_AddVertex( vertexData[4].x,vertexData[4].y,vertexData[4].z );
		GLd.P_AddVertex( vertexData[0].x,vertexData[0].y,vertexData[0].z );
		GLd.P_AddVertex( vertexData[1].x,vertexData[1].y,vertexData[1].z );
		// Back
		//glColor3f(1.0f,1.0f,0.0f);
		GLd.P_PushNormal( 0,1.0f,0 );
		GLd.P_PushTangent( 1.0f,0,0 );
		GLd.P_AddVertex( vertexData[7].x,vertexData[7].y,vertexData[7].z );
		GLd.P_AddVertex( vertexData[6].x,vertexData[6].y,vertexData[6].z );
		GLd.P_AddVertex( vertexData[2].x,vertexData[2].y,vertexData[2].z );
		GLd.P_AddVertex( vertexData[3].x,vertexData[3].y,vertexData[3].z );
		// Left
		//glColor3f(1.0f,0.0f,1.0f);
		GLd.P_PushNormal( -1.0f,0,0 );
		GLd.P_PushTangent( 0,1.0f,0 );
		GLd.P_AddVertex( vertexData[3].x,vertexData[3].y,vertexData[3].z );
		GLd.P_AddVertex( vertexData[0].x,vertexData[0].y,vertexData[0].z );
		GLd.P_AddVertex( vertexData[4].x,vertexData[4].y,vertexData[4].z );
		GLd.P_AddVertex( vertexData[7].x,vertexData[7].y,vertexData[7].z );
		// Right
		//glColor3f(0.0f,1.0f,1.0f);
		GLd.P_PushNormal( 1.0f,0,0 );
		GLd.P_PushTangent( 0,1.0f,0 );
		GLd.P_AddVertex( vertexData[6].x,vertexData[6].y,vertexData[6].z );
		GLd.P_AddVertex( vertexData[5].x,vertexData[5].y,vertexData[5].z );
		GLd.P_AddVertex( vertexData[1].x,vertexData[1].y,vertexData[1].z );
		GLd.P_AddVertex( vertexData[2].x,vertexData[2].y,vertexData[2].z );
	GLd.EndPrimitive(lPrim);

	//aMat.unbind();
	return true;
}