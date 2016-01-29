
#include "CVolumeTrigger.h"
#include "engine/physics/CPhysicsEngine.h"
#include "engine/behavior/CGameObject.h"

#include "physical/physics/motion/physTriggerVolume.h"
#include "physical/physics/motion/physRigidBodyInfo.h"

//==Constructor==
CVolumeTrigger::CVolumeTrigger ( CCollider* pTargetCollider, CGameObject * pOwnerGameObject )
	: CRigidBody ()
{
	// The rigidbody of course is active.
	bRigidBodyActive = true;

	// Save the collider
	pCollider = pTargetCollider;
	// Save the transform we want to edit
	pTargetTransform = &(pOwnerGameObject->transform);

	// Tell the collider who it's bitch to
	pCollider->SetRigidBody( this );

	// Create the body info
	physRigidBodyInfo info;
	//info.m_shape = pCollider->GetCollisionShape();		// Set the collision shape to the collider's
	//pBody = PhysicsEngine::CreateTriggerVolume( &info, pCollider->GetCollisionShape() ); // Create a trigger volume and assign it to the body variable.
	throw Core::NotYetImplementedException();
	pBody = new physTriggerVolume( &info, pCollider->GetCollisionShape(), NULL ); // Create a trigger volume and assign it to the body variable.
	pBody->setUserData( uint64_t(GetId()) );				// Give the rigid body the rigidbody's object index, so other objects may reference it on havok raycast results.
}

//==Destructor==
CVolumeTrigger::~CVolumeTrigger ( void )
{
	// Tell the physics engine that this rigidbody is no longer important.
	//Physics::FreeRigidBody( pBody );
	delete_safe( pBody );
}