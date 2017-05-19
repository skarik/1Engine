

#include "CRenderPlane.h"


// == Constructor + Destructor ==
CRenderPlane::CRenderPlane ( Real xsize, Real ysize )
	: CRenderablePrimitive()
{
	width = xsize;
	height = ysize;

	vertexData = new arModelVertex [6];
	vertexNum = 6;

	GenerateShape( width/2.0f, height/2.0f );
}

CRenderPlane::~CRenderPlane ( void )
{
	// Nothing, as vertexData is owned by CRenderablePrimitive.
}

void CRenderPlane::SetSize ( Real xsize, Real ysize )
{
	width = xsize;
	height = ysize;

	GenerateShape( width/2.0f, height/2.0f );
}

void CRenderPlane::GenerateShape ( Real xsize, Real ysize )
{
	for ( unsigned int i = 0; i < 6; i += 1 )
	{
		vertexData[i].nx = 0;
		vertexData[i].ny = 0;
		vertexData[i].nz = -1;
	}

	// tri 1
	vertexData[0].x = -xsize;
	vertexData[0].y = -ysize;
	vertexData[0].z = 0;
	vertexData[0].u = 0;
	vertexData[0].v = 0;

	vertexData[1].x = xsize;
	vertexData[1].y = -ysize;
	vertexData[1].z = 0;
	vertexData[1].u = 1;
	vertexData[1].v = 0;

	vertexData[2].x = -xsize;
	vertexData[2].y = ysize;
	vertexData[2].z = 0;
	vertexData[2].u = 0;
	vertexData[2].v = 1;

	// tri 2
	vertexData[4].x = xsize;
	vertexData[4].y = -ysize;
	vertexData[4].z = 0;
	vertexData[4].u = 1;
	vertexData[4].v = 0;

	vertexData[3].x = -xsize;
	vertexData[3].y = ysize;
	vertexData[3].z = 0;
	vertexData[3].u = 0;
	vertexData[3].v = 1;

	vertexData[5].x = xsize;
	vertexData[5].y = ysize;
	vertexData[5].z = 0;
	vertexData[5].u = 1;
	vertexData[5].v = 1;
}