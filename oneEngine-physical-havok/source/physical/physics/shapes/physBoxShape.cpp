
#include "physBoxShape.h"
#include "physical/physics.h"

physBoxShape::physBoxShape ( const Vector3d& halfExtents, const Vector3d& centerOffset )
{
	hkVector4 halfExtent ( halfExtents.x, halfExtents.y, halfExtents.z );
	hkpBoxShape* boxShape = new hkpBoxShape( halfExtent, 0.01f );

	if ( centerOffset.sqrMagnitude() > FTYPE_PRECISION )
	{
		// Create a translated version of the child shape
		hkVector4 translation;
		translation.set( centerOffset.x, centerOffset.y, centerOffset.z );
		hkpConvexTranslateShape* translatedBoxShape = new hkpConvexTranslateShape( boxShape, translation );
		boxShape->removeReference();

		m_shape = translatedBoxShape;
	}
	else
	{
		m_shape = boxShape;
	}
}

void physBoxShape::setHalfExtents ( const Vector3d& halfExtents )
{
	hkpBoxShape* boxShape = dynamic_cast<hkpBoxShape*>(m_shape);
	if ( boxShape )
	{
		hkVector4 halfExtent( halfExtents.x, halfExtents.y, halfExtents.z );
		boxShape->setHalfExtents( halfExtent );
	}
}