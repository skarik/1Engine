#ifndef _C_MODEL_DATA_
#define _C_MODEL_DATA_

#include "core/math/Vector3d.h"
#include "core/types/types.h"

// 128 byte Model Vertex class
struct ModelVertex
{
	float x,y,z; // Position
	float u,v,w; // UV's
	float nx,ny,nz; // Normal
	float tx,ty,tz; // Tangent
	float bx,by,bz; // Binormal
	float r,g,b,a; // Vertex color //76
	unsigned char bone[4]; // Bone indexes //80
	float weight[4]; // Bone weights //96
	float u2,v2,w2; //108
	float u3,v3,w3; //120
	float u4,v4; //128
};
typedef ModelVertex CModelVertex;

inline Vector3d _VertexToVect ( CModelVertex& vert )
{
	return Vector3d( vert.x, vert.y, vert.z );
};
inline Vector3d _NormalToVect ( CModelVertex& vert )
{
	return Vector3d( vert.nx, vert.ny, vert.nz );
};

// 24 byte Physics Vertex class
struct PhysicsVertex
{
public:
	float x,y,z; // Position
	float nx,ny,nz; // Normal
};
typedef PhysicsVertex CPhysicsVertex;

// 64 byte Terrain Vertex class
/*class CTerrainVertex
{
public:
	float x,y,z; // Position
	float u,v,w; // UV's
	float nx,ny,nz; // Normal
	float tx,ty,tz; // Tangent
	float r,g,b,a; // Vertex color (texture blending)
};*/
// 96 byte Terrain Vertex class
struct TerrainVertex
{
public:
	float x,y,z; // Position
	float u,v,w; // UV's
	float nx,ny,nz; // Normal
	float tx,ty,tz; // Tangent
	float r,g,b,a; // Vertex color (texture blending)
	float l1,l2,l3,l4; // More texture blending
	float m1,m2,m3,m4; // More texture blending
};
typedef TerrainVertex CTerrainVertex;

// 64 byte Particle Vertex class
struct ParticleVertex
{
public:
	float x,y,z; // Position
	float nx,ny,nz; // Normal
	float u,v; // UV's
	float r,g,b,a; // Vertex color
	float r2,g2,b2,a2; // Secondary color
};
typedef ParticleVertex CParticleVertex;

struct ModelTriangle
{
public:
	uint32_t vert [3];
};
typedef ModelTriangle CModelTriangle;

struct ModelQuad
{
public:
	unsigned short vert [4];
};
typedef ModelQuad CModelQuad;

struct ModelData
{
public:
	CModelVertex* vertices;
	CModelTriangle* triangles;
	unsigned int vertexNum;
	unsigned int triangleNum;
};
typedef ModelData CModelData;

struct PhysicsData
{
public:
	CPhysicsVertex* vertices;
	CModelTriangle* triangles;
	unsigned int vertexNum;
	unsigned int triangleNum;
};
typedef PhysicsData CPhysicsData;


#endif