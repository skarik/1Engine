
#include "core/math/Math.h"
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
	// Change previous frame's used buffer to the unused buffer

	// Update system mesh to the ununused buffer
	
}

//	PostStepSynchronus()
// Executed after PostStep jobs requests are started.
// Threaded particle system updater and mesher.
void RrParticleSystem::PostStepSynchronus ( void )
{

}


//		SetParticleSize ( size ) : sets size of a single particle in floats (4 bytes)
// sets the size of a particle in floats (4 bytes). actual size is input parameter * 4
void RrParticleSystem::SetParticleSize ( const size_t size )
{
	particle_stride = size * 4;

	int temp_power = Math::log2(particle_stride);
	particle_stride = 1 << temp_power; // TODO: Verify this result
}