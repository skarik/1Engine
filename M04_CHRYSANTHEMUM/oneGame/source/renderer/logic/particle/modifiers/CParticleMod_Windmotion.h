
#ifndef _C_PARTICLE_MODE_WIND_MOTION_H_
#define _C_PARTICLE_MODE_WIND_MOTION_H_

#include "CParticleModifier.h"

class CParticleMod_Windmotion : public CParticleModifier
{
public:
	RENDER_API CParticleMod_Windmotion ( ftype SpeedMultiplier = 1.0f ) : CParticleModifier(), m_SpeedMultipler( SpeedMultiplier ) {
		;
	}
	RENDER_API void Modify ( std::vector<CParticle>::iterator& particle ) override;
	RENDER_API virtual void	serialize ( Serializer &, const uint );
private:
	ftype m_SpeedMultipler;
};

#endif