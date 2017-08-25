#ifndef I_PR_GAME_UPDATE_H_
#define I_PR_GAME_UPDATE_H_

#include "core/types.h"

class IPrGameUpdate
{
public:
	PHYS_API void	PhysicsUpdate ( const float deltaTime );
	PHYS_API void	PhysicsUpdateThreaded ( const float frameDeltaTime, const float fixedDeltaTime );

	virtual void	FixedUpdate (void) = 0;
	virtual void	RigidbodyUpdate (Real interpolation) = 0;
};

#endif//I_PR_GAME_UPDATE_H_