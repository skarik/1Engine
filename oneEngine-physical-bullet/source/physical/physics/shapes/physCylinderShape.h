
#ifndef _PHYS_SHAPE_CYLINDER_
#define _PHYS_SHAPE_CYLINDER_

#include "physShape.h"
#include "physical/wrapper/wrapper_common.h"

// Interface for a physics shape

class physCylinderShape : public physShape
{
public:
	explicit physCylinderShape ( const Vector3d& n_min, const Vector3d& n_max, const Real radius )
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


#endif//_PHYS_SHAPE_CYLINDER_