#ifndef PHYSICS_RP_CAST_H_
#define PHYSICS_RP_CAST_H_

#include "core/math/Ray.h"
#include "core-ext/types/baseType.h"
#include "physical/types/collisionMask.h"
#include "physical/physics/cast/RaycastHit.h"

class PrWorld;

//	struct rpRaycastQuery : input options for a raycast request
struct prRaycastQuery
{
	// World to cast into
	PrWorld*					world;
	// Defines both start and end
	Ray							ray;
	// Collision mask for filtering possible collisions
	physical::prCollisionMask	collision;
	// Object that the cast is originating from
	void*						owner;
	core::arBaseType			ownerType;
	// Maximum number of hits the cast can store
	// 0 refers to no limit.
	unsigned int				maxHits;

};
struct prShapecastQuery
{
	PrWorld*	world;
};

//	class PrCast : Physics casting interface.
// To use, create and pass in the query matching the desired information.
// The cast is performed in the constructor and results stored immediately.
// This class (safely) may be created on the stack or (dangerously) as a temporary object.
class PrCast
{
public:
	PHYS_API explicit		PrCast ( const prRaycastQuery& query );
	PHYS_API explicit		PrCast ( const prShapecastQuery& query );

	PHYS_API				~PrCast ( void );

	//	Hit() : Did this cast hit anything?
	PHYS_API bool			Hit ( void ) const;
	//	cast to bool : Returns Hit()
	PHYS_API				operator bool() const { return Hit(); }

	// Retrieves the hit type of the input.
	// Is optimized to be unsafe.
	template <int N>
	core::arBaseType		HitType ( void )
	{
		return m_hits[N].hitType;
	}

	template <int N, typename T>
	T*						HitObjectAs ( void )
	{
		return static_cast<T*>(m_hits[N].hitObject);
	}

	template <int N>
	btRigidBody*			HitRigidBody ( void )
	{
		return m_hits[N].hitBody;
	}


private:
	RaycastHit*		m_hits;
	unsigned int	m_hits_count;
};

#endif//PHYSICS_RP_CAST_H_