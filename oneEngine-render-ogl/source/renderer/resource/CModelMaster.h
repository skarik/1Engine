
#ifndef _C_MODEL_MASTER_H_
#define _C_MODEL_MASTER_H_

// Includes
//#include "glMesh.h"
//#include "physMesh.h"
//#include "CAnimation.h"
//#include "CMorpher.h"
//#include "sHitbox.h"
#include "core-ext/types/sHitbox.h"
#include <vector>
#include <map>
#include <string>
using std::string;

struct sHitbox;

class glMesh;
class physMesh;
class CAnimation;
class CMorpher;

// Storage Class Definition
class MeshSetReference
{
public:
	unsigned int referenceCount;
	std::vector<glMesh*> vMeshes;
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
};

// Main Class Definition
class CModelMaster
{
public:
	CModelMaster ( void );
	~CModelMaster ( void );

	void AddReference ( const string& filename, std::vector<glMesh*>& meshes, std::vector<sHitbox>& hitboxes );
	void AddReference ( const string& filename, std::vector<physMesh*>& meshes );
	void AddReference ( const string& filename, CAnimation* animSet );
	void AddReference ( const string& filename, CMorpher* animSet );

	void RemoveReference ( const string& filename );
	void RemovePhysReference ( const string& filename );
	void RemoveAnimSetReference ( const string& filename );
	void RemoveMorpherSetReference ( const string& filename );

	const std::vector<glMesh*>*		GetReference ( const string& filename );
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
extern CModelMaster ModelMaster;

#endif