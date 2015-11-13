
#include "BuffRacialKitten.h"

#include "after/entities/character/CCharacter.h"

#include "renderer/state/Settings.h"
#include "after/states/CWorldState.h"

#include "after/entities/world/environment/DayAndNightCycle.h"
#include "engine-common/entities/CRendererHolder.h"
#include "renderer/object/screenshader/effects/CBloomShader.h"

#include "core/math/Math.h"
#include "engine/state/CGameState.h"

BuffRacialKitten::BuffRacialKitten ( CCharacter* inTarget )
	: CCharacterBuff( TYPE_PASSIVE_PLAYER, inTarget )
{
	can_stack = false;
	// Neutral buff
	positive = 2;

	bloomTarget = NULL;
	currentStrength = 1;
}

BuffRacialKitten::~BuffRacialKitten ( void )
{
	;
}

void BuffRacialKitten::Update ( void )
{
	// Add 2 to max health
	//pTarget->stats.fHealthMax += 2;

	//RenderSettings.ambientColor = RenderSettings.ambientColor * 2.0f + Color( 0.1f,0.1f,0.1f );
	ftype calculatedStrength = 0.0f;
	calculatedStrength = Math.Clamp( 0.2f-Daycycle::DominantCycle->GetSunDirection().z*2.0f, 0,1 );

	ftype delta = calculatedStrength-currentStrength;
	ftype changespeed = Time::deltaTime * 0.2f;
	if ( fabs( delta ) < changespeed*1.5f ) {
		currentStrength = calculatedStrength;
	}
	else {
		currentStrength += Math.sgn<ftype>( delta ) * changespeed;
	}

	//ActiveGameWorld->cBaseAmbient = Color( 0.21f,0.18f,0.25f ) * currentStrength;
	ActiveGameWorld->cBaseAmbient = Color( 0.21f,0.18f,0.25f ) * currentStrength;

	// Set bloom values
	if ( !bloomTarget ) {
		// Get reference to current bloom
		bloomTarget = (CBloomShader*)((CRendererHolder*)CGameState::Active()->FindFirstObjectWithName( "Bloom Shader Holder" ))->GetRenderer();
	}
	else {
		if ( Daycycle::DominantCycle )
		{
			// Default values
			//m_threshold = 0.84f;
			//m_gradient	= 1.40f;

			// Set bloom values for dark elves
			/*bloomTarget->SetParameters(
				0.62f - 0.42f * std::max<ftype>( std::min<ftype>( Daycycle::DominantCycle->GetSunDirection().z*5.0f + 0.8f, 1.0f ), 0.0f ),
				(2.0f + std::min<ftype>( Daycycle::DominantCycle->GetSkyobjectDiffuse().red*3.0f, 1.0f ))*0.47f );*/
			bloomTarget->SetParameters( 0.94f - 0.48f * currentStrength, 1.40f + 1.60f * currentStrength );
		}
		/*else 
		{
			bloomTarget->SetParameters( 0.6f, 3.0f );
		}*/
	}

}