#include "gametypeCharview.h"

#include "after/states/CWorldState.h"
#include "after/entities/world/environment/CEnvironmentEffects.h"
#include "after/entities/world/environment/DayAndNightCycle.h"
#include "after/entities/world/environment/CloudSphere.h"

// Gameplay state
void gametypeCharview::Update ( void )
{
	if ( m_worldstate && m_worldeffects )
	{
		m_worldstate->fTimeSpeed = 0;
		m_worldstate->Tick();

		m_worldeffects->pWorldDaycycle->cAmbientOffset = m_worldstate->cBaseAmbient;

		//pWorldDaycycle->SetTimeOfDay( worldState.fCurrentTime );
		m_worldeffects->pWorldDaycycle->SetTimeOfDay( (ftype)m_worldstate->fCurrentTime );
		m_worldeffects->pWorldMooncycle->SetTimeOfCycle( (ftype)m_worldstate->fCurrentTime );
		m_worldeffects->pWorldCloudsphere->SetTimeOfDay( (ftype)m_worldstate->fCurrentTime );
		m_worldeffects->pWorldDaycycle->SetSpaceEffect( 0 ); // For now, turn off space
	}
	// If no event system, make one
	/*if ( World::EventGenerator == NULL ) {
		// Create game event system
		new World::CGameEventGenerator();
		World::EventGenerator->RemoveReference();
	}

	/// Have the terrain follow player as well
	static int ticker = 0;
	if ( COctreeTerrain::GetActive() && CPlayer::GetActivePlayer() && (((++ticker)%10)==0) )
	{
		COctreeTerrain::GetActive()->SetStateFollowTarget( CPlayer::GetActivePlayer()->transform.position );
	}*/
}