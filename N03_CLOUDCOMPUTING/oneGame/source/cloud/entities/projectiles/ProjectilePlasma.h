#ifndef _PROJECTILE_PLASMA_H_
#define _PROJECTILE_PLASMA_H_

#include "engine-common/entities/CProjectile.h"

class CModel;
class CParticleSystem;

class ProjectilePlasma : public CProjectile
{
public:
	explicit ProjectilePlasma ( const Ray & direction, Real speed );
	~ProjectilePlasma ( void );

	void Update ( void ) override;

	void OnHit ( CGameObject*, Item::HitType ) override;
protected:
	CParticleSystem* pTrailParticle;
};

#endif//_PROJECTILE_PLASMA_H_