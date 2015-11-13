
#include "NPC_CombatState.h"
#include "core/time/time.h"

// combat here for now
NPC::sCombatInfo::sCombatInfo ( void )
{
	timeInState = 0.0f;
	state = RELAXED;
}
void NPC::sCombatInfo::Update ( void )
{
	timeInState += Time::deltaTime;
	if ( state == COMBAT ) {
		if ( timeInState > 8.0f ) {
			state = GUARDED;
			timeInState = 0.0f;
		}
	}
	else if ( state == GUARDED ) {
		if ( timeInState > 5.0f ) {
			state = RELAXED;
			timeInState = 0.0f;
		}
	}
}
void NPC::sCombatInfo::OnSheathe ( void )
{
	if ( state == COMBAT ) {
		state = GUARDED;
		timeInState = 0.0f;
	}
}
void NPC::sCombatInfo::OnAttack ( void ) 
{
	state = COMBAT;
	timeInState = 0.0f;
}
void NPC::sCombatInfo::OnDraw ( void ) 
{
	if ( state == RELAXED ) {
		state = GUARDED;
		timeInState = 0.0f;
	}
}
void NPC::sCombatInfo::OnDefend ( void )
{
	if ( state == RELAXED ) {
		state = GUARDED;
		timeInState = 0.0f;
	}
}
void NPC::sCombatInfo::OnAttacked ( void )
{
	if ( state == RELAXED ) {
		state = GUARDED;
		timeInState = -2.0f;
	}
	/*else if ( state == GUARDED ) {
		state = COMBAT;
		timeInState = 0.0f;
	}*/
}
