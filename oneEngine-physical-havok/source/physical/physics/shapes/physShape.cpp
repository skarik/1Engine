
#include "physShape.h"
#include "physical/physics.h"

#ifdef PHYSICS_USING_HAVOK

physShape::~physShape ()
{
	if ( m_shape != NULL ) {
		m_shape->removeReference();
		m_shape = NULL;
	}
}

// Explicit shape get
hkpShape* physShape::getShape ( void )
{
	return m_shape;
}
const hkpShape* physShape::getShape ( void ) const
{
	return m_shape;
}
// Non-explicit shape get
physShape::operator const hkpShape* ()
{
	return m_shape;
}

#endif