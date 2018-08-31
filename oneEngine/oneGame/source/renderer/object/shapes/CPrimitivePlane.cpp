#include "CPrimitivePlane.h"

CPrimitivePlane::CPrimitivePlane ( Real xsize, Real ysize )
	: CRenderable3D()
{
	setSize(xsize, ysize);
}

CPrimitivePlane::~CPrimitivePlane ( void )
{
	;
}

void CPrimitivePlane::setSize ( Real xsize, Real ysize )
{
	buildPlane( xsize/2.0F, ysize/2.0F );
	PushModeldata();

	// Remove unneeded data:
	delete[] m_modeldata.vertices;
	delete[] m_modeldata.triangles;

	m_modeldata.vertices = NULL;
	m_modeldata.triangles = NULL;
}

void CPrimitivePlane::buildPlane ( Real hxsize, Real hysize )
{
	// Initialize data if it doesn't exist.
	if (m_modeldata.vertices == NULL)
	{
		m_modeldata.vertexNum = 4;	// TODO: Put this data on the stack instead. It's not that complicated.
		m_modeldata.triangleNum = 2;

		m_modeldata.vertices = new arModelVertex [m_modeldata.vertexNum];
		m_modeldata.triangles = new arModelTriangle [2];

		//memset(m_modeldata.vertices, 0, sizeof(arModelVertex) * m_modeldata.vertexNum);
		for ( uint i = 0; i < m_modeldata.vertexNum; i += 1 )
		{
			m_modeldata.normal[i] = Vector3f(0, 0, -1.0F);
		}

		// Set UVs:
		m_modeldata.texcoord0[0] = Vector2f(0,0);
		m_modeldata.texcoord0[1] = Vector2f(1,0);
		m_modeldata.texcoord0[2] = Vector2f(0,1);
		m_modeldata.texcoord0[3] = Vector2f(1,1);

		// Set triangles:
		m_modeldata.triangles[0].vert[0] = 0;
		m_modeldata.triangles[0].vert[1] = 1;
		m_modeldata.triangles[0].vert[2] = 2;

		m_modeldata.triangles[1].vert[0] = 2;
		m_modeldata.triangles[1].vert[1] = 1;
		m_modeldata.triangles[1].vert[2] = 3;
	}

	// Set new positions
	m_modeldata.position[0] = Vector2f(-hxsize, -hysize);
	m_modeldata.position[1] = Vector2f( hxsize, -hysize);
	m_modeldata.position[2] = Vector2f(-hxsize,  hysize);
	m_modeldata.position[3] = Vector2f( hxsize,  hysize);
}