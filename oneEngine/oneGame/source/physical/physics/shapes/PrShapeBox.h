#ifndef PHYSICAL_PHYSICS_SHAPES_PRSHAPE_BOX_
#define PHYSICAL_PHYSICS_SHAPES_PRSHAPE_BOX_

#include "core/math/Math3d.h"
#include "physical/physics/shapes/PrShape.h"

class PrShapeBox : public PrShape
{
public:
	PHYS_API explicit		PrShapeBox ( const Vector3f& centered_box_size );

	PHYS_API				~PrShapeBox ( void );
};

#endif//PHYSICAL_PHYSICS_SHAPES_PRSHAPE_BOX_