
#include "CRIDynamite.h"
#include "core/time/time.h"
#include "after/terrain/modifiers/CTerraExplosion.h"

#include "engine-common/entities/CParticleSystem.h"

#include "renderer/logic/model/CModel.h"

CRIDynamite::CRIDynamite ( tTossableProperties& inProps )
	: CBaseRandomTossable( inProps, ItemData() )
{
	lit = false;
	fusetime = 4.0f;
}

CRIDynamite::~CRIDynamite ( void )
{
	;
}

CRIDynamite::CRIDynamite ( void )
	: CBaseRandomTossable( tTossableProperties(), ItemData() )
{
	lit = false;
	fusetime = 4.0f;
}


void CRIDynamite::Update ( void )
{
	if ( lit )
	{
		fusetime -= Time::deltaTime;
		if ( fusetime <= 0 )
		{
			CTerraExplosion* newExplo = new CTerraExplosion( transform.position, tossable_stats.damage );
			newExplo->Explode();

			CParticleSystem* exploEffect = new CParticleSystem( ".res/particlesystems/explo01.pcf" );
			exploEffect->transform.position = transform.position;
			exploEffect->PostUpdate();
			exploEffect->RemoveReference();

			DeleteObject( this );
		}
	}
}

void CRIDynamite::Generate ( void )
{
	CBaseRandomItem::Generate();

	pModel = new CModel( "models\\items\\dynamite.FBX" );
}

bool CRIDynamite::Use ( int x )
{
	if ( !CanUse( x ) )
		return true;
	if ( x == Item::UPrimary )
	{
		bool result = CBaseRandomTossable::Use( x );
		if ( result ) {
			if ( lastTossed ) {
				((CRIDynamite*)(lastTossed))->lit = true;
			}
		}
		return result;
	}
	else if ( x == Item::USecondary )
	{
		// Er...
		lit = true;
		return true;
	}
	return true;
}