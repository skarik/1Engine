
// Includes
#include "core/time/time.h"
#include "CRotBoxRPrimitive.h"

#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

// Constructor
//  sets values to unit cube, generate vertices
CRotBoxRPrimitive::CRotBoxRPrimitive ( void )
	: CRenderablePrimitive ()
{
	fWidth = 1.0;
	fDepth = 1.0;
	fHeight = 1.0;

	GenerateVertices();

}
//  sets values to user-defined, then creates vertices
CRotBoxRPrimitive::CRotBoxRPrimitive ( Real width, Real depth, Real height )
	: CRenderablePrimitive ()
{
	fWidth = width;
	fDepth = depth;
	fHeight = height;

	GenerateVertices();

}

// GenerateVertices
//  protected function called to generate cube's vertex list
void CRotBoxRPrimitive::GenerateVertices ( void )
{
	fRot = 0;
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
}

// Render object
bool CRotBoxRPrimitive::Render ( const char pass )
{
	GL_ACCESS GLd_ACCESS
		/*
	glTranslatef(-1.5f,0.0f,-6.0f);
	//fRot += 10.0f * Time::deltaTime;
	fRot += 0.3f * Time::deltaTime;
	glRotatef( (float) fabs(sin(fRot))*60.0f, 0,1,0 );
	glRotatef( 80, 1,0,0 );
	*/
	//if ( fRot > 30.0 )
	//	fRot -= 30.0;
	if ( fRot > 3.1415*2.0 )
		fRot = 0;
	
	auto lPrim = GLd.BeginPrimitive( GL_QUADS, m_material );
		// Bottom
		GLd.P_PushColor(1.0f,0.0f,0.0f);
		GLd.P_AddVertex( vertexData[0].x,vertexData[0].y,vertexData[0].z );
		GLd.P_AddVertex( vertexData[1].x,vertexData[1].y,vertexData[1].z );
		GLd.P_AddVertex( vertexData[2].x,vertexData[2].y,vertexData[2].z );
		GLd.P_AddVertex( vertexData[3].x,vertexData[3].y,vertexData[3].z );
		// Top
		GLd.P_PushColor(0.0f,1.0f,0.0f);
		GLd.P_AddVertex( vertexData[4].x,vertexData[4].y,vertexData[4].z );
		GLd.P_AddVertex( vertexData[5].x,vertexData[5].y,vertexData[5].z );
		GLd.P_AddVertex( vertexData[6].x,vertexData[6].y,vertexData[6].z );
		GLd.P_AddVertex( vertexData[7].x,vertexData[7].y,vertexData[7].z );
		// Front
		GLd.P_PushColor(0.0f,0.0f,1.0f);
		GLd.P_AddVertex( vertexData[1].x,vertexData[1].y,vertexData[1].z );
		GLd.P_AddVertex( vertexData[0].x,vertexData[0].y,vertexData[0].z );
		GLd.P_AddVertex( vertexData[4].x,vertexData[4].y,vertexData[4].z );
		GLd.P_AddVertex( vertexData[5].x,vertexData[5].y,vertexData[5].z );
		// Back
		GLd.P_PushColor(1.0f,1.0f,0.0f);
		GLd.P_AddVertex( vertexData[3].x,vertexData[3].y,vertexData[3].z );
		GLd.P_AddVertex( vertexData[2].x,vertexData[2].y,vertexData[2].z );
		GLd.P_AddVertex( vertexData[6].x,vertexData[6].y,vertexData[6].z );
		GLd.P_AddVertex( vertexData[7].x,vertexData[7].y,vertexData[7].z );
		// Left
		GLd.P_PushColor(1.0f,0.0f,1.0f);
		GLd.P_AddVertex( vertexData[3].x,vertexData[3].y,vertexData[3].z );
		GLd.P_AddVertex( vertexData[0].x,vertexData[0].y,vertexData[0].z );
		GLd.P_AddVertex( vertexData[4].x,vertexData[4].y,vertexData[4].z );
		GLd.P_AddVertex( vertexData[7].x,vertexData[7].y,vertexData[7].z );
		// Right
		GLd.P_PushColor(0.0f,1.0f,1.0f);
		GLd.P_AddVertex( vertexData[2].x,vertexData[2].y,vertexData[2].z );
		GLd.P_AddVertex( vertexData[1].x,vertexData[1].y,vertexData[1].z );
		GLd.P_AddVertex( vertexData[5].x,vertexData[5].y,vertexData[5].z );
		GLd.P_AddVertex( vertexData[6].x,vertexData[6].y,vertexData[6].z );
	GLd.EndPrimitive(lPrim);
	return true;
}