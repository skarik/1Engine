
#include "physSphereShape.h"
#include "physical/physics.h"

physSphereShape::physSphereShape ( const Real radius )
{
	hkpSphereShape* sphereShape = new hkpSphereShape( radius );
	m_shape = sphereShape;
}