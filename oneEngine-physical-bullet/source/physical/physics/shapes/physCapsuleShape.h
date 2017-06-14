
#ifndef _PHYS_SHAPE_CAPSULE_
#define _PHYS_SHAPE_CAPSULE_

#include "physShape.h"
#include "physical/wrapper/wrapper_common.h"

// Interface for a physics shape

class physCapsuleShape : public physShape
{
public:
	explicit physCapsuleShape ( const Vector3d& n_min, const Vector3d& n_max, const Real radius )
	{
		throw core::NotYetImplementedException();
	}

	void setVertex ( const int vertex, const Vector3d& position )
	{
		throw core::NotYetImplementedException();
	}
	void setRadius ( const Real radius )
	{
		throw core::NotYetImplementedException();
	}

};


#endif//_PHYS_SHAPE_CAPSULE_