
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
	RENDER_API bool Simulate ( void );

private:
	ftype	fStartLife;
	ftype	fStartSize;
	ftype	fEndSize;

	ftype	fLife;
	ftype	fSize;
	ftype	fAngle;
	ftype	fAngleVelocity;
	ftype	fAngleAcceleration;
	ftype	fAngleDamping;
	Vector3d	vPosition;
	Vector3d	vVelocity;
	Vector3d	vAcceleration;
	Vector3d	vLinearDamping;
	Color		cColor;

};

#endif