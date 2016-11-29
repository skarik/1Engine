#ifndef _C_PARTICLE_HPP_
#define _C_PARTICLE_HPP_

#include "core/time/time.h"
#include "CParticle.h"

// Simulation function
void CParticle::Simulate ( void )
{
	// Update life
	fLife -= Time::deltaTime;

	// Update the rest
	fAngleVelocity += fAngleAcceleration * Time::deltaTime;
	fAngleVelocity -= fAngleVelocity * fAngleDamping * Time::deltaTime;

	vVelocity += vAcceleration * Time::deltaTime;
	vVelocity.x -= vVelocity.x * vLinearDamping.x * Time::deltaTime;
	vVelocity.y -= vVelocity.y * vLinearDamping.y * Time::deltaTime;
	vVelocity.z -= vVelocity.z * vLinearDamping.z * Time::deltaTime;

	fAngle		+= fAngleVelocity * Time::deltaTime;
	vPosition	+= vVelocity * Time::deltaTime;

	// Return if alive (very small branch)
	//return fLife > 0;
}

#endif//_C_PARTICLE_HPP_