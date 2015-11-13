
#include "core/time/time.h"
#include "CParticle.h"

// Simulation function
bool CParticle::Simulate ( void )
{
	/*ftype	fStartLife
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
	Color		cColor;*/
	fLife		-= Time::deltaTime;

	if ( fLife > 0 )
	{
		fAngleVelocity += fAngleAcceleration * Time::deltaTime;
		fAngleVelocity -= fAngleVelocity * fAngleDamping * Time::deltaTime;

		vVelocity += vAcceleration * Time::deltaTime;
		vVelocity.x -= vVelocity.x * vLinearDamping.x * Time::deltaTime;
		vVelocity.y -= vVelocity.y * vLinearDamping.y * Time::deltaTime;
		vVelocity.z -= vVelocity.z * vLinearDamping.z * Time::deltaTime;

		fAngle		+= fAngleVelocity * Time::deltaTime;
		vPosition	+= vVelocity * Time::deltaTime;
		
		return true;
	}
	return false;
}