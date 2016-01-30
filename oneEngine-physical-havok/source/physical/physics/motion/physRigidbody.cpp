
#include "physRigidbody.h"

_FORCE_INLINE_ PHYS_API physRigidBody::physRigidBody ( physRigidBodyInfo* info, bool isDynamic )
{
	body = Physics::CreateRigidBody( info, isDynamic );
	if ( !body->isAddedToWorld() ) {
		throw Core::InvalidInstantiationException();
	}
}
_FORCE_INLINE_ PHYS_API physRigidBody::~physRigidBody ( void )
{
	Physics::FreeRigidBody( body );
}


_FORCE_INLINE_ PHYS_API hkpCollidable* physRigidBody::getCollidableRw ( void )
{
	return body->getCollidableRw();
}


_FORCE_INLINE_ PHYS_API void physRigidBody::setUserData ( const uint64_t userdata )
{
	body->setUserData( (hkUlong)userdata );
}
_FORCE_INLINE_ PHYS_API uint64_t physRigidBody::getUserData ( void ) const
{
	return body->getUserData();
}
_FORCE_INLINE_ PHYS_API void physRigidBody::setCollisionFilterInfo ( const uint32_t bitmask )
{
	body->setCollisionFilterInfo( bitmask );
}
_FORCE_INLINE_ PHYS_API void physRigidBody::setMotionType ( const hkpMotion::MotionType motion )
{
	body->setMotionType( motion );
}
_FORCE_INLINE_ PHYS_API hkpMotion::MotionType physRigidBody::getMotionType ( void ) const
{
	return body->getMotionType();
}
_FORCE_INLINE_ PHYS_API void physRigidBody::setQualityType ( const hkpCollidableQualityType type )
{
	body->setQualityType( type );
}
_FORCE_INLINE_ PHYS_API void physRigidBody::setAllowedPenetrationDepth ( const Real_32 penetration )
{
	body->setAllowedPenetrationDepth( penetration );
}
_FORCE_INLINE_ PHYS_API void physRigidBody::setShape ( const hkpShape* shape )
{
	body->setShape(shape);
	// Get the Read/Write copy of the rigidbody's collision info, and set the collision's new shape.
	getCollidableRw()->setShape(shape);
}


_FORCE_INLINE_ PHYS_API void physRigidBody::setMass ( const Real_32 mass )
{
	body->setMass( mass );
}
_FORCE_INLINE_ PHYS_API Real_32 physRigidBody::getMass ( void ) const
{
	return body->getMass();
}
_FORCE_INLINE_ PHYS_API void physRigidBody::setGravityFactor ( const Real_32 factor )
{
	body->setGravityFactor( factor );
}
_FORCE_INLINE_ PHYS_API Real_32 physRigidBody::getGravityFactor ( void ) const
{
	return body->getGravityFactor();
}
_FORCE_INLINE_ PHYS_API void physRigidBody::setRestitution ( const Real_32 restitution )
{
	body->setRestitution( restitution );
}
_FORCE_INLINE_ PHYS_API Real_32 physRigidBody::getRestitution ( void ) const
{
	return body->getRestitution();
}
_FORCE_INLINE_ PHYS_API void physRigidBody::setFriction ( const Real_32 friction )
{
	body->setFriction( friction );
}
_FORCE_INLINE_ PHYS_API Real_32 physRigidBody::getFriction ( void ) const
{
	return body->getFriction();
}
_FORCE_INLINE_ PHYS_API void physRigidBody::setLinearDamping ( const Real_32 damping )
{
	body->setLinearDamping( damping );
}
_FORCE_INLINE_ PHYS_API void physRigidBody::setAngularDamping ( const Real_32 damping )
{
	body->setAngularDamping( damping );
}
_FORCE_INLINE_ PHYS_API void physRigidBody::setMaxAngularVelocity ( Real_32 maxVel )
{
	body->setMaxAngularVelocity( maxVel );
}
_FORCE_INLINE_ PHYS_API Real_32 physRigidBody::getMaxAngularVelocity ( void ) const
{
	return body->getMaxAngularVelocity();
}


_FORCE_INLINE_ PHYS_API void physRigidBody::activate ( void )
{
	body->activate();
}
_FORCE_INLINE_ PHYS_API void physRigidBody::applyForce ( const Real_32 delta, const Vector3d& vect )
{
	body->applyForce( delta, hkVector4( vect.x,vect.y,vect.z ) );
}
_FORCE_INLINE_ PHYS_API void physRigidBody::setIdentityRotation ( void )
{
	body->setRotation( hkQuaternion::getIdentity() );
}

_FORCE_INLINE_ PHYS_API void physRigidBody::setPosition ( const Vector3d& newPosition )
{
	body->setPosition( hkVector4( newPosition.x, newPosition.y, newPosition.z ) );
}
_FORCE_INLINE_ PHYS_API const Vector3d physRigidBody::getPosition ( void ) const
{
	Vector3d result;
	body->getPosition().store3( &(result.x) );
	return result;
}
_FORCE_INLINE_ PHYS_API void physRigidBody::setRotation ( const Quaternion& newRotation )
{
	body->setRotation( hkQuaternion( newRotation.x, newRotation.y, newRotation.z, newRotation.w ) );
}
_FORCE_INLINE_ PHYS_API const Quaternion physRigidBody::getRotation ( void ) const
{
	Quaternion result;
	body->getRotation().m_vec.store4( &(result.x) );
	return result;
}
_FORCE_INLINE_ PHYS_API void physRigidBody::setPositionAndRotation ( const Vector3d& position, const Quaternion& rotation )
{
	setPosition( position );
	setRotation( rotation );
}
_FORCE_INLINE_ PHYS_API void physRigidBody::setLinearVelocity ( const Vector3d& newVelocity )
{
	body->setLinearVelocity( hkVector4( newVelocity.x, newVelocity.y, newVelocity.z ) );
}
_FORCE_INLINE_ PHYS_API const Vector3d physRigidBody::getLinearVelocity ( void ) const
{
	Vector3d result;
	body->getLinearVelocity().store3( &(result.x) );
	return result;
}
_FORCE_INLINE_ PHYS_API void physRigidBody::setAngularVelocity ( const Vector4d& newVelocity )
{
	body->setAngularVelocity( hkVector4( newVelocity.x,newVelocity.y,newVelocity.z,newVelocity.w ) );
}
_FORCE_INLINE_ PHYS_API const Vector4d physRigidBody::getAngularVelocity ( void ) const
{
	Vector4d result;
	body->getAngularVelocity().store4( &(result.x) );
	return result;
}


_FORCE_INLINE_ PHYS_API void physRigidBody::addContactListener ( hkpContactListener* listener )
{
	body->addContactListener( listener );
}
_FORCE_INLINE_ PHYS_API void physRigidBody::removeContactListener ( hkpContactListener* listener )
{
	body->removeContactListener( listener );
}


_FORCE_INLINE_ PHYS_API void physRigidBody::SetTransform ( CTransform* transform )
{
	Physics::SetRigidBodyTransform( body, transform );
}
_FORCE_INLINE_ PHYS_API void physRigidBody::GetTransform ( CTransform* transform )
{
	Physics::GetRigidBodyTransform( body, transform );
}
_FORCE_INLINE_ PHYS_API void physRigidBody::GetTranslation ( CTransform* transform )
{
	Physics::GetRigidBodyTranslation( body, transform );
}
_FORCE_INLINE_ PHYS_API void physRigidBody::ForcePropertyUpdate ( void )
{
	CPhysics::ForceEntityUpdate( body );
}
