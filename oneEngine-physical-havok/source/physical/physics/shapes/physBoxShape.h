
#ifndef _PHYS_SHAPE_BOX_
#define _PHYS_SHAPE_BOX_

#include "core/types/types.h"
#include "core/math/Vector3d.h"
#include "physShape.h"
//#include "physical/wrapper/wrapper_common.h"

// Interface for a physics shape

//#include "Box2D/Collision/Shapes/b2PolygonShape.h"

class physBoxShape : public physShape
{
public:
	PHYS_API explicit physBoxShape ( const Vector3d& halfExtents, const Vector3d& centerOffset=Vector3d::zero );

	PHYS_API void setHalfExtents ( const Vector3d& halfExtents );
};

typedef physBoxShape physRectangleShape;

#endif//_PHYS_SHAPE_BOX_