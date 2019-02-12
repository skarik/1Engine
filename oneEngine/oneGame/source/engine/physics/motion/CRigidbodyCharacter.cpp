#include <stdio.h>

#include "core/debug/CDebugConsole.h"
#include "core/time/time.h"

//#include "physical/physics/motion/physCharacter.h"
//#include "physical/physics/motion/physCharacterRigidBodyInfo.h"

#include "engine/behavior/CGameObject.h"

#include "CRigidbodyCharacter.h"

#ifdef PHYSICS_USING_HAVOK

/*class CRigidBodyCharacter::mContactModifier : public hkpContactListener
{
public:
	CRigidBodyCharacter*	m_rb;
	hkArray<hkContactPoint>	m_contacts;
	int						m_contactcount;

public:
	mContactModifier ( CRigidBodyCharacter* rb ) : m_rb(rb)
	{
		m_contactcount = 0;
	}

	void contactPointCallback( const hkpContactPointEvent& m_event ) override
	{
		//hkVisualDebugger::
		//HK_DISPLAY_ARROW( m_event.m_contactPoint->getPosition(), m_event.m_contactPoint->getNormal(), hkColor::RED );
		if ( m_contacts.getSize() > 50 ) {
			//m_contacts.removeAt(0);
			m_contacts[m_contactcount] = *m_event.m_contactPoint;
			m_contactcount += 1;
			if ( m_contactcount >= m_contacts.getSize() ) {
				m_contactcount = 0;
			}
		}
		else {
			m_contacts.pushBack( *m_event.m_contactPoint );
		}
	}

	void drawContactPoints ( void )
	{
		for ( int i = 0; i < m_contacts.getSize(); ++i )
		{
			HK_DISPLAY_ARROW( m_contacts[i].getPosition(), m_contacts[i].getNormal(), hkColor::RED );
		}
		//hkDebugDisplay::getInstance().
	}

};*/

//==Constructor==
CRigidBodyCharacter::CRigidBodyCharacter ( CCollider* pTargetCollider, CGameObject * pOwnerGameObject, float fMass )
	: CRigidbody(), /*mContactListener(NULL),*/ mCharController(NULL)
{
	// todo: move to CRigidbody constructor
	bGravityEnabled = true;

	// Set the layer
	layer = Layers::Rigidbody;

	// The rigidbody of course is active.
	bRigidBodyActive = true;

	// Save the collider
	pCollider = pTargetCollider;
	// Save the transform we want to edit
	pTargetTransform = &(pOwnerGameObject->transform);
	pTargetTransform->SetDirty(); // First frame is ALWAYS a dirty frame
	pOwner = pOwnerGameObject;

	// Tell the collider who it's bitch to
	pCollider->SetRigidBody( this );

	
	physCharacterRigidBodyInfo info;
	info.m_up = hkVector4(0,0,1);
	//info.m_maxSlope = 2.0944f; //60 degree slope
	info.m_maxSlope = 1.0472f; //60 degree slope
	//info.m_supportDistance = 0.083f; // 1 inch above ground for floating support
	info.m_hardSupportDistance = 0.083f * 0.5F; // 0.5 inch above ground for floating support
	info.m_supportDistance = info.m_hardSupportDistance + 0.083f*0.5F; // 1.0 inch above ground for support

	info.m_mass = fabs(fMass);
	info.m_shape = pCollider->GetCollisionShape();
	vCenterOfMass = pCollider->GetCenter();				// Get the collision shape's center of mass
	info.m_position = hkVector4( vCenterOfMass.x, vCenterOfMass.y, vCenterOfMass.z );
	info.m_maxLinearVelocity = 140;

	/*mCharController = new hkpCharacterRigidBody( info );
	//Physics::World()->addEntity( mCharController->getRigidBody() );
	Physics::AddEntity( mCharController->getRigidBody() );

	// set body
	pBody = mCharController->m_character;*/
	mCharController = new physCharacter( &info );
	mCharController->setUserData( GetId() );
	pBody = (physRigidBody*)mCharController;
	
	// Create the body info
	/*physRigidBodyInfo info;
	info.m_mass	= fabs(fMass);							// Set the body's mass
	info.m_shape = pCollider->GetCollisionShape();		// Set the collision shape to the collider's
	vCenterOfMass = pCollider->GetCenter();				// Get the collision shape's center of mass
	info.m_centerOfMass = hkVector4( vCenterOfMass.x, vCenterOfMass.y, vCenterOfMass.z );	
	info.m_friction	= 0.5f;								// Set the default friction value
	//info.m_motionType = physMotion::MOTION_CHARACTER;
	//info.m_motionType = physMotion::MOTION_DYNAMIC;
	info.m_restitution = 0.01f;
	info.m_qualityType = HK_COLLIDABLE_QUALITY_CHARACTER;
	// If mass is negative, change the motion

	pBody = Physics::CreateRigidBody( &info );			// Create a rigidbody and assign it to the body variable.
	pBody->setUserData( hkLong(GetId()) );				// Give the rigid body the rigidbody's object index, so other objects may reference it on havok raycast results.
	
	// Get the max angular velocity
	fMaxAngularVelocity = pBody->getMaxAngularVelocity();
	SetRotationEnabled( false );

	// Set the collision type to default Dynamic
	SetCollisionLayer( Layers::PHYS_CHARACTER );
	
	// Add a contact listener
	mContactListener = new mContactModifier( this );
	pBody->addContactListener( mContactListener );*/
}

