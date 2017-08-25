#ifndef PHYSICAL_PHYSICS_MOTION_I_PR_GAME_MOTION_H_
#define PHYSICAL_PHYSICS_MOTION_I_PR_GAME_MOTION_H_

#include "core/types/types.h"

class btCollisionObject;

class IPrGameMotion
{
public:
	PHYS_API explicit		IPrGameMotion ( void )
		{}
	PHYS_API virtual		~IPrGameMotion ( void )
		{}

	PHYS_API virtual void	OnWorldDestroyed ( void )=0;
};

#endif//PHYSICAL_PHYSICS_MOTION_I_PR_GAME_MOTION_H_