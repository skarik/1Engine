
#include "physical/physics.h"

#include "physContactEvents.h"


physRigidBody* physContactPointEvent::getBody ( int index )
{
	throw core::NotYetImplementedException();
	/*if ( index == 0 ) return m_contactInfo->GetFixtureA();
	else if ( index == 1 ) return m_contactInfo->GetFixtureB();*/
}
const physRigidBody* physContactPointEvent::getBody ( int index ) const
{
	throw core::NotYetImplementedException();
	/*if ( index == 0 ) return m_contactInfo->GetFixtureA();
	else if ( index == 1 ) return m_contactInfo->GetFixtureB();*/
}