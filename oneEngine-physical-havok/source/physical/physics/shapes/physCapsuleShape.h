#ifndef _PHYS_SHAPE_CAPSULE_
#define _PHYS_SHAPE_CAPSULE_

#include "core/types/types.h"
#include "core/math/Vector3d.h"
#include "physShape.h"
//#include "physical/wrapper/wrapper_common.h"

// Interface for a physics shape

class physCapsuleShape : public physShape
{
public:
	PHYS_API explicit physCapsuleShape ( const Vector3d& n_min, const Vector3d& n_max, const Real radius );

	PHYS_API void setVertex ( const int vertex, const Vector3d& position );
	PHYS_API void setRadius ( const Real radius );

};


#endif//_PHYS_SHAPE_CAPSULE_