#ifndef PHYSICAL_PHYSICS_FLUID_PR_WATER_QUERY_H_
#define PHYSICAL_PHYSICS_FLUID_PR_WATER_QUERY_H_

#include "core/math/Math2d.h"
#include "core/math/Math3d.h"
#include "core-ext/types/baseType.h"
#include "physical/types/collisionMask.h"
#include "physical/physics/cast/RaycastHit.h"

class PrWorld;

namespace physical
{
	namespace water
	{
		//	struct prInside : water position query
		// Used to check if a position is inside water.
		// Query will be true if position is inside water.
		struct prInside
		{
			// Which world to check? NULL for current world.
			PrWorld*	world;
			// Position to check.
			Vector3d	position;

			// Shorthand check constructor
			prInside ( const Vector3d& n_position )
				: world(NULL), position(n_position)
			{
				;
			}
		};
	}
}

//	class PrWaterQuery : Physics fluid casting interface.
// To use, create and pass in the query matching the desired information.
// The cast is performed in the constructor and results stored immediately.
// This class (safely) may be created on the stack or (dangerously) as a temporary object.
// Compare with class PrCast.
class PrWaterQuery
{
public:
	PHYS_API explicit		PrWaterQuery ( const physical::water::prInside& query );

	PHYS_API				~PrWaterQuery ( void );

	//	cast to bool : Returns simplified query result.
	PHYS_API				operator bool() const;

private:
	bool	hit;
};

#endif//PHYSICAL_PHYSICS_FLUID_PR_WATER_QUERY_H_