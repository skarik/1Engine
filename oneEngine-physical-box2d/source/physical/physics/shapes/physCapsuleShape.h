
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
		throw Core::NotYetImplementedException();
	}

	void setVertex ( const int vertex, const Vector3d& position )
	{
		throw Core::NotYetImplementedException();
	}
	void setRadius ( const Real radius )
	{
		throw Core::NotYetImplementedException();
	}

};


#endif//_PHYS_SHAPE_CAPSULE_