// If you want to create a static mesh easily, just pass a negative mass into the constructor.

#ifndef _C_RIGID_BODY_H_
#define _C_RIGID_BODY_H_

#include "physical/physics/CPhysics.h"
#include "engine/physics/collider/CCollider.h"
#include "engine/behavior/CGameBehavior.h"
#include "engine/physics/material/physMaterial.h"
#include "engine/physics/motion/CMotion.h"

class physRigidBody;

class CRigidBody : public CMotion
{
	ClassName( "CRigidbody" );
protected:
	explicit CRigidBody () : CMotion(), pBody(NULL) {};
public:
	// Regular constructor
	ENGINE_API explicit CRigidBody ( CCollider* pTargetCollider, CGameObject * pOwnerGameObject, float fMass=1.0f );
	// Destructor
	ENGINE_API ~CRigidBody ( void );

	//==Game step==
	virtual void Update ( void ) {};
	virtual void LateUpdate ( void ) {};
	
	//==Physics step==
	void RigidbodyUpdate ( void );

	//==Setters and Getters==
	// Get the target transform this rigidbody is editing
	CTransform* GetTargetTransform ( void ) { return pTargetTransform; }
	// Set the target transform this rigidbody is editing
	void SetTargetTransform ( CTransform* nTransform ) { pTargetTransform = nTransform; }
	// Mass Properties
	ENGINE_API void	SetMass ( float=1.0f );
	ENGINE_API float	GetMass ( void );
	// Movement setters
	ENGINE_API virtual void		SetVelocity ( Vector3d );
	ENGINE_API virtual Vector3d	GetVelocity ( void );
	ENGINE_API virtual void		SetAcceleration ( Vector3d );
	ENGINE_API virtual Vector3d	GetAcceleration ( void );
	ENGINE_API void	SetGravity ( bool );
	ENGINE_API bool	GetGravity ( void );
	ENGINE_API void	ApplyForce ( Vector3d );
	ENGINE_API void		SetPosition ( Vector3d );
	ENGINE_API Vector3d	GetPosition ( void );
	ENGINE_API Vector3d	AddToPosition ( Vector3d );
	ENGINE_API void		SetRotation ( Quaternion );
	ENGINE_API Quaternion	GetRotation ( void );
	// Movement property setters
	ENGINE_API void SetRestitution ( float=0.4f );
	ENGINE_API void	SetFriction ( float=0.5f );
	ENGINE_API float	GetFriction ( void );
	ENGINE_API void SetLinearDamping ( float=0.0f );
	ENGINE_API void SetAngularDamping ( float=0.05f );
	// Motion Settings
	ENGINE_API void SetMotionType ( physMotionType );
	ENGINE_API void SetQualityType ( physMotionQualityType );
	ENGINE_API physMotionType			GetMotionType ( void );
	ENGINE_API physMotionQualityType	GetQualityType ( void );
	ENGINE_API void SetRotationEnabled ( bool );
	ENGINE_API void Wake ( void );
	// Collision Settings
	ENGINE_API void SetShape ( physShape* );
	ENGINE_API void SetPenetrationDepth ( float=0.05f );
	ENGINE_API void SetCollisionLayer ( Layers::PhysicsTypes, int=0 );

	ENGINE_API void EnableCollisionCallback ( void );
	ENGINE_API void DisableCollisionCallback ( void );

	// Accessors
	ENGINE_API physRigidBody* GetBody ( void ) override;

	class mCollisionListener;
	friend mCollisionListener;
protected:
	physRigidBody*	pBody;
	CCollider*		pCollider;
	CTransform*		pTargetTransform;
	Vector3d		vCenterOfMass;
	//CGameObject*	pOwner;

	mCollisionListener*	m_listener;

	physMaterial	mMaterial;

	Vector3d velocity;
	Vector3d gravity;
	bool	bGravityEnabled;
	bool	bRotationEnabled;
	float	fMaxAngularVelocity;

	bool	bRigidBodyActive;


	//void FixedUpdate ( void );
	unsigned int bitmask;

};

#endif