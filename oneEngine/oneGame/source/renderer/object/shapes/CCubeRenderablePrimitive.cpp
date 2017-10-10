
// Includes
#include "CCubeRenderablePrimitive.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

// Constructor
//  sets values to unit cube, generate vertices
CPrimitiveCube::CPrimitiveCube ( void )
	: CRenderablePrimitive ()
{
	setSize(1.0F, 1.0F, 1.0F);
}
//  sets values to user-defined, then creates vertices
CPrimitiveCube::CPrimitiveCube ( Real width, Real depth, Real height )
	: CRenderablePrimitive ()
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

	m_modeldata.vertices = new arModelVertex [24];
	m_modeldata.vertexNum = 24;
	m_modeldata.triangles = new arModelTriangle [12];
	m_modeldata.triangleNum = 12;

	memset(m_modeldata.vertices, 0, sizeof(arModelVertex) * m_modeldata.vertexNum);

	// Bottom
	m_modeldata.vertices[0 + 0].position = kCorners[3];
	m_modeldata.vertices[0 + 1].position = kCorners[2];
	m_modeldata.vertices[0 + 2].position = kCorners[1];
	m_modeldata.vertices[0 + 3].position = kCorners[0];
	for (uint i = 0; i < 4; ++i)
	{
		m_modeldata.vertices[0 + i].normal = Vector3f(0.0F, 0.0F, -1.0F);
		m_modeldata.vertices[0 + i].tangent = Vector3f(0.0F, -1.0F, 0.0F);
	}
	// Top
	m_modeldata.vertices[4 + 0].position = kCorners[4];
	m_modeldata.vertices[4 + 1].position = kCorners[5];
	m_modeldata.vertices[4 + 2].position = kCorners[6];
	m_modeldata.vertices[4 + 3].position = kCorners[7];
	for (uint i = 0; i < 4; ++i)
	{
		m_modeldata.vertices[4 + i].normal = Vector3f(0.0F, 0.0F, 1.0F);
		m_modeldata.vertices[4 + i].tangent = Vector3f(0.0F, 1.0F, 0.0F);
	}

	// Front
	m_modeldata.vertices[8 + 0].position = kCorners[5];
	m_modeldata.vertices[8 + 1].position = kCorners[4];
	m_modeldata.vertices[8 + 2].position = kCorners[0];
	m_modeldata.vertices[8 + 3].position = kCorners[1];
	for (uint i = 0; i < 4; ++i)
	{
		m_modeldata.vertices[8 + i].normal = Vector3f(0.0F, -1.0F, 0.0F);
		m_modeldata.vertices[8 + i].tangent = Vector3f(1.0F, 0.0F, 0.0F);
	}
	// Back
	m_modeldata.vertices[12 + 0].position = kCorners[7];
	m_modeldata.vertices[12 + 1].position = kCorners[6];
	m_modeldata.vertices[12 + 2].position = kCorners[2];
	m_modeldata.vertices[12 + 3].position = kCorners[3];
	for (uint i = 0; i < 4; ++i)
	{
		m_modeldata.vertices[12 + i].normal = Vector3f(0.0F, 1.0F, 0.0F);
		m_modeldata.vertices[12 + i].tangent = Vector3f(1.0F, 0.0F, 0.0F);
	}

	// Left
	m_modeldata.vertices[16 + 0].position = kCorners[3];
	m_modeldata.vertices[16 + 1].position = kCorners[0];
	m_modeldata.vertices[16 + 2].position = kCorners[4];
	m_modeldata.vertices[16 + 3].position = kCorners[7];
	for (uint i = 0; i < 4; ++i)
	{
		m_modeldata.vertices[16 + i].normal = Vector3f(-1.0F, 0.0F, 0.0F);
		m_modeldata.vertices[16 + i].tangent = Vector3f(0.0F, 1.0F, 0.0F);
	}
	// Right
	m_modeldata.vertices[20 + 0].position = kCorners[6];
	m_modeldata.vertices[20 + 1].position = kCorners[5];
	m_modeldata.vertices[20 + 2].position = kCorners[1];
	m_modeldata.vertices[20 + 3].position = kCorners[2];
	for (uint i = 0; i < 4; ++i)
	{
		m_modeldata.vertices[20 + i].normal = Vector3f(1.0F, 0.0F, 0.0F);
		m_modeldata.vertices[20 + i].tangent = Vector3f(0.0F, 1.0F, 0.0F);
	}

	// Set color & texcoord:
	for (uint i = 0; i < 24; ++i)
	{
		m_modeldata.vertices[i].color = Vector4f(1.0F, 1.0F, 1.0F, 1.0F);
		m_modeldata.vertices[i].texcoord0 = Vector3f(0.5F,0.5F,0.5F);
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
}