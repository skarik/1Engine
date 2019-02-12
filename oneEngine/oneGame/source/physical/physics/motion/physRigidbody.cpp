
#include "physRigidbody.h"

_FORCE_INLINE_ PHYS_API physRigidBody::physRigidBody ( physRigidBodyInfo* info, bool isDynamic )
{
	//body = Physics::CreateRigidBody( info, isDynamic );
	body = Physics::CreateRigidBody( info, isDynamic );
	b2FixtureDef fixtureDef;
	fixtureDef.shape = info->m_shape->getShape();
	fixtureDef.friction = info->m_friction;
	fixtureDef.restitution = info->m_restitution;
	//fixtureDef.density = info->m_density;
	//fixtureDef.
	fixture = body->CreateFixture( &fixtureDef );
}
_FORCE_INLINE_ PHYS_API physRigidBody::~physRigidBody ( void )
{
	Physics::FreeRigidBody( body );
}


_FORCE_INLINE_ PHYS_API physCollidable* physRigidBody::getCollidableRw ( void )
{
	//return body->getCollidableRw();
	//fixture->GetBody();
	//return fixture;
	throw core::InvalidCallException();
}


_FORCE_INLINE_ PHYS_API void physRigidBody::setUserData ( const uint64_t userdata )
{
	//body->setUserData( (hkUlong)userdata );
	fixture->SetUserData( (void*)userdata );
}
_FORCE_INLINE_ PHYS_API uint64_t physRigidBody::getUserData ( void ) const
{
	//return body->getUserData();
	return (uint64_t)body->GetUserData();
}
_FORCE_INLINE_ PHYS_API void physRigidBody::setCollisionFilterInfo ( const b2Filter& bitmask )
{
	//body->setCollisionFilterInfo( bitmask );
	//b2Filter newfilter = fixture->GetFilterData();
	//newfilter.maskBits = bitmask;
	//fixture->SetFilterData( newfilter );
	fixture->SetFilterData( bitmask );
}
_FORCE_INLINE_ PHYS_API void physRigidBody::setMotionType ( const hkpMotion::MotionType motion )
{
	//body->setMotionType( motion );
	throw core::InvalidCallException();
}
_FORCE_INLINE_ PHYS_API hkpMotion::MotionType physRigidBody::getMotionType ( void ) const
{
	//return body->getMotionType();
	switch (body->GetType()) 
	{
	case b2_dynamicBody:
		return physMotion::MOTION_DYNAMIC;
	case b2_kinematicBody:
		return physMotion::MOTION_KEYFRAMED;
	case b2_staticBody:
		return physMotion::MOTION_FIXED;
	}
	return physMotion::MOTION_DYNAMIC;
}
_FORCE_INLINE_ PHYS_API void physRigidBody::setQualityType ( const hkpCollidableQualityType type )
{
	//body->setQualityType( type );
	throw core::InvalidCallException();
}
_FORCE_INLINE_ PHYS_API void physRigidBody::setAllowedPenetrationDepth ( const Real32 penetration )
{
	//body->setAllowedPenetrationDepth( penetration );
}
_FORCE_INLINE_ PHYS_API void physRigidBody::setShape ( const b2Shape* shape )
{
	//body->setShape(shape);
	throw core::InvalidCallException();
}


_FORCE_INLINE_ PHYS_API void physRigidBody::setMass ( const Real32 mass )
{
	//body->setMass( mass );
	b2MassData newmassdata;
	body->GetMassData( &newmassdata );
	newmassdata.mass = mass;
	body->SetMassData( &newmassdata );
}
_FORCE_INLINE_ PHYS_API Real32 physRigidBody::getMass ( void ) const
{
	return body->GetMass();
}
_FORCE_INLINE_ PHYS_API void physRigidBody::setGravityFactor ( const Real32 factor )
{
	body->SetGravityScale( factor );
}
_FORCE_INLINE_ PHYS_API Real32 physRigidBody::getGravityFactor ( void ) const
{
	return body->GetGravityScale();
}
_FORCE_INLINE_ PHYS_API void physRigidBody::setRestitution ( const Real32 restitution )
{
	fixture->SetRestitution( restitution );
}
_FORCE_INLINE_ PHYS_API Real32 physRigidBody::getRestitution ( void ) const
{
	return fixture->GetRestitution();
}
_FORCE_INLINE_ PHYS_API void physRigidBody::setFriction ( const Real32 friction )
{
	fixture->SetFriction( friction );
}
_FORCE_INLINE_ PHYS_API Real32 physRigidBody::getFriction ( void ) const
{
	return fixture->GetFriction();
}
_FORCE_INLINE_ PHYS_API void physRigidBody::setLinearDamping ( const Real32 damping )
{
	body->SetLinearDamping( damping );
}
_FORCE_INLINE_ PHYS_API void physRigidBody::setAngularDamping ( const Real32 damping )
{
	body->SetAngularDamping( damping );
}
_FORCE_INLINE_ PHYS_API void physRigidBody::setMaxAngularVelocity ( Real32 maxVel )
{
	//body->setMaxAngularVelocity( maxVel );
	//throw core::NotYetImplementedException();
}
_FORCE_INLINE_ PHYS_API Real32 physRigidBody::getMaxAngularVelocity ( void ) const
{
	//return body->getMaxAngularVelocity();
	//throw core::NotYetImplementedException();
	return INFINITY;
}


