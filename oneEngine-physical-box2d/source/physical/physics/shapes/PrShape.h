#ifndef PHYSICAL_PHYSICS_SHAPES_PRSHAPE_
#define PHYSICAL_PHYSICS_SHAPES_PRSHAPE_

#include "core/types/types.h"

class btCollisionShape;

class PrShape
{
public:
	ENGINE_API btCollisionShape*	ApiShape ( void )
	{
		return shape;
	}
public:
	btCollisionShape*	shape;
};

#endif//PHYSICAL_PHYSICS_SHAPES_PRSHAPE_