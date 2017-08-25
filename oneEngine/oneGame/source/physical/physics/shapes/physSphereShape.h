
#ifndef _PHYS_SHAPE_SPHERE_
#define _PHYS_SHAPE_SPHERE_

#include "physShape.h"
#include "physical/wrapper/wrapper_common.h"

// Interface for a physics shape

#include "Box2D/Collision/Shapes/b2CircleShape.h"

class physSphereShape : public physShape
{
public:
	explicit physSphereShape ( const Real radius )
	{
		b2CircleShape* circleShape = new b2CircleShape;
		circleShape->m_radius = radius;
		m_shape = circleShape;
	}

	void setRadius ( const Real radius )
	{
		((b2CircleShape*)m_shape)->m_radius = radius;
	}
};

typedef physSphereShape physCircleShape;

#endif//_PHYS_SHAPE_SPHERE_