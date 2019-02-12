
#include "physBoxShape.h"

physBoxShape::physBoxShape ( const Vector3f& halfExtents )
{
	b2PolygonShape* boxShape = new b2PolygonShape;
	boxShape->SetAsBox( halfExtents.x,halfExtents.y );
	m_shape = boxShape;
}
physBoxShape::physBoxShape ( const Vector3f& halfExtents, const Vector3f& center )
{
	b2PolygonShape* boxShape = new b2PolygonShape;
	boxShape->SetAsBox( halfExtents.x,halfExtents.y, b2Vec2(center.x,center.y), 0 );
	m_shape = boxShape;
}

void physBoxShape::setHalfExtents ( const Vector3f& halfExtents )
{
	((b2PolygonShape*)m_shape)->SetAsBox( halfExtents.x, halfExtents.y );
}