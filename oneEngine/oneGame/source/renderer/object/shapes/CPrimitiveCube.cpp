#include "CPrimitiveCube.h"
//#include "renderer/system/glMainSystem.h"
//#include "renderer/system/glDrawing.h"

// Constructor
//  sets values to unit cube, generate vertices
CPrimitiveCube::CPrimitiveCube ( void )
	: CRenderable3D ()
{
	setSize(1.0F, 1.0F, 1.0F);
}
//  sets values to user-defined, then creates vertices
CPrimitiveCube::CPrimitiveCube ( Real width, Real depth, Real height )
	: CRenderable3D ()
{
	setSize(width, depth, height);
}

void CPrimitiveCube::buildCube ( Real x, Real y, Real z )
{
	const Vector3f kHS ( x * 0.5F, y * 0.5F, z * 0.5F ); // Halfsize
	const Vector3f kCorners [8] = {
		{-kHS.x, -kHS.y, -kHS.z}, {+kHS.x, -kHS.y, -kHS.z}, {+kHS.x, +kHS.y, -kHS.z}, {-kHS.x, +kHS.y, -kHS.z},
		{-kHS.x, -kHS.y, +kHS.z}, {+kHS.x, -kHS.y, +kHS.z}, {+kHS.x, +kHS.y, +kHS.z}, {-kHS.x, +kHS.y, +kHS.z}
	};
	const Vector3f kTexCoords [4] = {
		Vector3f(0,0,0), Vector3f(1,0,0), Vector3f(1,1,0), Vector3f(0,1,0)
	};

	m_modeldata.vertices = new arModelVertex [24];
	m_modeldata.vertexNum = 24;
	m_modeldata.triangles = new arModelTriangle [12];
	m_modeldata.triangleNum = 12;

	memset(m_modeldata.vertices, 0, sizeof(arModelVertex) * m_modeldata.vertexNum);

	// Bottom
	m_modeldata.position[0 + 0] = kCorners[3];
	m_modeldata.position[0 + 1] = kCorners[2];
	m_modeldata.position[0 + 2] = kCorners[1];
	m_modeldata.position[0 + 3] = kCorners[0];
	for (uint i = 0; i < 4; ++i)
	{
		m_modeldata.normal[0 + i] = Vector3f(0.0F, 0.0F, -1.0F);
		m_modeldata.tangent[0 + i] = Vector3f(0.0F, -1.0F, 0.0F);
	}
	// Top
	m_modeldata.position[4 + 0] = kCorners[4];
	m_modeldata.position[4 + 1] = kCorners[5];
	m_modeldata.position[4 + 2] = kCorners[6];
	m_modeldata.position[4 + 3] = kCorners[7];
	for (uint i = 0; i < 4; ++i)
	{
		m_modeldata.normal[4 + i] = Vector3f(0.0F, 0.0F, 1.0F);
		m_modeldata.tangent[4 + i] = Vector3f(0.0F, 1.0F, 0.0F);
	}

	// Front
	m_modeldata.position[8 + 0] = kCorners[5];
	m_modeldata.position[8 + 1] = kCorners[4];
	m_modeldata.position[8 + 2] = kCorners[0];
	m_modeldata.position[8 + 3] = kCorners[1];
	for (uint i = 0; i < 4; ++i)
	{
		m_modeldata.normal[8 + i] = Vector3f(0.0F, -1.0F, 0.0F);
		m_modeldata.tangent[8 + i] = Vector3f(1.0F, 0.0F, 0.0F);
	}
	// Back
	m_modeldata.position[12 + 0] = kCorners[7];
	m_modeldata.position[12 + 1] = kCorners[6];
	m_modeldata.position[12 + 2] = kCorners[2];
	m_modeldata.position[12 + 3] = kCorners[3];
	for (uint i = 0; i < 4; ++i)
	{
		m_modeldata.normal[12 + i] = Vector3f(0.0F, 1.0F, 0.0F);
		m_modeldata.tangent[12 + i] = Vector3f(1.0F, 0.0F, 0.0F);
	}

	// Left
	m_modeldata.position[16 + 0] = kCorners[3];
	m_modeldata.position[16 + 1] = kCorners[0];
	m_modeldata.position[16 + 2] = kCorners[4];
	m_modeldata.position[16 + 3] = kCorners[7];
	for (uint i = 0; i < 4; ++i)
	{
		m_modeldata.normal[16 + i] = Vector3f(-1.0F, 0.0F, 0.0F);
		m_modeldata.tangent[16 + i] = Vector3f(0.0F, 1.0F, 0.0F);
	}
	// Right
	m_modeldata.position[20 + 0] = kCorners[6];
	m_modeldata.position[20 + 1] = kCorners[5];
	m_modeldata.position[20 + 2] = kCorners[1];
	m_modeldata.position[20 + 3] = kCorners[2];
	for (uint i = 0; i < 4; ++i)
	{
		m_modeldata.normal[20 + i] = Vector3f(1.0F, 0.0F, 0.0F);
		m_modeldata.tangent[20 + i] = Vector3f(0.0F, 1.0F, 0.0F);
	}

	// Set color & texcoord:
	for (uint i = 0; i < 24; ++i)
	{
		m_modeldata.color[i] = Vector4f(1.0F, 1.0F, 1.0F, 1.0F);
		m_modeldata.texcoord0[i] = kTexCoords[i % 4];
	}

	// Set triangles:
	for (uint i = 0; i < 6; ++i)
	{
		m_modeldata.triangles[i*2 + 0].vert[0] = i*4 + 0;
		m_modeldata.triangles[i*2 + 0].vert[1] = i*4 + 1;
		m_modeldata.triangles[i*2 + 0].vert[2] = i*4 + 2;
		m_modeldata.triangles[i*2 + 1].vert[0] = i*4 + 2;
		m_modeldata.triangles[i*2 + 1].vert[1] = i*4 + 3;
		m_modeldata.triangles[i*2 + 1].vert[2] = i*4 + 0;
	}
}

// Set a new size
void CPrimitiveCube::setSize ( Real width, Real depth, Real height )
{
	buildCube(width, depth, height);

	PushModeldata();
	
	delete[] m_modeldata.vertices;
	delete[] m_modeldata.triangles;
	m_modeldata.vertices = NULL;
	m_modeldata.triangles= NULL;
}