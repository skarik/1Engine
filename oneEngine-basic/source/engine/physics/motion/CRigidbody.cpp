

#include "CRigidbody.h"

#include "core/debug/CDebugConsole.h"
#include <stdio.h>
#include "core/time/time.h"
#include "core/exceptions/exceptions.h"

#include "engine/state/CGameState.h"
#include "engine/behavior/CGameObject.h"

#include "physical/physics/motion/physRigidbody.h"
#include "physical/physics/motion/physRigidBodyInfo.h"

//#include "CRagdollCollision.h"
class CRagdollCollision;

//==Collision Listener==
class CRigidbody::mCollisionListener : public physContactListener
{
public:
	CRigidbody*	m_rb;

public:
	mCollisionListener ( CRigidbody* rb ) : m_rb(rb)
	{
		;
	}

	void contactPointCallback( const physContactPointEvent& m_event ) override
	{
		sCollision n_collision;
		physVector4 t_hitPos = m_event.m_contactPoint->getPosition();
		physVector4 t_hitNorm = m_event.m_contactPoint->getNormal();
		n_collision.m_hit.pos = Vector3d( t_hitPos.getComponent<0>(),t_hitPos.getComponent<1>(),t_hitPos.getComponent<2>() );
		n_collision.m_hit.dir = Vector3d( t_hitNorm.getComponent<0>(),t_hitNorm.getComponent<1>(),t_hitNorm.getComponent<2>() );

		n_collision.m_collider_Other	= NULL;
		n_collision.m_hit_Other			= NULL;
		n_collision.m_collider_This		= NULL;
		n_collision.m_hit_This			= NULL;

		uint64_t t_hitUserdata = 0;
		CGameBehavior* behavior = NULL;

		// Grab collision of object 0
		t_hitUserdata = m_event.getBody(0)->getUserData();
		behavior = CGameState::Active()->GetBehavior( (gameid_t)t_hitUserdata );
		if ( behavior && behavior->GetTypeName() == "CRagdollCollision" ) {
			//n_collision.m_hit_This = (CGameBehavior*) (((CRagdollCollision*)behavior)->GetActor());
			n_collision.m_hit_This = (CGameBehavior*)(CRagdollCollision*)behavior;
		}
		else if ( behavior && behavior->GetTypeName() == "CRigidbody" ) {
			//n_collision.m_hit_This = ((CRigidbody*)behavior)->GetOwner();
			n_collision.m_hit_This = (CRigidbody*)behavior;
			n_collision.m_collider_This = ((CRigidbody*)behavior)->pCollider;
		}

		// Grab collision of object 1
		t_hitUserdata = m_event.getBody(1)->getUserData();
		behavior = CGameState::Active()->GetBehavior( (gameid_t)t_hitUserdata );
		if ( behavior && behavior->GetTypeName() == "CRagdollCollision" ) {
			//n_collision.m_hit_Other = (CGameBehavior*) (((CRagdollCollision*)behavior)->GetActor());
			n_collision.m_hit_Other = (CGameBehavior*)(CRagdollCollision*)behavior;
		}
		else if ( behavior && behavior->GetTypeName() == "CRigidbody" ) {
			//n_collision.m_hit_Other = ((CRigidbody*)behavior)->GetOwner();
			n_collision.m_hit_Other = (CRigidbody*)behavior;
			n_collision.m_collider_Other = ((CRigidbody*)behavior)->pCollider;
		}

		// Swap the collisions to give match to Other and This
		if ( n_collision.m_collider_Other == m_rb->pCollider )
		{
			std::swap( n_collision.m_hit_Other, n_collision.m_hit_This );
			std::swap( n_collision.m_collider_Other, n_collision.m_collider_This );
		}
		else if ( n_collision.m_collider_This != m_rb->pCollider )
		{
			throw core::CorruptedDataException();
		}

		// Apply the event
		m_rb->pCollider->OnCollide( n_collision );
	}
};

