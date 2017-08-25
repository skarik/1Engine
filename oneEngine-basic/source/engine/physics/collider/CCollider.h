
#ifndef _C_COLLIDER_H_
#define _C_COLLIDER_H_

#include "sCollision.h"

class CCollider;
class CRigidbody;
class PrShape;

class CCollider
{
public:
	ENGINE_API void OnCollide ( sCollision& );
	ENGINE_API void OnEnter ( sCollision& );
	ENGINE_API void OnLeave ( sCollision& );

	PrShape* GetCollisionShape ( void ) {
		return pCollisionShape;
	}
	Vector3d GetCenter ( void ) {
		return vCenterOfMass;
	}
	CRigidbody* GetRigidBody ( void ) {
		return pRigidBody;
	}

	void SetRigidBody ( CRigidbody* pNewBody ) {
		pRigidBody = pNewBody;
	}
	void SetCenter ( Vector3d const& v_newCenter ) {
		vCenterOfMass = v_newCenter;
	}


protected:
	PrShape*	pCollisionShape;
	CRigidbody*	pRigidBody;

	Vector3d vCenterOfMass;
};

#include "CCollider.hpp"

#endif