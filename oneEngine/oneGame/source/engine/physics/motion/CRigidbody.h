// If you want to create a static mesh easily, just pass a negative mass into the constructor.

#ifndef _C_RIGID_BODY_H_
#define _C_RIGID_BODY_H_

#include "core-ext/transform/Transform.h"
#include "core-ext/types/baseType.h"
#include "physical/types/layers.h"
#include "physical/types/motion.h"
#include "physical/physics/motion/IPrRigidBody.h"
#include "engine/physics/collider/CCollider.h"
#include "engine/behavior/CGameBehavior.h"
#include "engine/physics/material/physMaterial.h"
#include "engine/physics/motion/CMotion.h"

#include "bullet/LinearMath/btMotionState.h"

//class physRigidBody;
class btRigidBody;
class PrWorld;
class PrShape;

struct prRigidbodyCreateParams
{
	// Owning object?
	void*				owner;
	core::arBaseType	ownerType;

	// What world is this body a part of? When NULL, adds it to the currently active one.
	PrWorld*			world;

	//CCollider*				collider;
	PrShape*				shape;
	physical::prLayer		group; 
	float	mass;
};

class CRigidbody : public CMotion, public IPrRigidBody, public btMotionState
{
	ClassName( "CRigidbody" );
//protected:
	//explicit CRigidbody () : CMotion(), pBody(NULL) {};
public:
	//===============================================================================================//
	// CONSTRUCTION and DESTRUCTION
	//===============================================================================================//
	//ENGINE_API explicit	CRigidbody ( CCollider* body_collider, CGameBehavior* owner_behavior, float mass=1.0F );

	ENGINE_API explicit	CRigidbody ( const prRigidbodyCreateParams& params );
	ENGINE_API			~CRigidbody ( void );

	//===============================================================================================//
	// STEP EVENTS
	//===============================================================================================//

	// Empty game loop 
	void				Update ( void ) override {}
	void				LateUpdate ( void ) override {}
	
	// Physics step - the only object type that uses this.
	void				RigidbodyUpdate ( Real interpolation ) override;


	ENGINE_API void		OnWorldDestroyed ( void ) override {}

	
	//	getWorldTransform (write transform) : Bullet API
	///synchronizes world transform from user to physics
	ENGINE_API void		getWorldTransform(btTransform& worldTrans) const override;
	//	setWorldTransform (read transform) : Bullet API
	///synchronizes world transform from physics to user
	///Bullet only calls the update of worldtransform for active objects
	ENGINE_API void		setWorldTransform(const btTransform& worldTrans) override;

public:
	// Internal storage.
	//core::Transform		transform;
	XrTransform			world_transform;
	XrTransform			interpolated_transform;

	// Output targets.
	
	// Transform that motion results are copied to. If NULL, transform must be queried manually.
	core::Transform*	target_transform;
	// Position that motion results are copied to. If NULL, transform must be queried manually.
	// target_transform must be NULL for this to work.
	Vector3f*			target_position;

public:
	//===============================================================================================//
	// GETTERS AND SETTERS (PHYSICS ENGINE WRAPPER)
	//===============================================================================================//
	// TODO: Start with the physics-level layers. It's quite possibile that now, the entirety of the system
	//       can reside in onePhysical, segregating the systems apart.
	// This is a wrapper for physBody. Since there is now an IPrGameMotion interface, functionality can be transferred to there.
	// All that would then need to be done is inherit it.

	// Motion Settings
	ENGINE_API void SetMotionType ( physical::motion::prMotionTypes );

	//	PushTransform() : Pushes current world transform to the physics engine.
	ENGINE_API void PushTransform ( void );

	//// Owner
	//ENGINE_API CGameBehavior*	GetOwner ( void );

	//// Mass Properties
	//ENGINE_API void		SetMass ( float=1.0F );
	//ENGINE_API float	GetMass ( void );

	//// Movement setters
	//ENGINE_API virtual void		SetVelocity ( Vector3f );
	//ENGINE_API virtual Vector3f	GetVelocity ( void );
	//ENGINE_API virtual void		SetAcceleration ( Vector3f );
	//ENGINE_API virtual Vector3f	GetAcceleration ( void );
	//ENGINE_API void	SetGravity ( bool );
	//ENGINE_API bool	GetGravity ( void );
	//ENGINE_API void	ApplyForce ( Vector3f );
	//ENGINE_API void		SetPosition ( Vector3f );
	//ENGINE_API Vector3f	GetPosition ( void );
	//ENGINE_API Vector3f	AddToPosition ( Vector3f );
	//ENGINE_API void		SetRotation ( Quaternion );
	//ENGINE_API Quaternion	GetRotation ( void );

	//// Movement property setters
	//ENGINE_API void SetRestitution ( float=0.4f );
	//ENGINE_API void	SetFriction ( float=0.5f );
	//ENGINE_API float	GetFriction ( void );
	//ENGINE_API void SetLinearDamping ( float=0.0f );
	//ENGINE_API void SetAngularDamping ( float=0.05f );

	//// Motion Settings
	//ENGINE_API void SetMotionType ( physMotionType );
	//ENGINE_API void SetQualityType ( physMotionQualityType );
	//ENGINE_API physMotionType			GetMotionType ( void );
	//ENGINE_API physMotionQualityType	GetQualityType ( void );
	//ENGINE_API void SetRotationEnabled ( bool );
	//ENGINE_API void Wake ( void );

	//// Collision Settings
	//ENGINE_API void SetShape ( physShape* );
	//ENGINE_API void SetPenetrationDepth ( float=0.05f );
	//ENGINE_API void SetCollisionLayer ( physical::layer::prLayerTypes, int=0 );

	//ENGINE_API void EnableCollisionCallback ( void );
	//ENGINE_API void DisableCollisionCallback ( void );

	//// Accessors
	//ENGINE_API physRigidBody* GetBody ( void ) override;

	

public:
	// Listener class used to forward collision events to the engine
	class mCollisionListener;
	friend mCollisionListener;

protected:
	//===============================================================================================//
	// INTERNAL STATE AND COMPONENTS
	//===============================================================================================//

	physical::motion::prMotionTypes	motionType;

	// Components
	//CCollider*		pCollider;
	//PrShape*		m_shape;
	Vector3f		vCenterOfMass;
	mCollisionListener*	m_listener;

	// Currently active world
	PrWorld*		world;

	// Properties
	physMaterial	mMaterial;
	unsigned int	bitmask;

	// Motion state
	Vector3f	velocity;
	Vector3f	gravity;
	bool		bGravityEnabled;
	bool		bRotationEnabled;
	float		fMaxAngularVelocity;

	// System motion state
	bool		bRigidBodyActive;
};

#endif