//==Constructor==
CRigidbody::CRigidbody ( CCollider* body_collider, CGameBehavior * owner_behavior, float mass )
	: CMotion(owner_behavior),
	target_transform(NULL), target_position(NULL),
	pBody(NULL), bGravityEnabled(true)
{
	// Set the layer
	layer = Layers::Rigidbody;

	// The rigidbody of course is active.
	bRigidBodyActive = true;
	bRotationEnabled = true;

	// Save the collider
	pCollider = body_collider;

	// Tell the collider who it's bitch to
	pCollider->SetRigidBody( this );

	// Create the body info
	physRigidBodyInfo info;
	info.m_mass	= fabs(mass);							// Set the body's mass
	info.m_shape = pCollider->GetCollisionShape();		// Set the collision shape to the collider's
	vCenterOfMass = pCollider->GetCenter();				// Get the collision shape's center of mass
	info.m_centerOfMass = physVector4( vCenterOfMass.x, vCenterOfMass.y, vCenterOfMass.z );	
	info.m_friction	= 0.5f;								// Set the default friction value
	info.m_collisionFilterInfo = Physics::GetCollisionFilter( Layers::PHYS_DYNAMIC );
	// If mass is negative, change the motion
	if ( mass < 0.0F )
	{
		info.m_motionType = physMotion::MOTION_FIXED;
	}

	//pBody = Physics::CreateRigidBody( &info );			// Create a rigidbody and assign it to the body variable.
	pBody = new physRigidBody( &info );
	pBody->setUserData( long(GetId()) );				// Give the rigid body the rigidbody's object index, so other objects may reference it on havok raycast results.

	// Get the max angular velocity
	fMaxAngularVelocity = pBody->getMaxAngularVelocity();

	// Set the collision type to default Dynamic
	//SetCollisionLayer( Layers::PHYS_DYNAMIC );

	m_listener = NULL;
}

//==Destructor==
CRigidbody::~CRigidbody ( void )
{
	// Tell the physics engine that this rigidbody is no longer important.
	if ( pBody ) {
		/*if ( m_listener ) {
			pBody->removeContactListener( m_listener );
		}*/
		//Physics::FreeRigidBody( pBody );
		delete pBody;
	}
	/*if ( m_listener ) {
		delete m_listener;
	}*/
}

// Accessors
physRigidBody* CRigidbody::GetBody ( void ) 
{
	return pBody;
}

void CRigidbody::EnableCollisionCallback ( void )
{
	if ( !m_listener )
	{
		// Add a contact listener
		m_listener = new mCollisionListener( this );
		pBody->addContactListener( m_listener );
	}
}
void CRigidbody::DisableCollisionCallback ( void )
{
	if ( m_listener )
	{
		pBody->removeContactListener( m_listener );
		delete m_listener;
		m_listener = NULL;
	}
}

//==Physics step==
// This is executed every physics step.
void CRigidbody::RigidbodyUpdate ( void )
{
	/*if ( bRigidBodyActive && pTargetTransform )
	{
		if ( pBody->getMotionType() == physMotion::MOTION_KEYFRAMED )
		{
			//Physics::SetRigidBodyTransform( pBody, pTargetTransform );
			pBody->SetTransform( pTargetTransform );
		}
		else
		{
			// If the transform has been modified outside of the rigid body, then output transform to pTargetTransform
			// The flag for a modified transform must be set or the physics engine will not recieve the position of the object.
			if ( pTargetTransform->IsDirty() )
			{
				//Physics::SetRigidBodyTransform( pBody, pTargetTransform );
				pBody->SetTransform( pTargetTransform );
				Wake();
			}
			else
			{
				if ( bRotationEnabled )
				{
					// This copies the rigidbody transform to the engine's transform.
					//Physics::GetRigidBodyTransform( pBody, pTargetTransform );
					pBody->GetTransform( pTargetTransform );
				}
				else
				{
					//Physics::GetRigidBodyTranslation( pBody, pTargetTransform );
					pBody->GetTranslation( pTargetTransform );
				}
			}
		}
	}
	else
	{
		// Disable rigidbody
	}*/
	if ( bRigidBodyActive )
	{
		switch (pBody->getMotionType())
		{
		case physMotion::MOTION_FIXED:
			// Nobody cares.
			break;
		case physMotion::MOTION_KEYFRAMED:
			// Keyframed? Pull data from the target.
			if ( target_transform != NULL )
			{
				pBody->SetTransform( target_transform );
			}
			else if ( target_position != NULL )
			{
				pBody->setPosition( *target_position );
			}
			break;
		default:
			// Otherwise, data goes from rigidbody to target.
			if ( target_transform != NULL )
			{
				if ( bRotationEnabled )
					pBody->GetTransform( target_transform );
				else
					pBody->GetTranslation( target_transform );
			}
			else if ( target_position != NULL )
			{
#ifdef PHYSICS_USING_BOX2D
				Vector3d position = pBody->getPosition();
				target_position->x = position.x;
				target_position->y = position.y;
#else
				*target_position = pBody->getPosition();
#endif
			}
			break;
		}
	}
	else
	{
		// Disable rigidbody
	}
}


