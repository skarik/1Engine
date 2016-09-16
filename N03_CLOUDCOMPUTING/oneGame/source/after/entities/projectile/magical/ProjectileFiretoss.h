
#ifndef _PROJECTILE_FIRETOSS_H_
#define _PROJECTILE_FIRETOSS_H_

#include "../ProjectileMagicalBase.h"

class CParticleSystem;
class CLight;
class CModel;

class ProjectileFiretoss : public ProjectileMagicalBase
{
public:
	explicit ProjectileFiretoss ( const Ray &, ftype );
	~ProjectileFiretoss ( void );

	void Update ( void );

	void OnHit ( CGameObject*, Item::HitType ) override;

private:
	CModel*	pModel;
	CParticleSystem* pTrailParticle;

	CLight* pLight;

	//Vector3d vTargetPosition;
	//Vector3d vRandomRotation;
	Vector3d vTargetVelocity;
};


#endif//_PROJECTILE_FIRETOSS_H_