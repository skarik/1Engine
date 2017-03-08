// If you want to create a static mesh easily, just pass a negative mass into the constructor.

#ifndef _C_RIGID_BODY_H_
#define _C_RIGID_BODY_H_

#include "physical/physics/CPhysics.h"
#include "engine/physics/collider/CCollider.h"
#include "engine/behavior/CGameBehavior.h"
#include "engine/physics/material/physMaterial.h"
#include "engine/physics/motion/CMotion.h"

class physRigidBody;

class CRigidbody : public CMotion
{
	ClassName( "CRigidbody" );
protected:
	explicit CRigidbody () : CMotion(), pBody(NULL) {};
public:
	//===============================================================================================//
	// CONSTRUCTION and DESTRUCTION
	//===============================================================================================//
	ENGINE_API explicit	CRigidbody ( CCollider* body_collider, CGameBehavior * owner_behavior, float mass=1.0F );
	ENGINE_API			~CRigidbody ( void );

	//===============================================================================================//
	// STEP EVENTS
	//===============================================================================================//

	// Empty game loop 
	void Update ( void ) override {};
	void LateUpdate ( void ) override {};
	
	// Physics step - the only object type that uses this.
	void RigidbodyUpdate ( void );

public:
	// Output targets.
	
	// Transform that motion results are copied to. If NULL, transform must be queried manually.
	Transform*		target_transform;
	// Position that motion results are copied to. If NULL, transform must be queried manually.
	// target_transform must be NULL for this to work.
	Vector3d*		target_position;

public:
	//===============================================================================================//
	// GETTERS AND SETTERS (PHYSICS ENGINE WRAPPER)
	//===============================================================================================//

	// Owner
	ENGINE_API CGameBehavior*	GetOwner ( void );

	// Mass Properties
	ENGINE_API void		SetMass ( float=1.0F );
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

public:
	// Listener class used to forward collision events to the engine
	class mCollisionListener;
	friend mCollisionListener;

protected:
	//===============================================================================================//
	// INTERNAL STATE AND COMPONENTS
	//===============================================================================================//

	// Components
	physRigidBody*	pBody;
	CCollider*		pCollider;
	Vector3d		vCenterOfMass;
	mCollisionListener*	m_listener;

	// Properties
	physMaterial	mMaterial;
	unsigned int	bitmask;

	// Motion state
	Vector3d	velocity;
	Vector3d	gravity;
	bool		bGravityEnabled;
	bool		bRotationEnabled;
	float		fMaxAngularVelocity;

	// System motion state
	bool		bRigidBodyActive;
};

#endif