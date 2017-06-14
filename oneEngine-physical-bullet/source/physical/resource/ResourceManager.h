
#ifndef _PHYSICS_RESOURCE_MANAGER_H_
#define _PHYSICS_RESOURCE_MANAGER_H_

#include "core/types/types.h"
#include "core/containers/arsingleton.h"
#include "core/containers/arstring.h"
#include "core-ext/animation/Skeleton.h"
#include "core-ext/types/sHitbox.h"

#include <vector>
#include <map>

class physMesh;
class CAnimationSet;

class PhysicalResources
{
	// Singleton definition
	ARSINGLETON_H_STORAGE(PhysicalResources,PHYS_API)
	ARSINGLETON_H_ACCESS(PhysicalResources)

public:
	PHYS_API void AddAnimationSet ( const char* filename, CAnimationSet* animSet );
	PHYS_API void AddSkeleton ( const char* filename, animation::Skeleton& skeleton );
	PHYS_API void AddPhysMeshSet ( const char* filename, std::vector<physMesh*>& meshSet );
	PHYS_API void AddHitboxSet ( const char* filename, std::vector<sHitbox>& hitboxSet );

	//	GetAnimationSet ( filename )
	// Returns the animation set saved previously, and increments the reference count.
	// Returns NULL if no reference is found.
	PHYS_API const CAnimationSet*			GetAnimationSet ( const char* filename );

	//	GetSkeleton ( filename )
	// Returns the skeleton saved previously, and increments the reference count.
	// Returns NULL if no reference is found.
	PHYS_API const animation::Skeleton*		GetSkeleton ( const char* filename );

	//	GetPhysMesh ( filename )
	// Returns the skeleton saved previously, and increments the reference count.
	// Returns NULL if no reference is found.
	PHYS_API const std::vector<physMesh*>*	GetPhysMesh ( const char* filename );

	//	GetHitbox ( filename )
	// Returns the skeleton saved previously, and increments the reference count.
	// Returns NULL if no reference is found.
	PHYS_API const std::vector<sHitbox>*		GetHitbox ( const char* filename );

	PHYS_API void ReleaseAnimationSet ( const char* filename );
	PHYS_API void ReleaseSkeleton ( const char* filename );
	PHYS_API void ReleasePhysMeshSet ( const char* filename );
	PHYS_API void ReleaseHitboxSet ( const char* filename );

	//	Cleanup ()
	// Deletes all mesh sets and morphs with no existing references
	PHYS_API void Cleanup ( void );

private:
	struct animationset_reference_t
	{
		uint16_t refCount;
		CAnimationSet* set;
	};
	struct skelly_reference_t
	{
		uint16_t refCount;
		animation::Skeleton set;
	};
	struct physmeshset_reference_t
	{
		uint16_t refCount;
		std::vector<physMesh*> set;
	};
	struct hitbox_reference_t
	{
		uint16_t refCount;
		std::vector<sHitbox> set;
	};

	std::map<arstring256,animationset_reference_t>	animSets;
	std::map<arstring256,skelly_reference_t>		skellySets;
	std::map<arstring256,physmeshset_reference_t>	physmeshSets;
	std::map<arstring256,hitbox_reference_t>		hitboxSets;
};


#endif//_PHYSICS_RESOURCE_MANAGER_H_