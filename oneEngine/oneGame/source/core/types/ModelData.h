#ifndef CORE_MODEL_DATA_
#define CORE_MODEL_DATA_

#include "core/types/types.h"
#include "core/math/Vector2d.h"
#include "core/math/Vector3d.h"
#include "core/math/Vector4d.h"

// Vertex structures:

//	Default ModelVertex structure
// 128 bytes
// use <stddef> offsetof macro or function in order for quick and readable offsets into vertex structures.
struct arModelVertex
{
	arModelVertex() {} // Empty default constructor
	arModelVertex(const arModelVertex&c) { memcpy(rawbytes, c.rawbytes, 128); } // Copy constructor
	union
	{
		struct
		{
			uint8_t rawbytes [128];
		};
		struct
		{
			// float3 position
			Real32 x,y,z;
			// float3 texcoord0
			Real32 u,v,w;
			// float3 normal
			Real32 nx,ny,nz;
			// float3 tangent (texcoord1)
			Real32 tx,ty,tz;
			// float3 binormal (unused)
			Real32 bx,by,bz;
			// float4 vertex color
			Real32 r,g,b,a;
	
			// uchar4 bone indices.
			// 253 bones are supported. Bone 0 and 255 have special meaning.
			uint8_t bone[4];
			// float4 bone weights
			Real32 weight[4];

			// float3 texcoord2
			Real32 u2,v2,w2; 
			// float3 texcoord3
			Real32 u3,v3,w3; 
			// float3 texcoord4
			Real32 u4,v4; 
		};
		struct
		{
			Vector3f	position;
			Vector3f	texcoord0;
			Vector3f	normal;
			Vector3f	tangent;
			Vector3f	binormal;
			Vector4f	color;

			// uchar4 bone indices.
			// 253 bones are supported. Bone 0 and 255 have special meaning.
			//Vector4u8	bone;
			uint8_t _bones[4];
			// float4 bone weights
			//Vector4f weights;
			Real32 _weight[4];

			Vector3f texcoord2;
			Vector3f texcoord3;
			Vector2f texcoord4;
		};
	};
};
static_assert(sizeof(arModelVertex)==128, "[core] arModelVertex must be 128 bytes.");

//	Default PhysicsVertex structure
// 24 bytes
struct arPhysicsVertex
{
	arPhysicsVertex() {} // Empty default constructor
	arPhysicsVertex(const arPhysicsVertex&c) { memcpy(rawbytes, c.rawbytes, 24); } // Copy constructor
	union
	{
		struct
		{
			uint8_t rawbytes [24];
		};
		struct
		{
			// float3 position
			float x,y,z;
			// float3 normal
			float nx,ny,nz;
		};
		struct
		{
			Vector3f	position;
			Vector3f	normal;
		};
	};
};
static_assert(sizeof(arPhysicsVertex)==24, "[core] arPhysicsVertex must be 24 bytes.");

//	Extended Terrain Vertex structure
// 96 byte 
// use <stddef> offsetof macro or function in order for quick and readable offsets into vertex structures.
struct arTerrainVertex
{
	float x,y,z; // Position
	float u,v,w; // UV's
	float nx,ny,nz; // Normal
	float tx,ty,tz; // Tangent
	float r,g,b,a; // Vertex color (texture blending)
	float l1,l2,l3,l4; // More texture blending
	float m1,m2,m3,m4; // More texture blending
};

//	Default Particle Vertex structure.
// 64 byte
// use <stddef> offsetof macro or function in order for quick and readable offsets into vertex structures.
struct arParticleVertex
{
	float x,y,z; // Position
	float nx,ny,nz; // Normal
	float u,v; // UV's
	float r,g,b,a; // Vertex color
	float r2,g2,b2,a2; // Secondary color
};

// Index structures:

// 32-bit index holder
struct arModelTriangle
{
	uint32_t vert [3];
};
// 16-bit index holder
struct arModelQuad
{
	uint16_t vert [4];
};

// Model structures:

//	Default model data storage class.
// Limit of 65000 vertices.
struct arModelData
{
	arModelVertex* vertices;
	arModelTriangle* triangles;
	uint16_t vertexNum;
	uint16_t triangleNum;
};

//	Default model data storage class for physics engine.
// Limit of 65000 vertices.
struct arModelPhysicsData
{
	arPhysicsVertex* vertices;
	arModelTriangle* triangles;
	uint16_t vertexNum;
	uint16_t triangleNum;
};


#endif//CORE_MODEL_DATA_