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
	if (!force2Dunoptimized || true)
	{
		btTriangleIndexVertexArray* meshInterface = new btTriangleIndexVertexArray();
		btIndexedMesh part;

		part.m_vertexBase = (const unsigned char*)pNewModelData->position;
		part.m_vertexStride = sizeof(Vector3f);
		part.m_numVertices = pNewModelData->vertexNum;
		part.m_triangleIndexBase = (const unsigned char*)pNewModelData->indices;
		part.m_triangleIndexStride = sizeof(uint16_t) * 3;
		part.m_numTriangles = pNewModelData->indexNum / 3;

		meshInterface->addIndexedMesh(part, PHY_INTEGER);

		bool useQuantizedAabbCompression = true;
		btBvhTriangleMeshShape* trimeshShape = new btBvhTriangleMeshShape(meshInterface, useQuantizedAabbCompression);
	
		shape = trimeshShape;
	}
	// 2D needs a special hack for it to work:
	else
	{
		btCompoundShape* compoundShape = new btCompoundShape(true, pNewModelData->indexNum / 3);
	
		for (uint i = 0; i < pNewModelData->indexNum; i += 3)
		{
			btTriangleShape* tri = new btTriangleShape(
				physical::bt(pNewModelData->position[pNewModelData->indices[i + 0]]),
				physical::bt(pNewModelData->position[pNewModelData->indices[i + 1]]),
				physical::bt(pNewModelData->position[pNewModelData->indices[i + 2]]));
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