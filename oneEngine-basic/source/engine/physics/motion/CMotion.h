
#ifndef _ENGINE_PHYSICS_C_MOTION_BASE_H_
#define _ENGINE_PHYSICS_C_MOTION_BASE_H_

#include "engine/behavior/CGameBehavior.h"

class CGameObject;

class CMotion : public CGameBehavior
{

public:
	explicit CMotion ( CGameObject* owner = NULL )
		: pOwner(owner)
	{
		;
	}

	// Owner properties
	CGameObject* GetOwner ( void )
	{
		return pOwner;
	}
	void SetOwner ( CGameObject* owner )
	{
		pOwner = owner;
	}

	// Accessors
	ENGINE_API virtual physRigidBody* GetBody ( void )
	{
		return NULL;
	}
protected:
	CGameObject*	pOwner;
};

#endif//_ENGINE_PHYSICS_C_MOTION_BASE_H_