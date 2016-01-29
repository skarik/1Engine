
#ifndef _PHYS_SHAPE_SPHERE_
#define _PHYS_SHAPE_SPHERE_

#include "core/types/types.h"
#include "core/math/Vector3d.h"
#include "physShape.h"

// Interface for a physics shape

class physSphereShape : public physShape
{
public:
	PHYS_API explicit physSphereShape ( const Real radius );

	PHYS_API void setRadius ( const Real radius );
};

typedef physSphereShape physCircleShape;

#endif//_PHYS_SHAPE_SPHERE_