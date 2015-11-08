
#ifndef _C_COLLIDER_H_
#define _C_COLLIDER_H_

#include "physical/physics/CPhysics.h"
#include "sCollision.h"

class CCollider;
class CRigidBody;

class CCollider
{
public:
	ENGINE_API void OnCollide ( sCollision& );
	ENGINE_API void OnEnter ( sCollision& );
	ENGINE_API void OnLeave ( sCollision& );

	physShape* GetCollisionShape ( void ) {
		return pCollisionShape;
	}
	Vector3d GetCenter ( void ) {
		return vCenterOfMass;
	}
	CRigidBody* GetRigidBody ( void ) {
		return pRigidBody;
	}

	void SetRigidBody ( CRigidBody* pNewBody ) {
		pRigidBody = pNewBody;
	}
	void SetCenter ( Vector3d const& v_newCenter ) {
		vCenterOfMass = v_newCenter;
	}


protected:
	physShape*	pCollisionShape;
	CRigidBody*	pRigidBody;

	Vector3d vCenterOfMass;
};

#include "CCollider.hpp"

#endif