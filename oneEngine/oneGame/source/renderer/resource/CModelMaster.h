
#ifndef _C_MODEL_MASTER_H_
#define _C_MODEL_MASTER_H_

// Includes
//#include "rrMesh.h"
//#include "physMesh.h"
//#include "CAnimation.h"
//#include "CMorpher.h"
//#include "sHitbox.h"
#include "core/types/types.h"
#include "core/containers/arsingleton.h"
#include "core/containers/arstring.h"
#include <vector>
#include <map>
#include <string>

struct sHitbox;

class rrMesh;
class physMesh;
class CAnimation;
class CMorpher;

// Storage Class Definition
/*class MeshSetReference
{
public:
	unsigned int referenceCount;
	std::vector<rrMesh*> vMeshes;
	std::vector<sHitbox> vHitboxes;
};
class PhysSetReference
{
public:
	unsigned int referenceCount;
	std::vector<physMesh*> vMeshes;
};
class AnimSetReference
{
public:
	unsigned int referenceCount;
	CAnimation*	pAnimSet;
};
class MorphSetReference
{
public:
	unsigned int referenceCount;
	CMorpher*	pAnimSet;
};*/
/*
// Main Class Definition
class CModelMaster
{
public:
	CModelMaster ( void );
	~CModelMaster ( void );

	void AddReference ( const string& filename, std::vector<rrMesh*>& meshes, std::vector<sHitbox>& hitboxes );
	void AddReference ( const string& filename, std::vector<physMesh*>& meshes );
	void AddReference ( const string& filename, CAnimation* animSet );
	void AddReference ( const string& filename, CMorpher* animSet );

	void RemoveReference ( const string& filename );
	void RemovePhysReference ( const string& filename );
	void RemoveAnimSetReference ( const string& filename );
	void RemoveMorpherSetReference ( const string& filename );

	const std::vector<rrMesh*>*		GetReference ( const string& filename );
	const std::vector<physMesh*>*	GetCollisionReference ( const string& filename );
	const std::vector<sHitbox>*		GetHitboxReference ( const string& filename );
	CAnimation*					GetAnimationReference ( const string& filename );
	CMorpher*					GetMorpherReference ( const string& filename );

	void Cleanup ( void );
private:
	std::map<string,MeshSetReference> meshMap;
	std::map<string,PhysSetReference> physMeshMap;
	std::map<string,AnimSetReference> animSetMap;
	std::map<string,MorphSetReference> morphSetMap;
};

// Global Class
extern CModelMaster ModelMaster;*/

class RenderResources
{
	// Singleton definition
	ARSINGLETON_H_STORAGE(RenderResources,RENDER_API)
	ARSINGLETON_H_ACCESS(RenderResources)

public:
	RENDER_API void AddMeshSet ( const char* filename, std::vector<rrMesh*>& meshes );
	RENDER_API void AddMorphSet ( const char* filename, CMorpher* morphSet );

	//	GetMesh ( filename )
	// Returns the mesh set saved previously, and increments the reference count.
	// Returns NULL if no reference is found.
	RENDER_API const std::vector<rrMesh*>*		GetMesh ( const char* filename );

	//	GetMorpher ( filename )
	// Returns the morpher saved previously, and increments the reference count.
	// Returns NULL if no reference is found.
	RENDER_API const CMorpher*					GetMorpher ( const char* filename );

	RENDER_API void ReleaseMeshSet ( const char* filename );
	RENDER_API void ReleaseMorphSet ( const char* filename );

	//	Cleanup ()
	// Deletes all mesh sets and morphs with no existing references
	RENDER_API void Cleanup ( void );

private:
	struct meshset_reference_t
	{
		uint16_t refCount;
		std::vector<rrMesh*> set;
	};
	struct morphset_reference_t
	{
		uint16_t refCount;
		CMorpher* set;
	};

	std::map<arstring256,meshset_reference_t>	meshSets;
	std::map<arstring256,morphset_reference_t>	morphSets;
};

#endif