
#ifndef _PROJECTILE_MAGIC_BOLT_H_
#define _PROJECTILE_MAGIC_BOLT_H_

#include "../ProjectileMagicalBase.h"

class CModel;
class CParticleSystem;
class CLight;

class ProjectileMagicBolt : public ProjectileMagicalBase
{
public:
	explicit ProjectileMagicBolt ( const Ray &, ftype );
	~ProjectileMagicBolt ( void );

	void Update ( void );

	void OnHit ( CGameObject*, Item::HitType ) override;

private:
	CModel*	pModel;
	CParticleSystem* pTrailParticle;

	CLight* pLight;

	Vector3d vTargetVelocity;
};


#endif//_PROJECTILE_MAGIC_BOLT_H_