
#include "physCylinderShape.h"
#include "physical/physics.h"

physCylinderShape::physCylinderShape ( const Vector3d& n_min, const Vector3d& n_max, const Real radius )
{
	hkVector4 start( n_min.x, n_min.y, n_min.z );
	hkVector4 end  ( n_max.x, n_max.y, n_max.z );
	hkpCylinderShape* capsuleShape = new hkpCylinderShape( start, end, radius ); // creates a capsule with a axis between "start" and "end", and the specified "radius"
	m_shape = capsuleShape;
}

void physCylinderShape::setVertex ( const int vertex, const Vector3d& position )
{
	hkVector4 vertpos( position.x, position.y, position.z );
	static_cast<hkpCylinderShape*>(m_shape)->setVertex( vertex, vertpos );
}
void physCylinderShape::setRadius ( const Real radius )
{
	static_cast<hkpCylinderShape*>(m_shape)->setRadius( radius );
}