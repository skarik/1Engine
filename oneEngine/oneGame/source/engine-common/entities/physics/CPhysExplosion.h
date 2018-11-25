
#ifndef _C_PHYS_EXPLOSION_H_
#define _C_PHYS_EXPLOSION_H_

#include "engine/behavior/CGameObject.h"

class CPhysExplosion : public CGameObject
{
	ClassName( "PhysExplosion" );
public:
	explicit CPhysExplosion ( Vector3f& vPos, Real fMagnitude=30.0f, Real fPhysicsMultiplier=1.0f );
	~CPhysExplosion ( void );

	void Explode ( void );
	void Update ( void );

protected:
	bool	exploded;
	Real	magnitude;
	Real	physics_multiplier;
};

#endif