//===============================================================================================//
// GETTERS AND SETTERS (PHYSICS ENGINE WRAPPER)
//===============================================================================================//

// Owner
CGameBehavior* CRigidbody::GetOwner ( void )
{
	if ( owner ) {
		return owner;
	}
	else if ( target_transform )
	{
		if ( target_transform->owner && target_transform->ownerType == Transform::TYPE_BEHAVIOR )
		{
			return (CGameBehavior*)target_transform->owner;
		}
	}
	return NULL;
}

// Mass Properties
void CRigidbody::SetMass ( float mass )
{
	pBody->setMass( mass );
}
float CRigidbody::GetMass ( void )
{
	return (float)(pBody->getMass());
}

// Changes the instantaneous velocity of the rigidbody
void CRigidbody::SetVelocity ( Vector3d newVelocity )
{
	pBody->setLinearVelocity( newVelocity );
}
// Returns the current velocity of the rigidbody
Vector3d CRigidbody::GetVelocity ( void )
{
	return pBody->getLinearVelocity();
}
void CRigidbody::SetAcceleration ( Vector3d )
{
	//pBody->set
}
Vector3d CRigidbody::GetAcceleration ( void )
{
	return Vector3d::zero;
}
// Enables or disables gravity
void CRigidbody::SetGravity ( bool bIn )
{
	bGravityEnabled = bIn;
	pBody->setGravityFactor( bGravityEnabled ? 1.0F : 0.0F );
}
// Returns if gravity is significantly enabled for the object
bool CRigidbody::GetGravity ( void )
{
	//pBody->setGravityFactor( 1 );
	return ( pBody->getGravityFactor() > 0.05f );
}
// Sets the current position of the rigidbody
void CRigidbody::SetPosition ( Vector3d newPosition )
{
	pBody->setPosition( newPosition );
}
// Returns the current position of the rigidbody
Vector3d CRigidbody::GetPosition ( void )
{
	return pBody->getPosition();
}
// Adds the given vector to the current position and then returns the new position
Vector3d CRigidbody::AddToPosition ( Vector3d posAdd )
{
	/*Vector3d result;
	pBody->getPosition().store3( &(result.x) );
	result += posAdd;
	pBody->setPosition( hkVector4( result.x, result.y, result.z ) );
	return result;*/
	Vector3d result = pBody->getPosition() + posAdd;
	pBody->setPosition( result );
	return result;
}
void CRigidbody::SetRotation ( Quaternion newRotation )
{
	pBody->setRotation( newRotation );
}
Quaternion CRigidbody::GetRotation ( void )
{
	return pBody->getRotation();
}
// This changes the motion type.
// Available values are found inside physMotionType. Common valid values include
//		physMotionType::MOTION_DYNAMIC		physically simulated
//		physMotionType::MOTION_FIXED		static object
//		physMotionType::MOTION_KEYFRAMED	moving/static but not physically simulated
void CRigidbody::SetMotionType ( physMotionType newMotionType )
{
	pBody->setMotionType( newMotionType );
}
// This changes the motion quality type.
// Available values are found inside physMotionQualityType. Common values include
//		PhysicsQualityDefault			= HK_COLLIDABLE_QUALITY_INVALID,
//		PhysicsQualityDebris			= HK_COLLIDABLE_QUALITY_DEBRIS,
//		PhysicsQualityDebris_SimpleTOI	= HK_COLLIDABLE_QUALITY_DEBRIS_SIMPLE_TOI,
//		PhysicsQualityMoving			= HK_COLLIDABLE_QUALITY_MOVING,
//		PhysicsQualityCritical			= HK_COLLIDABLE_QUALITY_CRITICAL,
//		PhysicsQualityBullet			= HK_COLLIDABLE_QUALITY_BULLET,
//		PhysicsQualityCharacter			= HK_COLLIDABLE_QUALITY_CHARACTER
void CRigidbody::SetQualityType ( physMotionQualityType newQualityType )
{
	Physics::ThreadLock();
	pBody->setQualityType( newQualityType );
	Physics::ThreadUnlock();
}
// This adds an impulse with the current deltaTime.
// This needs to be tested to make sure the current timestep works and lag doesn't change the size of the impulse.
void CRigidbody::ApplyForce ( Vector3d push )
{
	//pBody->applyForce( Time::deltaTime, hkVector4( push.x,push.y,push.z ) );
	pBody->applyForce( Time::fixedTime*10.0f, push );
}

