
#ifndef _C_FX_EXPLOSION_H_
#define _C_FX_EXPLOSION_H_

#include "engine/behavior/CGameObject.h"

class CFXExplosion : public CGameObject
{
	ClassName( "FXExplosion" );
public:
	explicit CFXExplosion ( Vector3d& vPos, ftype fMagnitude=30.0f, ftpye fPhysicsMultiplier=1.0f, bool ignoreDamage=false );
	~CFXExplosion ( void );

	void Explode ( void );
	void Update ( void );

protected:
	bool	exploded;
	ftype	magnitude;
	ftype	physics_multiplier;
	bool	ignoreDamage;
};

#endif