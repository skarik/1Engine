
#ifndef _ENGINE_PHYSICS_C_MOTION_BASE_H_
#define _ENGINE_PHYSICS_C_MOTION_BASE_H_

#include "engine/behavior/CGameBehavior.h"

class CGameObject;

class CMotion : public CGameBehavior
{

public:
	explicit CMotion ( CGameBehavior* n_owner = NULL )
		: owner(n_owner)
	{
		;
	}

	// Owner properties
	CGameBehavior* GetOwner ( void )
	{
		return owner;
	}
	void SetOwner ( CGameBehavior* n_owner )
	{
		owner = n_owner;
	}

	// Accessors
	ENGINE_API virtual physRigidBody* GetBody ( void )
	{
		return NULL;
	}
protected:
	CGameBehavior*	owner;
};

#endif//_ENGINE_PHYSICS_C_MOTION_BASE_H_