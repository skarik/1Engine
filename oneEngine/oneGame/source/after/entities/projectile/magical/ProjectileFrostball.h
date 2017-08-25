
#ifndef _PROJECTILE_FROSTBALL_H_
#define _PROJECTILE_FROSTBALL_H_

#include "../ProjectileMagicalBase.h"

class CModel;
class CParticleSystem;
class CLight;

class ProjectileFrostball : public ProjectileMagicalBase
{
public:
	explicit ProjectileFrostball ( const Ray &, ftype );
	~ProjectileFrostball ( void );

	void Update ( void );

	void OnHit ( CGameObject*, Item::HitType ) override;

private:
	CModel*	pModel;
	CParticleSystem* pTrailParticle;

	CLight* pLight;

	Vector3d vTargetVelocity;
};


#endif//_PROJECTILE_FROSTBALL_H_