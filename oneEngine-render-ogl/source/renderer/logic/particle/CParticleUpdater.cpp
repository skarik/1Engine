
#include "CParticleUpdater.h"

CParticleUpdater::CParticleUpdater ( CParticleEmitter* pInEmitter )
	: CLogicObject()
{
	myEmitter = pInEmitter;
}
CParticleUpdater::~CParticleUpdater ( void )
{
	// Delete everything in mod list
	for ( std::vector<CParticleModifier*>::iterator mod = vMyModifiers.begin(); mod != vMyModifiers.end(); ++mod )
		delete (*mod);
	vMyModifiers.clear();
}

void CParticleUpdater::AddModifier ( CParticleModifier* mod )
{
	vMyModifiers.push_back( mod );
}

void CParticleUpdater::PreStepSynchronus ( void )
{
	if ( myEmitter == NULL ) {
		return;
	}

	ftype fTempPercent;
	int	  iColorIndex;
	ftype fColorIndex;

	// Loop through all the particles and simulate, as well as update color and size.
	for ( uint16_t i = 0; i < myEmitter->m_max_particle_index; ++i )
	{
		CParticle& particle = myEmitter->m_particles[i];
		if ( particle.alive )
		{
			// Sim the particle
			particle.Simulate();
			// Will the particle be dying?
			if ( particle.fLife < 0 )
			{
				// Mark particle as dead and decrement the particle count
				particle.alive = false;
				myEmitter->m_particle_count -= 1;

				// Strictly ordered particles for trails? Going to shift everything back
				if (myEmitter->m_strictly_ordered_particles)
				{	// Shift it back
					myEmitter->m_max_particle_index -= 1;
					i -= 1; // Go back one particle
					memcpy( myEmitter->m_particles + i, myEmitter->m_particles + i + 1, myEmitter->m_max_particle_index - i );
				}
			}
			// Else we update the particle normally
			else
			{
				fTempPercent = particle.fLife / particle.fStartLife;
				particle.fSize = particle.fEndSize + ( particle.fStartSize - particle.fEndSize ) * fTempPercent;

				// Now update color
				if ( myEmitter->vcColors.size() > 1 )
				{
					// todo: make this faster
					fColorIndex = ( (1-fTempPercent) * (myEmitter->vcColors.size()-1) );
					iColorIndex = int(fColorIndex);
					fColorIndex -= iColorIndex;
					if ( iColorIndex >= (signed)(myEmitter->vcColors.size()-1) )
					{
						iColorIndex = myEmitter->vcColors.size()-2;
						fColorIndex = 1.0f;
					}
					particle.cColor = myEmitter->vcColors[iColorIndex].Lerp( myEmitter->vcColors[iColorIndex+1], fColorIndex );
				}
				else if ( myEmitter->vcColors.size() > 0 )
				{
					particle.cColor = myEmitter->vcColors[0];
				}
				else
				{
					particle.cColor = Color( 1.0f,1.0f,1.0f,1.0f );
				}

				// Now, perform modefiers
				for ( std::vector<CParticleModifier*>::iterator mod = vMyModifiers.begin(); mod != vMyModifiers.end(); ++mod )
					(*mod)->Modify( myEmitter->m_particles + i );
			}
		}
	}
}

#include "core-ext/system/io/serializer/CBaseSerializer.h"
void	CParticleUpdater::serialize ( Serializer & ser, const uint ver )
{
	ser & name;
}