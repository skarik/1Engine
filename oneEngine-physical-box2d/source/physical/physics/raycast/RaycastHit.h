
#ifndef _STRUCT_RAYCAST_HIT_H_
#define _STRUCT_RAYCAST_HIT_H_

// Includes
#include "core/math/Vector3d.h"
#include "physical/physics/shapes/physShape.h"

class CGameBehavior;
class CMotion;

// Struct definition
struct RaycastHit
{
	bool		hit;
	Vector3d	hitPos;
	Vector3d	hitNormal;
	ftype		distance;
	CGameBehavior*	pHitBehavior;
	physRigidBody*	pHitBody;

	RaycastHit ( void ) 
		: hit(false), distance(0), pHitBehavior(NULL), pHitBody(NULL)
	{
		;
	}
};

#endif