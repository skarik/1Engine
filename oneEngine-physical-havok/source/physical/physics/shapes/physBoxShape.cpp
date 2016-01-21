
#include "physBoxShape.h"
#include "physical/physics.h"

physBoxShape::physBoxShape ( const Vector3d& halfExtents, const Vector3d& centerOffset )
{
	b2PolygonShape* boxShape = new b2PolygonShape;
	boxShape->SetAsBox( halfExtents.x,halfExtents.y );
	m_shape = boxShape;
}

void physBoxShape::setHalfExtents ( const Vector3d& halfExtents )
{
	((b2PolygonShape*)m_shape)->SetAsBox( halfExtents.x, halfExtents.y );
}