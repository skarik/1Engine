#ifndef _PROJECTILE_BULLET_H_
#define _PROJECTILE_BULLET_H_

#include "engine-common/entities/CProjectile.h"

class CModel;
class CParticleSystem;

class ProjectileBullet : public CProjectile
{
public:
	explicit ProjectileBullet ( const Ray & direction, Real speed );
	~ProjectileBullet ( void );

	void Update ( void ) override;

	void OnHit ( CGameObject*, Item::HitType ) override;
protected:
	CParticleSystem* pTrailParticle;

};

#endif//_PROJECTILE_BULLET_H_