
#include "physCharacter.h"
#include "physCharacterRigidBodyInfo.h"

_FORCE_INLINE_ PHYS_API physCharacter::physCharacter( physCharacterRigidBodyInfo* info )
	: physRigidBody()
{
	hkpCharacterRigidBodyCinfo hkinfo;
	info->saveToHk(hkinfo);
	controller = new hkpCharacterRigidBody( hkinfo );
	Physics::AddEntity( controller->getRigidBody() );
	body = controller->m_character;
}


_FORCE_INLINE_ PHYS_API void physCharacter::setLinearVelocity ( const Vector3d& newVel, const Real_32 timestep )
{
	if ( controller ) {
		controller->setLinearVelocity( hkVector4( newVel.x, newVel.y, newVel.z ), timestep );
	}
	else {
		physRigidBody::setLinearVelocity( newVel );
	}
}
_FORCE_INLINE_ PHYS_API Vector3d physCharacter::getLinearVelocity ( void )
{
	if ( controller ) {
		Vector3d result;
		controller->getLinearVelocity().store3( &(result.x) );
		return result;
	}
	return physRigidBody::getLinearVelocity();
}

