
#include "core/time/time.h"
#include "CParticleMod_Windmotion.h"
#include "physical/physics/wind/Wind.h"

void CParticleMod_Windmotion::Modify ( CParticle* particle )
{
	particle->vVelocity += WindTester::Get()->GetFlowFieldFast( particle->vPosition ) * 32.2f * Time::deltaTime * m_SpeedMultipler;
}

#include "core-ext/system/io/serializer/CBaseSerializer.h"
void CParticleMod_Windmotion::serialize ( Serializer & ser, const uint ver )
{
	ser & m_SpeedMultipler;
}