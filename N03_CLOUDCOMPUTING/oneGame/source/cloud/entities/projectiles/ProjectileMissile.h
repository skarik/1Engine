#ifndef _PROJECTILE_MISSILE_H_
#define _PROJECTILE_MISSILE_H_

#include "engine-common/entities/CProjectile.h"

class CModel;
class CParticleSystem;

class ProjectileMissile : public CProjectile
{
public:
	explicit ProjectileMissile ( const Ray & direction, Real speed );
	~ProjectileMissile ( void );

	void Update ( void ) override;

	void OnHit ( CGameObject*, Item::HitType ) override;
protected:
	CParticleSystem* pTrailParticle;
	Vector3d randomOffset;
};

#endif//_PROJECTILE_MISSILE_H_