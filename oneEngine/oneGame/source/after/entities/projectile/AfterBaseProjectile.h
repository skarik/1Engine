
#ifndef _AFTER_ENTITY_BASE_PROJECTILE_H_
#define _AFTER_ENTITY_BASE_PROJECTILE_H_

#include "engine-common/entities/CProjectile.h"
#include "after/states/projectileeffect/ProjectileEffects.h"

class AfterBaseProjectile : public CProjectile
{
public:
	explicit AfterBaseProjectile( Ray const& rnInRay, ftype fnInSpeed, ftype fnWidth=0.2f );

	// == Update ==
	void Update ( void );

	// ===Callbacks===
	virtual void OnHit ( CGameObject*, Item::HitType ) override;
	//virtual void OnEnterWater ( void ) override;

	// Buff Accessor for editing effects on creation
	Debuffs::ProjectileEffects* Effects ( void ) { return &effects; };

protected:
	Debuffs::ProjectileEffects effects;
};

#endif//_AFTER_ENTITY_BASE_PROJECTILE_H_