_FORCE_INLINE_ PHYS_API void physRigidBody::activate ( void )
{
	//body->activate();
	body->SetAwake(true);
}
_FORCE_INLINE_ PHYS_API void physRigidBody::applyForce ( const Real32 delta, const Vector3f& vect )
{
	//body->applyForce( delta, hkVector4( vect.x,vect.y,vect.z ) );
	Vector3f force = vect.mulComponents( Physics::WorldScaling() );
	body->ApplyForceToCenter( b2Vec2(force.x,force.y), true );
}
_FORCE_INLINE_ PHYS_API void physRigidBody::setIdentityRotation ( void )
{
	//body->setRotation( hkQuaternion::getIdentity() );
	body->SetTransform( body->GetPosition(), 0 );
}

_FORCE_INLINE_ PHYS_API void physRigidBody::setPosition ( const Vector3f& newPosition )
{
	//body->setPosition( hkVector4( newPosition.x, newPosition.y, newPosition.z ) );
	Vector3f position = newPosition.mulComponents( Physics::WorldScaling() );
	body->SetTransform( b2Vec2(position.x,position.y), body->GetAngle() );
}
_FORCE_INLINE_ PHYS_API const Vector3f physRigidBody::getPosition ( void ) const
{
	//Vector3f result;
	//body->getPosition().store3( &(result.x) );
	b2Vec2 pos = body->GetPosition();
	return Vector2f( pos.x, pos.y ).divComponents( Physics::WorldScaling() );
}
_FORCE_INLINE_ PHYS_API void physRigidBody::setRotation ( const Quaternion& newRotation )
{
	//body->setRotation( hkQuaternion( newRotation.x, newRotation.y, newRotation.z, newRotation.w ) );
	//body->SetTransform( body->GetPosition(), newRotation.GetEulerAngles().z );
	throw core::NotYetImplementedException();
}
_FORCE_INLINE_ PHYS_API const Quaternion physRigidBody::getRotation ( void ) const
{
	/*Quaternion result;
	body->getRotation().m_vec.store4( &(result.x) );
	return result;*/
	throw core::NotYetImplementedException();
}
_FORCE_INLINE_ PHYS_API void physRigidBody::setPositionAndRotation ( const Vector3f& position, const Quaternion& rotation )
{
	setPosition( position );
	setRotation( rotation );
}
_FORCE_INLINE_ PHYS_API void physRigidBody::setLinearVelocity ( const Vector3f& newVelocity )
{
	//body->setLinearVelocity( hkVector4( newVelocity.x, newVelocity.y, newVelocity.z ) );
	Vector3f velocity = newVelocity.mulComponents( Physics::WorldScaling() );
	body->SetLinearVelocity( b2Vec2( velocity.x, velocity.y ) );
}
_FORCE_INLINE_ PHYS_API const Vector3f physRigidBody::getLinearVelocity ( void ) const
{
	//Vector3f result;
	//body->getLinearVelocity().store3( &(result.x) );
	b2Vec2 velocity = body->GetLinearVelocity();
	return Vector2f( velocity.x, velocity.y ).divComponents( Physics::WorldScaling() );
}
_FORCE_INLINE_ PHYS_API void physRigidBody::setAngularVelocity ( const Vector4f& newVelocity )
{
	//body->setAngularVelocity( hkVector4( newVelocity.x,newVelocity.y,newVelocity.z,newVelocity.w ) );
}
_FORCE_INLINE_ PHYS_API const Vector4f physRigidBody::getAngularVelocity ( void ) const
{
	/*Vector4f result;
	body->getAngularVelocity().store4( &(result.x) );
	return result;*/
	throw core::NotYetImplementedException();
}


_FORCE_INLINE_ PHYS_API void physRigidBody::addContactListener ( physContactListener* listener )
{
	//body->addContactListener( listener );
	throw core::NotYetImplementedException();
}
_FORCE_INLINE_ PHYS_API void physRigidBody::removeContactListener ( physContactListener* listener )
{
	//body->removeContactListener( listener );
	throw core::NotYetImplementedException();
}


_FORCE_INLINE_ PHYS_API void physRigidBody::SetTransform ( core::Transform* transform )
{
	//Physics::SetRigidBodyTransform( body, transform );
	Vector2f position = transform->world.position.mulComponents( Physics::WorldScaling() );
	Real32 rotation = transform->world.rotation.getEulerAngles().z;
	body->SetTransform( b2Vec2(position.x,position.y), rotation );
}
_FORCE_INLINE_ PHYS_API void physRigidBody::GetTransform ( core::Transform* transform )
{
	//Physics::GetRigidBodyTransform( body, transform );
	GetTranslation(transform);
}
_FORCE_INLINE_ PHYS_API void physRigidBody::GetTranslation ( core::Transform* transform )
{
	//Physics::GetRigidBodyTranslation( body, transform );
	b2Vec2 position = body->GetPosition();
	transform->world.position = Vector2f( position.x, position.y ).divComponents( Physics::WorldScaling() );
}
_FORCE_INLINE_ PHYS_API void physRigidBody::ForcePropertyUpdate ( void )
{
	//CPhysics::ForceEntityUpdate( body );
	throw core::NotYetImplementedException();
}
