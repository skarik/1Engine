
#ifndef _ENGINE_PHYSICS_C_MOTION_BASE_H_
#define _ENGINE_PHYSICS_C_MOTION_BASE_H_

#include "core-ext/types/baseType.h"
#include "physical/physics/motion/IPrGameMotion.h"
#include "engine/behavior/CGameBehavior.h"

class CGameBehavior;
class RrLogicObject;
class RrRenderObject;
class btMotionState;

class CMotion : public CGameBehavior, public IPrGameMotion
{
public:
	explicit CMotion ( void* n_owner, core::arBaseType n_ownerType )
		: owner(n_owner), ownerType(n_ownerType),
		CGameBehavior(), IPrGameMotion()
	{
		;
	}

	template <typename T>
	T* GetOwner ( void )
	{
		return static_cast<T*>(owner);
	}

	template <typename T>
	void SetOwner ( T* n_owner )
	{
		owner = static_cast<void*>(n_owner);
		ownerType = core::kBasetypeVoidPointer;
	}
	// Accessors
	/*ENGINE_API virtual physRigidBody* GetBody ( void )
	{
		return NULL;
	}*/
protected:
	void*				owner;
	core::arBaseType	ownerType;

	btMotionState*		motion;
};


#endif//_ENGINE_PHYSICS_C_MOTION_BASE_H_