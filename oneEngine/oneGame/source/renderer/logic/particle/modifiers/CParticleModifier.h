
#ifndef _C_PARTICLE_MODIFIER_
#define _C_PARTICLE_MODIFIER_

// Include particle emitter
#include "../CParticleEmitter.h"

//	class CParticleModifier
// Inherit from this class and override Modify() to create a particle modifier.
class CParticleModifier
{
public:
	RENDER_API virtual void Modify ( CParticle* ) =0;
};

#endif