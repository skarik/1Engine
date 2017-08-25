
#ifndef _STRUCT_RAYCAST_HIT_H_
#define _STRUCT_RAYCAST_HIT_H_

// Includes
#include "core/math/Vector3d.h"
#include "core-ext/types/baseType.h"
//#include "physical/physics/shapes/physShape.h"

class CGameBehavior;
class CMotion;
class btRigidBody;

// Struct definition
struct RaycastHit
{
	bool		hit;
	Vector3d	hitPos;
	Vector3d	hitNormal;
	Real		distance;
	//CGameBehavior*	pHitBehavior;
	//physRigidBody*	pHitBody;
	btRigidBody*		hitBody;
	void*				hitObject;
	core::arBaseType	hitType;

	RaycastHit ( void ) 
		: hit(false), distance(0),
		hitBody(NULL), hitObject(NULL), hitType(core::kBasetypeVoidPointer)
	{
		;
	}
};

#endif