
#include "EventAutomatonDesert.h"

#include "after/entities/character/npc/zoned/CZonedCharacterController.h"
#include "after/entities/world/CNpcSpawner.h"
#include "after/types/character/NPC_WorldState.h"
#include "after/interfaces/io/CZonedCharacterIO.h"
#include "after/entities/character/NPC/CNpcBase.h"

#include "core/math/random/Random.h"
#include "engine/state/CGameState.h"

using namespace GameEvents;

EventAutomatonDesert::EventAutomatonDesert ( void ) : CEventBase()
{
	for ( uint i = 0; i < 2; ++i )
	{
		Vector3d spawnPosition;
		NPC::CZonedCharacter* character;

		bool success = false;
		while ( !success ) {
			spawnPosition = NPC::Spawner->GetSpawnPosition( m_focus_player_actor->transform.position, success );
		}
		// Now, change bandit's equipment 
		character = NPC::Manager->SpawnFauna( "RobotAutomaton", spawnPosition );

		// Assign main bandit id to check when should call event over
		mainBanditId = character->GetId();
	}
}


EventAutomatonDesert::~EventAutomatonDesert ( void )
{

}


void EventAutomatonDesert::Update ( void )
{
	if ( CGameState::Active()->GetBehavior( mainBanditId ) == NULL ) {
	//	DeleteObject( this );
	}
}