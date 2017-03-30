
#include "RrParticleSystem.h"

RrParticleSystem::RrParticleSystem ( void )
	: CLogicObject()
{
	;
}

RrParticleSystem::~RrParticleSystem ( void )
{
	;
}



//	PreStep()
// Executed before the renderer starts. Particle system GPU uploader.
// Lags a frame behind to give a chance to upload.
void RrParticleSystem::PreStep ( void )
{

}

//	PostStepSynchronus()
// Executed after PostStep jobs requests are started.
// Threaded particle system updater and mesher.
void RrParticleSystem::PostStepSynchronus ( void )
{

}