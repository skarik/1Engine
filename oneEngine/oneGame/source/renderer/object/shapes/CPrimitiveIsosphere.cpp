#include "CPrimitiveIsosphere.h"
//#include "renderer/system/glMainSystem.h"
//#include "renderer/system/glDrawing.h"
#include "renderer/camera/RrCamera.h"
#include <math.h>

//==Constants==
const Real kIsoSphereX = .525731112119133606f;
const Real kIsoSphereZ = .850650808352039932f;
const Vector3f kIsoSphereVertexSeed[12] = {    
    {-kIsoSphereX, 0.0, kIsoSphereZ}, {kIsoSphereX, 0.0, kIsoSphereZ}, {-kIsoSphereX, 0.0, -kIsoSphereZ}, {kIsoSphereX, 0.0, -kIsoSphereZ},    
    {0.0, kIsoSphereZ, kIsoSphereX}, {0.0, kIsoSphereZ, -kIsoSphereX}, {0.0, -kIsoSphereZ, kIsoSphereX}, {0.0, -kIsoSphereZ, -kIsoSphereX},    
    {kIsoSphereZ, kIsoSphereX, 0.0}, {-kIsoSphereZ, kIsoSphereX, 0.0}, {kIsoSphereZ, -kIsoSphereX, 0.0}, {-kIsoSphereZ, -kIsoSphereX, 0.0} 
};
const arModelTriangle kIsoSphereTriangles[20] = { 
    {0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},    
    {8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},    
    {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6}, 
    {6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11}
};

CPrimitiveIsosphere::CPrimitiveIsosphere ( int divisions )
	: CRenderable3D ()
{
	buildIsosphere(divisions);
	PushModeldata();
	
	// Free model data now.
	delete[] m_modeldata.vertices;
	delete[] m_modeldata.triangles;
}

CPrimitiveIsosphere::~CPrimitiveIsosphere ( void )
{
	;
}

FORCE_INLINE void rrAddTri ( int* vindex, int* tindex, arModelData* md, Vector3f a, Vector3f b, Vector3f c, int div )
{
	if (div <= 0)
	{
		// Build tex-coord values:
		float ay = atan2(a[2],a[0])/(float)M_PI;
		float by = atan2(b[2],b[0])/(float)M_PI;
		float cy = atan2(c[2],c[0])/(float)M_PI;

		if (ay > 0.8f)
		{
			if (by < 0.2f)
				by += 1.0F;
			if (cy < 0.2f)
				cy += 1.0F;
		}
		if (by > 0.8f)
		{
			if (ay < 0.2f)
				ay += 1.0F;
			if (cy < 0.2f)
				cy += 1.0F;
		}
		if (cy > 0.8f)
		{
			if (ay < 0.2f)
				ay += 1.0F;
			if (by < 0.2f)
				by += 1.0F;
		}

		// Add a triangle:
		md->vertices[*vindex + 0].position = a;
		md->vertices[*vindex + 1].position = b;
		md->vertices[*vindex + 2].position = c;

		md->vertices[*vindex + 0].normal = a;
		md->vertices[*vindex + 1].normal = b;
		md->vertices[*vindex + 2].normal = c;

		md->vertices[*vindex + 0].texcoord0 = Vector2f((a[1]+1)*0.5f, ay);
		md->vertices[*vindex + 1].texcoord0 = Vector2f((b[1]+1)*0.5f, by);
		md->vertices[*vindex + 2].texcoord0 = Vector2f((c[1]+1)*0.5f, cy);

		md->triangles[*tindex].vert[0] = *vindex + 0;
		md->triangles[*tindex].vert[1] = *vindex + 1;
		md->triangles[*tindex].vert[2] = *vindex + 2;

		*vindex += 3;
		*tindex += 1;
	}
	else 
	{
		// We need to subdivide further.
		Vector3f ab = (a + b) / 2;
		Vector3f ac = (a + c) / 2;
		Vector3f bc = (b + c) / 2;

		ab.normalize(); ac.normalize(); bc.normalize();

		rrAddTri(vindex, tindex, md, a, ab, ac, div-1);
		rrAddTri(vindex, tindex, md, b, bc, ab, div-1);
		rrAddTri(vindex, tindex, md, c, ac, bc, div-1);
		rrAddTri(vindex, tindex, md, ab, bc, ac, div-1);
	}  
}

void CPrimitiveIsosphere::buildIsosphere ( int divisions )
{
	// Each division splits every triangle into 4 new triangles.
	m_modeldata.vertexNum	= (uint16_t)(20 * 3 * (int)powf(4, (Real)divisions));
	m_modeldata.triangleNum = (uint16_t)(20 * (int)powf(4, (Real)divisions));

	// Create data arrays
	m_modeldata.vertices	= new arModelVertex [m_modeldata.vertexNum];
	m_modeldata.triangles	= new arModelTriangle [m_modeldata.triangleNum];

	memset(m_modeldata.vertices, 0, sizeof(arModelVertex) * m_modeldata.vertexNum);

	int vert = 0, tri = 0;
	for (int i = 0; i < 20; i++)
	{
		rrAddTri(
			&vert, &tri, &m_modeldata,
			kIsoSphereVertexSeed[kIsoSphereTriangles[i].vert[0]],
			kIsoSphereVertexSeed[kIsoSphereTriangles[i].vert[1]],
			kIsoSphereVertexSeed[kIsoSphereTriangles[i].vert[2]],
			divisions);
	}
}
