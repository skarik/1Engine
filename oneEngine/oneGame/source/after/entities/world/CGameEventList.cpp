
#include "CGameEventGenerator.h"

#include "events/EventBanditSpawnSmall.h"
#include "events/EventAutomatonDesert.h"
#include "events/EventDesertStorm.h"

using namespace GameEvents;

void World::CGameEventGenerator::RegisterGameEvents ( void )
{
	AddGameEvent( "", NULL ); // add invalid event

	AddGameEvent( "bandits small",			&_instGameBehavior<EventBanditSpawnSmall> );
	//AddGameEvent( "desert automatons 1",	&_instGameBehavior<EventAutomatonDesert> );
	AddGameEvent( "desert storm 1",			&_instGameBehavior<EventDesertStorm> );
}