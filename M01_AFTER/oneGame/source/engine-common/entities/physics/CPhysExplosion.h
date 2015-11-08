
#ifndef _C_PHYS_EXPLOSION_H_
#define _C_PHYS_EXPLOSION_H_

#include "engine/behavior/CGameObject.h"

class CPhysExplosion : public CGameObject
{
	ClassName( "PhysExplosion" );
public:
	explicit CPhysExplosion ( Vector3d& vPos, ftype fMagnitude=30.0f, ftype fPhysicsMultiplier=1.0f );
	~CPhysExplosion ( void );

	void Explode ( void );
	void Update ( void );

protected:
	bool	exploded;
	ftype	magnitude;
	ftype	physics_multiplier;
};

#endif