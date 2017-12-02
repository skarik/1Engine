#include "PrMesh.h"
#include "physical/interface/tobt.h"

#include "BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h"
#include "BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h"

#include "BulletCollision/CollisionShapes/btTriangleShape.h"
#include "BulletCollision/CollisionShapes/btConvex2dShape.h"
#include "BulletCollision/CollisionShapes/btCompoundShape.h"

#include "BulletCollision/CollisionShapes/btBox2dShape.h"

PrMesh::PrMesh ( void )
{
	shape = NULL;
}

PrMesh::~PrMesh ( void )
{
	delete shape;
	shape = NULL;
}

// Init
void PrMesh::Initialize ( arModelPhysicsData* const pNewModelData, const bool force2Dunoptimized )
{
	if (!force2Dunoptimized)
	{
		btTriangleIndexVertexArray* meshInterface = new btTriangleIndexVertexArray();
		btIndexedMesh part;

		part.m_vertexBase = (const unsigned char*)pNewModelData->vertices;
		part.m_vertexStride = sizeof(arPhysicsVertex);
		part.m_numVertices = pNewModelData->vertexNum;
		part.m_triangleIndexBase = (const unsigned char*)pNewModelData->triangles;
		part.m_triangleIndexStride = sizeof(arModelTriangle);
		part.m_numTriangles = pNewModelData->triangleNum;
		part.m_indexType = PHY_SHORT;

		meshInterface->addIndexedMesh(part, PHY_SHORT);

		bool useQuantizedAabbCompression = true;
		btBvhTriangleMeshShape* trimeshShape = new btBvhTriangleMeshShape(meshInterface, useQuantizedAabbCompression);
	
		shape = trimeshShape;
	}
	// 2D needs a special hack for it to work:
	else
	{
		btCompoundShape* compoundShape = new btCompoundShape(true, pNewModelData->triangleNum);
	
		for (uint i = 0; i < pNewModelData->triangleNum; ++i)
		{
			btTriangleShape* tri = new btTriangleShape(
				physical::bt(pNewModelData->vertices[pNewModelData->triangles[i].vert[0]].position),
				physical::bt(pNewModelData->vertices[pNewModelData->triangles[i].vert[1]].position),
				physical::bt(pNewModelData->vertices[pNewModelData->triangles[i].vert[2]].position));
			btConvex2dShape* part = new btConvex2dShape(tri);
			//btBox2dShape* part = new btBox2dShape(physical::bt(pNewModelData->vertices[pNewModelData->triangles[i].vert[2]].position));

			compoundShape->addChildShape(
				btTransform::getIdentity(),
				part);
			shape = part;
		}
		compoundShape->createAabbTreeFromChildren();

		shape = compoundShape;
	}
}