// Movement Property Settings

// Set the restitution. This controls how bouncy the object is.
//   The default value is 0.4. To main something bounce forever, set the value to 1 (one).
//   For full energy absorption, set restitution to 0 (zero).
void CRigidbody::SetRestitution ( float fRestitution )
{
	Physics::ThreadLock();
	pBody->setRestitution( fRestitution );
	Physics::ThreadUnlock();
}

// Set the friction. This controls how rough the surface of the object is.
//   The default value is 0.5. To emulate something with a rougher surface, increase the friction.
void CRigidbody::SetFriction ( float fFriction )
{
	Physics::ThreadLock();
	pBody->setFriction( fFriction );
	Physics::ThreadUnlock();
}
float CRigidbody::GetFriction ( void )
{
	return pBody->getFriction();
}

// Set the linear damping. This controls approximately what percentage of an object's velocity is removed each second
//   For example, if set to 0.1, the velocity will be reduced by about 10 percent each second.
//   The default value is 0
void CRigidbody::SetLinearDamping ( float fLinearDamping )
{
	Physics::ThreadLock();
	pBody->setLinearDamping( fLinearDamping );
	Physics::ThreadUnlock();
}
// Set the angular damping. No idea what exactly this works with, but it slows stuff down.
//   The default value is 0.05
void CRigidbody::SetAngularDamping ( float fAngularDamping )
{
	Physics::ThreadLock();
	pBody->setAngularDamping( fAngularDamping );
	Physics::ThreadUnlock();
}

// Set if rotation is enabled
void CRigidbody::SetRotationEnabled ( bool inRotationEnabled )
{
	bRotationEnabled = inRotationEnabled;
	if ( bRotationEnabled ) {
		pBody->setMaxAngularVelocity( fMaxAngularVelocity );
	}
	else {
		pBody->setMaxAngularVelocity( 0 );
	}
}

// Wakes up the rigidbody
void CRigidbody::Wake ( void )
{
	pBody->activate();
}

// Collision Settings
// Set new Rigidbody shape. This does NOT re-calculate the mass or inertia data.
void CRigidbody::SetShape ( physShape* pNewShape )
{
	// Get the Read/Write copy of the rigidbody's collision info, and set the collision's new shape.
	//pBody->getCollidableRw()->setShape( pNewShape );
	pBody->setShape( pNewShape->getShape() ); // Code is moved into physRigidbody
}

// Set new minimum penetration depth.
// The default value is 0.05, which works for objects down to size of 0.2
// Decreasing this value will allow for better simulation, but at a cost of speed.
void CRigidbody::SetPenetrationDepth ( float fPenetrationDepth )
{
	pBody->setAllowedPenetrationDepth( fPenetrationDepth );
}

// Set new collision layer
void CRigidbody::SetCollisionLayer ( Layers::PhysicsTypes layer, int subsystem )
{
	pBody->setCollisionFilterInfo( CPhysics::GetCollisionFilter( layer, subsystem ) );
	pBody->ForcePropertyUpdate(); //calls CPhysics::ForceEntityUpdate( pBody );
//	CPhysics::pWorld->updateCollisionFilterOnEntity( pBody, HK_UPDATE_FILTER_ON_ENTITY_DISABLE_ENTITY_ENTITY_COLLISIONS_ONLY, HK_UPDATE_COLLECTION_FILTER_IGNORE_SHAPE_COLLECTIONS ); //todo
	//CPhysics::World()->updateCollisionFilterOnEntity( pBody, HK_UPDATE_FILTER_ON_ENTITY_FULL_CHECK, HK_UPDATE_COLLECTION_FILTER_PROCESS_SHAPE_COLLECTIONS ); //todo
	
}



