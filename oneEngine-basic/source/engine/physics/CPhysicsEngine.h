
#ifndef _C_PHYSICS_ENGINE_H_
#define _C_PHYSICS_ENGINE_H_

#include "engine/state/CGameState.h"
//#include "physical/physics/shapes/physShape.h"

class physShape;

//===Physics Class Wrapper Definition===
// Engine-level wrapper functions
class CPhysicsEngine
{
public:
	static void Update ( float, CGameState* pGameState, void (CGameState::*pt2FixedUpdate)(void), void(CGameState::*pt2RigidbodyUpdate)(void) );
	static void UpdateThreaded ( float frameDeltaTime, float fixedDeltaTime, CGameState* pGameState, void (CGameState::*pt2FixedUpdate)(void), void(CGameState::*pt2RigidbodyUpdate)(void) );

public:
	//==Trigger Phantoms==
	// Creation of a trigger phantom
	//FORCE_INLINE ENGINE_API static hkpRigidBody* CreateTriggerVolume ( physRigidBodyInfo* pBodyInfo, physShape* pShape );

	//==Tracing Collision Queries==
	// Cast a ray
	ENGINE_API static void Raycast ( Ray const& rDir, ftype fCastDist, RaycastHit * outHitInfo, const physCollisionFilter& collisionFilter, void* mismatch=NULL );
	// Cast a shape
	ENGINE_API static void Linearcast ( Ray const& rDir, ftype fCastDist, physShape* pShape, RaycastHit* outHitInfo, const int hitInfoArrayCount, const physCollisionFilter& collisionFilter, void* mismatch=NULL );

};

typedef CPhysicsEngine PhysicsEngine;

#include "CPhysicsEngine.hpp"

#endif//_C_PHYSICS_ENGINE_H_