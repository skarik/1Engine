
#ifndef _P_EFFECT_BOUNCING_H_
#define _P_EFFECT_BOUNCING_H_

#include "../ProjectileEffects.h" 

class PEffectBouncing : public Debuffs::CProjectileBuff
{
public:
	explicit PEffectBouncing ( const int nMaxBounceCount );
	//void Initialize ( void ) override;

	bool OnHitWorld( const Vector3d&, const Vector3d& ) override;

private:
	int mMaxBounces;
	int mBounces;

};

#endif//_P_EFFECT_BOUNCING_H_