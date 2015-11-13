
#ifndef _P_EFFECT_BURN_ON_HIT_H_
#define _P_EFFECT_BURN_ON_HIT_H_

#include "../ProjectileEffects.h"

class CParticleSystem;

class PEffectBurnOnHit : public Debuffs::CProjectileBuff
{
public:
	void Initialize ( void ) override;
	~PEffectBurnOnHit ( void );

	bool OnHitEnemy ( CActor* enemy ) override;

private:
	CParticleSystem*	ps_trail_effect;

};


#endif//_P_EFFECT_BURN_ON_HIT_H_