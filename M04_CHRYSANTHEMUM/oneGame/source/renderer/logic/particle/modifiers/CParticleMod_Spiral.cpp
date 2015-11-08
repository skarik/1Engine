
#include "CParticleMod_Spiral.h"
#include "core/math/Math.h"
#include "core/time/time.h"

CParticleMod_Spiral::CParticleMod_Spiral ( void )
	: CParticleModifier()
{
	m_rotaryFrequency		= Vector3d( 1,0,0 );
	m_rotaryOffset			= Vector3d( 0,0,0 );
	m_rotaryPosition		= Vector3d( 1,0,0 );
	m_rotaryVelocity		= Vector3d( 0,0,0 );
	m_rotaryAcceleration	= Vector3d( 0,0,0 );
}

void CParticleMod_Spiral::Modify ( std::vector<CParticle>::iterator& particle )
{
	//particle->vVelocity += WindMotion.GetFlowFieldFast( particle->vPosition ) * 32.2f * Time::deltaTime * m_SpeedMultipler;
	particle->vPosition.x += sin( (m_rotaryOffset.x + m_rotaryFrequency.x*(particle->fStartLife - particle->fLife))*PI ) * m_rotaryPosition.x * Time::deltaTime;
	particle->vPosition.y += sin( (m_rotaryOffset.y + m_rotaryFrequency.y*(particle->fStartLife - particle->fLife))*PI ) * m_rotaryPosition.y * Time::deltaTime;
	particle->vPosition.z += sin( (m_rotaryOffset.z + m_rotaryFrequency.z*(particle->fStartLife - particle->fLife))*PI ) * m_rotaryPosition.z * Time::deltaTime;

	particle->vVelocity.x += sin( (m_rotaryOffset.x + m_rotaryFrequency.x*(particle->fStartLife - particle->fLife))*PI ) * m_rotaryVelocity.x * Time::deltaTime;
	particle->vVelocity.y += sin( (m_rotaryOffset.y + m_rotaryFrequency.y*(particle->fStartLife - particle->fLife))*PI ) * m_rotaryVelocity.y * Time::deltaTime;
	particle->vVelocity.z += sin( (m_rotaryOffset.z + m_rotaryFrequency.z*(particle->fStartLife - particle->fLife))*PI ) * m_rotaryVelocity.z * Time::deltaTime;

	particle->vAcceleration.x += sin( (m_rotaryOffset.x + m_rotaryFrequency.x*(particle->fStartLife - particle->fLife))*PI ) * m_rotaryAcceleration.x * Time::deltaTime;
	particle->vAcceleration.y += sin( (m_rotaryOffset.y + m_rotaryFrequency.y*(particle->fStartLife - particle->fLife))*PI ) * m_rotaryAcceleration.y * Time::deltaTime;
	particle->vAcceleration.z += sin( (m_rotaryOffset.z + m_rotaryFrequency.z*(particle->fStartLife - particle->fLife))*PI ) * m_rotaryAcceleration.z * Time::deltaTime;
}

#include "core-ext/system/io/serializer/CBaseSerializer.h"
void CParticleMod_Spiral::serialize ( Serializer & ser, const uint ver )
{
	//ser & m_SpeedMultipler;
}