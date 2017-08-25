// class CTerraExplosion
// Class for creating a raytraced explosion through the terrain and applying damage and forces to objects nearby.
// This class has no visual or audible aspect. For that, instantiate a CFXExplosion with the ignoreDamage flag set to true.
// The CFXExplosion will then provide the visual and audible aspects, while CTerraExplosion does all the work.

#ifndef _C_TERRA_EXPLOSION_H_
#define _C_TERRA_EXPLOSION_H_

#include "engine/behavior/CGameObject.h"

class CTerraExplosion : public CGameObject
{
	ClassName( "TerraExplosion" );
public:
	explicit CTerraExplosion ( Vector3d& vPos, ftype fMagnitude=30.0f, ftype fPhysicsMultiplier=1.0f );
	~CTerraExplosion ( void );

	void Explode ( void );
	void Update ( void );

protected:
	bool	exploded;
	ftype	magnitude;
	ftype	physics_multiplier;
};

#endif