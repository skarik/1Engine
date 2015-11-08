
#ifndef _PROJECTILE_PEBBLE_
#define _PROJECTILE_PEBBLE_

#include "../AfterBaseProjectile.h"

class CModel;
class CParticleSystem;

class ProjectilePebble : public CProjectile
{
public:
	explicit ProjectilePebble ( const Ray &, ftype );
	~ProjectilePebble ( void );

	void Update ( void );

	void OnHit ( CGameObject*, Item::HitType ) override;

private:
	CModel*	pModel;
	CParticleSystem* pTrailParticle;

	Vector3d vTargetPosition;
	Vector3d vRandomRotation;
};

#endif//_PROJECTILE_PEBBLE_