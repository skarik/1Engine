#include "ResourceManager.h"
#include "core-ext/animation/set/AnimationSet.h"
#include "physical/physics/shapes/PrMesh.h"

ARSINGLETON_CPP_DEF(PhysicalResources);

#define HANDLE_REFERENCE_FIND(LISTING) \
	for ( auto itr = LISTING.begin(); itr != LISTING.end(); ++itr ) \
		if ( itr->first.compare( filename ) )

#define HANDLE_REFERENCE_DECREMENT(LISTING) \
	HANDLE_REFERENCE_FIND(LISTING) { \
		if ( itr->second.refCount == 0 ) throw core::NullReferenceException(); \
		itr->second.refCount--; return; \
	} throw core::NullReferenceException();

void PhysicalResources::AddAnimationSet ( const char* filename, AnimationSet* animSet )
{
	HANDLE_REFERENCE_FIND(animSets) throw core::InvalidCallException();
	animationset_reference_t ref;
	ref.refCount = 0;
	ref.set = animSet;
	animSets[arstring256(filename)] = ref;
}

void PhysicalResources::AddSkeleton ( const char* filename, animation::Skeleton& skeleton )
{
	HANDLE_REFERENCE_FIND(skellySets) throw core::InvalidCallException();
	skelly_reference_t ref;
	ref.refCount = 0;
	ref.set = skeleton;
	skellySets[arstring256(filename)] = ref;
}

void PhysicalResources::AddPhysMeshSet ( const char* filename, std::vector<PrMesh*>& meshSet )
{
	HANDLE_REFERENCE_FIND(physmeshSets) throw core::InvalidCallException();
	physmeshset_reference_t ref;
	ref.refCount = 0;
	ref.set = meshSet;
	physmeshSets[arstring256(filename)] = ref;
}

void PhysicalResources::AddHitboxSet ( const char* filename, std::vector<sHitbox>& hitboxSet )
{
	HANDLE_REFERENCE_FIND(hitboxSets) throw core::InvalidCallException();
	hitbox_reference_t ref;
	ref.refCount = 0;
	ref.set = hitboxSet;
	hitboxSets[arstring256(filename)] = ref;
}

//	GetAnimationSet ( filename )
// Returns the animation set saved previously, and increments the reference count.
// Returns NULL if no reference is found.
const AnimationSet* PhysicalResources::GetAnimationSet ( const char* filename )
{
	HANDLE_REFERENCE_FIND(animSets)
	{
		itr->second.refCount++;
		return itr->second.set;
	}
	return NULL;
}

//	GetSkeleton ( filename )
// Returns the skeleton saved previously, and increments the reference count.
// Returns NULL if no reference is found.
const animation::Skeleton* PhysicalResources::GetSkeleton ( const char* filename )
{
	HANDLE_REFERENCE_FIND(skellySets)
	{
		itr->second.refCount++;
		return &itr->second.set;
	}
	return NULL;
}

//	GetPhysMesh ( filename )
// Returns the skeleton saved previously, and increments the reference count.
// Returns NULL if no reference is found.
const std::vector<PrMesh*>* PhysicalResources::GetPhysMesh ( const char* filename )
{
	HANDLE_REFERENCE_FIND(physmeshSets)
	{
		itr->second.refCount++;
		return &itr->second.set;
	}
	return NULL;
}

//	GetHitbox ( filename )
// Returns the skeleton saved previously, and increments the reference count.
// Returns NULL if no reference is found.
const std::vector<sHitbox>* PhysicalResources::GetHitbox ( const char* filename )
{
	HANDLE_REFERENCE_FIND(hitboxSets)
	{
		itr->second.refCount++;
		return &itr->second.set;
	}
	return NULL;
}

void PhysicalResources::ReleaseAnimationSet ( const char* filename )
{
	HANDLE_REFERENCE_DECREMENT(animSets)
}
void PhysicalResources::ReleaseSkeleton ( const char* filename )
{
	HANDLE_REFERENCE_DECREMENT(skellySets)
}
void PhysicalResources::ReleasePhysMeshSet ( const char* filename )
{
	HANDLE_REFERENCE_DECREMENT(physmeshSets)
}
void PhysicalResources::ReleaseHitboxSet ( const char* filename )
{
	HANDLE_REFERENCE_DECREMENT(hitboxSets)
}

//	Cleanup ()
// Deletes all mesh sets and morphs with no existing references
void PhysicalResources::Cleanup ( void )
{
	// Clean up the animation sets
	for ( auto itr = animSets.begin(); itr != animSets.end(); )
	{
		if ( itr->second.refCount == 0 )
		{
			// Delete the animation set
			delete itr->second.set;
			// Remove this entry from the list
			itr = animSets.erase(itr);
		}
		else
		{
			++itr;
		}
	}

	// Clean up the skellys
	for ( auto itr = skellySets.begin(); itr != skellySets.end(); )
	{
		if ( itr->second.refCount == 0 )
		{
			// Remove this entry from the list
			itr = skellySets.erase(itr);
		}
		else
		{
			++itr;
		}
	}

	// Clean up the physmeshes
	for ( auto itr = physmeshSets.begin(); itr != physmeshSets.end(); )
	{
		if ( itr->second.refCount == 0 )
		{
			for ( size_t i = 0; i < itr->second.set.size(); ++i )
			{
				// Delete all the meshes in the list
				delete itr->second.set[i];
			}
			// Remove this entry from the list
			itr = physmeshSets.erase(itr);
		}
		else
		{
			++itr;
		}
	}

	// Clean up the hitboxes
	for ( auto itr = hitboxSets.begin(); itr != hitboxSets.end(); )
	{
		if ( itr->second.refCount == 0 )
		{
			// Remove this entry from the list
			itr = hitboxSets.erase(itr);
		}
		else
		{
			++itr;
		}
	}

	// Done cleaning
}


#undef HANDLE_REFERENCE_FIND
#undef HANDLE_REFERENCE_DECREMENT