
#ifndef _C_PARTICLE_H_
#define _C_PARTICLE_H_

// Include all attributes
#include "core/types/types.h"
#include "core/types/float.h"
#include "core/math/Vector2d.h"
#include "core/math/Vector3d.h"
#include "core/math/Color.h"
#include "core/math/random/RangeValue.h"

// Prototypes for friending Emitter, Updater, Modifier, and Renderer
class CParticleEmitter;
class CParticleUpdater;
class CParticleModifier;
class CParticleRenderer;
class CParticleRenderer_Animated;

// List of modifiers
class CParticleMod_Windmotion;
class CParticleMod_Spiral;

// List of emitters
class CParticleEmitterCloud;
class CParticleEmitterSmooth;

// Class Definition
class CParticle
{
private:
	// List of particle system classes
	friend CParticleEmitter;
	friend CParticleUpdater;
	friend CParticleModifier;
	friend CParticleRenderer;
	friend CParticleRenderer_Animated;

	// List of modifiers
	friend CParticleMod_Windmotion;
	friend CParticleMod_Spiral;

	// List of emitters
	friend CParticleEmitterCloud;
	friend CParticleEmitterSmooth;

public:
	FORCE_INLINE RENDER_API void Simulate ( void );

private:
	uint8_t		alive;
	uint8_t		dummy;

	Real32	fLife;

	Real32	fStartLife;
	Real32	fStartSize;
	Real32	fEndSize;
	Real32	fSize;
	
	Real32	fAngle;
	Real32	fAngleVelocity;
	Real32	fAngleAcceleration;
	Real32	fAngleDamping;
	
	Vector3d	vPosition;
	Vector3d	vVelocity;
	Vector3d	vAcceleration;
	Vector3d	vLinearDamping;
	
	Color		cColor;
};

// Source Definition:
#include "CParticle.hpp"

#endif