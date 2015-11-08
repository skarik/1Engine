// Cloud particle emitter. Emits particles specifically where clouds are, and at a very low rate.
// System you'd want to use this with is a slow fade in, slow fade out, yet low lifetime.

#ifndef _C_PARTICLE_EMITTER_CLOUD_H_
#define _C_PARTICLE_EMITTER_CLOUD_H_

// Includes
#include "CParticleEmitter.h"

// Class definition
class CParticleEmitterCloud : public CParticleEmitter
{
private:
	friend CParticleUpdater;
	friend CParticleRenderer;
	friend CParticleEditor;

public:
	RENDER_API explicit CParticleEmitterCloud ( void );
	RENDER_API explicit CParticleEmitterCloud ( const string & );

	RENDER_API void Emit ( void ) override;

private:
	unsigned int current_cloud;
};

#endif