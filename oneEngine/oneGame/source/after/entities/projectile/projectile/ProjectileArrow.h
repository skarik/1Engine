
#ifndef _PROJECTILE_ARROW_
#define _PROJECTILE_ARROW_

#include "../AfterBaseProjectile.h"

class CModel;
class CParticleSystem;

class ProjectileArrow : public AfterBaseProjectile
{
public:
	explicit ProjectileArrow ( const Ray &, ftype );
	~ProjectileArrow ( void );

	void Update ( void );

	void OnHit ( CGameObject*, Item::HitType ) override;

private:
	CModel*	pModel;
	CParticleSystem* pTrailParticle;
};

#endif