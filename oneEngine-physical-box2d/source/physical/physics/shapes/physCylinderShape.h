
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


#endif//_PHYS_SHAPE_CYLINDER_