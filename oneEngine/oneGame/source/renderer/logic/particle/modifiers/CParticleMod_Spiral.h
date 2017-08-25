
#ifndef _C_PARTICLE_MODE_SPIRAL_H_
#define _C_PARTICLE_MODE_SPIRAL_H_

#include "CParticleModifier.h"

class CParticleMod_Spiral : public CParticleModifier
{
public:
	RENDER_API CParticleMod_Spiral ( void );

	RENDER_API void Modify ( CParticle* particle ) override;
	RENDER_API virtual void	serialize ( Serializer &, const uint );

public:
	Vector3d m_rotaryFrequency;
	Vector3d m_rotaryOffset;
	Vector3d m_rotaryPosition;
	Vector3d m_rotaryVelocity;
	Vector3d m_rotaryAcceleration;
};

#endif//_C_PARTICLE_MODE_SPIRAL_H_