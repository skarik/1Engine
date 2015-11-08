
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
	for ( std::vector<CParticle>::iterator it = myEmitter->vParticles.begin(); it != myEmitter->vParticles.end(); )
	{
		if ( it->Simulate() )
		{
			fTempPercent = it->fLife / it->fStartLife;
			it->fSize = it->fEndSize + ( it->fStartSize - it->fEndSize ) * fTempPercent;

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
				it->cColor = myEmitter->vcColors[iColorIndex].Lerp( myEmitter->vcColors[iColorIndex+1], fColorIndex );
			}
			else if ( myEmitter->vcColors.size() > 0 )
				it->cColor = myEmitter->vcColors[0];
			else
				it->cColor = Color( 1.0f,1.0f,1.0f,1.0f );

			// Now, perform modefiers
			for ( std::vector<CParticleModifier*>::iterator mod = vMyModifiers.begin(); mod != vMyModifiers.end(); ++mod )
				(*mod)->Modify( it );

			// Move onto next particle
			++it;
		}
		else
		{
			// Delete the particle if it's too old
			it = myEmitter->vParticles.erase( it );
		}
	}
}

#include "core-ext/system/io/serializer/CBaseSerializer.h"
void	CParticleUpdater::serialize ( Serializer & ser, const uint ver )
{
	ser & name;
}