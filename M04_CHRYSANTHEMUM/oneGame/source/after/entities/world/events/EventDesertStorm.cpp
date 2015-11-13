
#include "EventDesertStorm.h"

#include "after/entities/character/npc/zoned/CZonedCharacterController.h"
#include "after/entities/world/CNpcSpawner.h"
#include "after/types/character/NPC_WorldState.h"
#include "after/interfaces/io/CZonedCharacterIO.h"
#include "after/entities/character/NPC/CNpcBase.h"

#include "core/math/random/Random.h"
#include "engine/state/CGameState.h"

#include "engine-common/entities/CParticleSystem.h"

#include "after/physics/wind/WindMotion.h"

#include "renderer/camera/CCamera.h"

#include "after/types/terrain/WeatherTypes.h"

using namespace GameEvents;

EventDesertStorm::EventDesertStorm ( void ) : CEventBase()
{
	ps = new CParticleSystem( ".res/particlesystems/env/a_desert_duststorm.pcf" );
	lifetime = 0;
	targetLifetime = Random.Range( 60.0f, 120.0f );
	centerOffset = 500.0f;
}


EventDesertStorm::~EventDesertStorm ( void )
{
	delete ps;
	ps = NULL;
}


void EventDesertStorm::Update ( void )
{
	ps->transform.position = CCamera::activeCamera->transform.position + Vector3d( centerOffset,0,15 );

	//WindMotion.GetW
	WindMotion.SetWeather( ps->transform.position, Terrain::WTH_DUST_STORM );

	lifetime += Time::deltaTime;
	if ( lifetime < targetLifetime ) {
		if ( centerOffset > 0 ) {
			centerOffset -= Time::deltaTime * 80.0f;
		}
		else {
			centerOffset = 0;
		}
	}
	else {
		centerOffset -= Time::deltaTime * 80.0f;
		if ( centerOffset < -500 ) {
			DeleteObject( this );
		}
	}
}