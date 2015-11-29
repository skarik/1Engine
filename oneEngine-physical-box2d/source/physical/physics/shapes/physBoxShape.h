
#ifndef _PHYS_SHAPE_BOX_
#define _PHYS_SHAPE_BOX_

#include "physShape.h"
#include "physical/wrapper/wrapper_common.h"

// Interface for a physics shape

#include "Box2D/Collision/Shapes/b2PolygonShape.h"

class physBoxShape : public physShape
{
public:
	PHYS_API explicit physBoxShape ( const Vector3d& halfExtents );

	PHYS_API void setHalfExtents ( const Vector3d& halfExtents );
};

typedef physBoxShape physRectangleShape;

#endif//_PHYS_SHAPE_BOX_