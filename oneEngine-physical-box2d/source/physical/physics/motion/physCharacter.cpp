
#include "physCharacter.h"

_FORCE_INLINE_ PHYS_API physCharacter::physCharacter( hkpCharacterRigidBodyCinfo* info )
	: physRigidBody()
{
	/*controller = new hkpCharacterRigidBody( *info );
	Physics::AddEntity( controller->getRigidBody() );
	body = controller->m_character;*/
	throw Core::NotYetImplementedException();
}


_FORCE_INLINE_ PHYS_API void physCharacter::setLinearVelocity ( const Vector3d& newVel, const Real_32 timestep )
{
	/*if ( controller ) {
		controller->setLinearVelocity( hkVector4( newVel.x, newVel.y, newVel.z ), timestep );
	}
	else {
		physRigidBody::setLinearVelocity( newVel );
	}*/
	throw Core::NotYetImplementedException();
}
_FORCE_INLINE_ PHYS_API Vector3d physCharacter::getLinearVelocity ( void )
{
	/*if ( controller ) {
		Vector3d result;
		controller->getLinearVelocity().store3( &(result.x) );
		return result;
	}
	return physRigidBody::getLinearVelocity();*/
	throw Core::NotYetImplementedException();
}