CRigidBodyCharacter::~CRigidBodyCharacter ( void )
{
	//pBody = NULL;
	delete_safe(pBody);
	// todo (delete mContactListener)
}

void CRigidBodyCharacter::Update ( void )
{
	/*if ( mContactListener ) {
		mContactListener->drawContactPoints();
	}*/
}

// Accessors
physRigidBody* CRigidBodyCharacter::GetBody ( void )
{
	return mCharController;
}

//==Physics step==
// This is executed every physics step.
void CRigidBodyCharacter::RigidbodyUpdate ( void )
{
	if ( pBody && !mCharController )
	{
		if ( bRigidBodyActive )
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
						//pBody->setRotation( hkQuaternion::getIdentity() ); // Set rotation to nothing
						pBody->setIdentityRotation();
					}
				}
			}
		}
		else
		{
			// Disable rigidbody
		}
	}
	else if ( mCharController )
	{
		if ( bRigidBodyActive )
		{
			if ( pTargetTransform->IsDirty() )
			{
				//Physics::SetRigidBodyTransform( pBody, pTargetTransform );
				//Wake();
				/*hkVector4 tempVect ( pTargetTransform->position.x, pTargetTransform->position.y, pTargetTransform->position.z );
				mCharController->m_character->setPosition( tempVect );*/
				mCharController->setPosition( pTargetTransform->position );
			}
			else {
				//Physics::GetRigidBodyTransform( pBody, pTargetTransform );
				pTargetTransform->rotation = Matrix3x3();
				/*Vector3f temp3Vect;
				mCharController->getPosition().store3( &(temp3Vect.x) );
				pTargetTransform->position = temp3Vect;*/
				pTargetTransform->position = mCharController->getPosition();
			}
		}
		else
		{
			// Disable rigidbody
		}
	}
}

void CRigidBodyCharacter::FixedUpdate ( void )
{
	if ( mCharController )
	{
		// Addd gravviityyy :D
		if ( bGravityEnabled && mCharController->getSupportState() == 0 )
		{
			/*hkVector4 preVelocity = mCharController->getLinearVelocity();
			//hkVector4 gravity = Physics::World()->getGravity();
			Vector3f vgravity = Physics::GetWorldGravity();
			hkVector4 gravity ( vgravity.x, vgravity.y, vgravity.z );
			gravity.mul( Time::deltaTime );
			preVelocity.add( gravity );
			mCharController->setLinearVelocity( preVelocity, Time::deltaTime );*/
			Vector3f velocity = mCharController->getLinearVelocity();
			velocity += Physics::GetWorldGravity() * Time::deltaTime;
			mCharController->setLinearVelocity( velocity, Time::deltaTime );
		}
	}
}


// Setters
void		CRigidBodyCharacter::SetVelocity ( Vector3f nvelo )
{
	if ( mCharController ) {
		//mCharController->setLinearVelocity( hkVector4( nvelo.x, nvelo.y, nvelo.z ), Time::deltaTime );
		mCharController->setLinearVelocity( nvelo, Time::deltaTime );
	}
	else {
		CRigidbody::SetVelocity( nvelo );
	}
}
Vector3f	CRigidBodyCharacter::GetVelocity ( void )
{
	if ( mCharController ) {
		return mCharController->getLinearVelocity();
	}
	return CRigidbody::GetVelocity();
}

#endif