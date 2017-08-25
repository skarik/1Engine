
// Unified AI code
// Aggro, Anger, and Alert routines

#include "NPC_AIState.h"

#include "after/entities/CCharacterModel.h"
#include "after/entities/character/npc/zoned/CZonedCharacter.h"

#include "renderer/debug/CDebugDrawer.h"

//#include "engine/physics/raycast/Raycaster.h"
#include "core/math/random/Random.h"

#include "engine-common/lua/CLuaController.h"

#include "after/entities/effects/CFXNotice.h"

#include "core/math/Ray.h"

using std::cout;
using std::endl;

void NPC::AIState::AI_OnAngry ( void )
{
	//cout << "ANGRY!" << endl;
	cout << "! : Angry" << endl;
	
	new CFXNotice(1, owner->GetEyeRay().pos + Vector3d::up*1.4f );

}
void NPC::AIState::AI_OnAlert ( void )
{
	//cout << "alert!" << endl;
	cout << "? : Alert" << endl;

	new CFXNotice(0, owner->GetEyeRay().pos + Vector3d::up*1.4f );
}

void NPC::AIState::AI_GenerateAggro ( ai_memory_target_state_t& o_aggro_state, const bool n_bias )
{
	ftype curAggroAmt = 0.0f;

	// Basis for aggro is of course based off of position
	if ( o_aggro_state.target_valid ) {
		curAggroAmt = (ownerEyeRay.pos - o_aggro_state.target->transform.position).sqrMagnitude() * 0.2f;
	}
	else {
		if ( o_aggro_state.heard.time < o_aggro_state.seen.time ) {
			curAggroAmt = (ownerEyeRay.pos - o_aggro_state.heard.position).sqrMagnitude() * 0.2f;
		}
		else {
			curAggroAmt = (ownerEyeRay.pos - o_aggro_state.seen.position).sqrMagnitude() * 0.2f;
		}
	}
	// Aggro amount increased if has gotten pissed at it
	curAggroAmt -= o_aggro_state.isAngryAt ? 75.0f : 0.0f;
	curAggroAmt -= o_aggro_state.targetSuspicious ? 30.0f : 0.0f;
	curAggroAmt += o_aggro_state.targetAllied ? 30.0f : 0.0f;
	curAggroAmt -= o_aggro_state.targetHostile? 50.0f : 0.0f;
	// The threat level (based on usage of weapons/support skills) is a very large aggro factor, however
	curAggroAmt -= o_aggro_state.threatLevel * 40.0f; // Directly hurting this NPC will grab attention
	// Also, if it's the current target is a very large aggro factor
	curAggroAmt -= (n_bias) ? 120.0f : 0.0f;
	// If we saw it die, or saw the dead body, though, reduce the aggro amount by quite a bit
	curAggroAmt = o_aggro_state.sawDead ? std::max<ftype>(-50,curAggroAmt+100.0f) : curAggroAmt;
	// If we can't see the target at all, reduce the aggro amount slighty
	curAggroAmt += (o_aggro_state.seen.time > 5.0f) ? 20.0f : 0.0f;
	// If we can't hear the target at all, reduce the aggro amount slighty
	curAggroAmt += (o_aggro_state.heard.time > 5.0f) ? 20.0f : 0.0f;

	// Lower aggro means more dangerous.
	// -50 means alert.
	// -125 means dangerous.

	// Apply the new aggro amount
	o_aggro_state.aggroLevel = curAggroAmt;
}

void NPC::AIState::AI_UpdateAggro ( void )
{
	// Update the attack request times
	if ( response_atk.atk_waitForResult )
	{
		response_atk.atk_waittime -= Time::deltaTime;
		if ( response_atk.atk_waittime <= 0 )
		{
			response_atk.atk_waitForResult = false;
		}
	}
	else
	{
		response_atk.atk_waittime = 0;
	}

	// Aggression simulation now
	bool hadAggro = false;
	bool updateAggro = false;
	bool canAnger = false;

	// Update aggro timer so to enable aggro time
	ai_think.aggroTimer += Time::deltaTime;
	if ( ai_think.aggroTimer > ai_think.aggroUpdateTime )
	{
		ai_think.aggroTimer = 0;
		updateAggro = true;
	}

	// Check if current target has died. 
	if ( haveTarget )
	{
		// If it has, then go back to alert.
		if ( ai_think.aggroTargets[ai_think.target].sawDead )
		{
			if ( ai_think.infostate == ai_think.AI_ANGRY ) {
				ai_think.infostate = ai_think.AI_ALERT;
			}
		}
	}

	// Update aggro targets
	if ( updateAggro ) // Targets only update in discrete time segements
	{
		// Reset target if invalid
		if ( ai_think.target >= (signed)ai_think.aggroTargets.size() ) {
			ai_think.target = -1;
		}

		// Loop through characters, search for those in view, and add them to the list
		for ( uint i = 0; i < CCharacter::msList.size(); ++i ) {
			if ( CCharacter::msList[i] != owner ) {
				if ( isInViewAngle( CCharacter::msList[i]->transform.position, ai_lookat.minDotView ) )
				{
					OnAquirePossibleTarget( CCharacter::msList[i] );
				}
			}
		}

		// Find most likely aggro targets 
		for ( uint i = 0; i < ai_think.aggroTargets.size(); ++i )
		{
			AI_GenerateAggro( ai_think.aggroTargets[i], i==ai_think.target );
		}

		// Save current aggro target
		CCharacter* tempSavedTarget = NULL;
		if ( ai_think.target != -1 ) {
			tempSavedTarget = ai_think.aggroTargets[ai_think.target].target;
		}
		// Sort the targets based on their aggro level
		std::sort( ai_think.aggroTargets.begin(),ai_think.aggroTargets.end() );

		// Since the sort changed order, update the target value
		if ( ai_think.target != -1 )
		{
			for ( uint i = 0; i < ai_think.aggroTargets.size(); ++i )
			{
				if ( ai_think.aggroTargets[i].target == tempSavedTarget )
				{
					ai_think.target = i;
					break;
				}
			}
		}

		// Loop through the targets, selecting the first one that looks good
		for ( uint i = 0; i < ai_think.aggroTargets.size(); ++i )
		{
			if ( ai_think.aggroTargets[i].targetAllied == false )
			{
				ai_think.target = i;
			}
			else
			{
				// If friendly with target, but angry at it, and it's this low in the list, it's time to stop a moment.
				if ( ai_think.aggroTargets[i].isAngryAt )
				{
					ai_think.target = -1;
				}
			}
		}
		//
	}

	// Update aggro for whole list
	for ( uint i = 0; i < ai_think.aggroTargets.size(); ++i )
	{
		// Skip all others unless aggro needs a forced update.
		if ( (updateAggro==false) && (i != ai_think.target) ) { // (if not updating aggro and not on target, skip)
			continue;
		}

		ai_memory_target_state_t& aggroTarget = ai_think.aggroTargets[i];

		// Get lookat position for the aggro target. (Grabs the position to raycast to when doing vision checks)
		Ray targetEyeRay;
		if ( aggroTarget.target_valid ) {
			targetEyeRay = aggroTarget.target->GetEyeRay();
		}
		else {
			if ( aggroTarget.heard.time < aggroTarget.seen.time )
			{
				targetEyeRay.pos = aggroTarget.heard.position;
			}
			else {
				targetEyeRay.pos = aggroTarget.seen.position; 
			}
		}

		// Cast out ray to see if the NPC is in range
		bool inViewAngle = isInViewAngle(targetEyeRay.pos,ai_lookat.minDotView);
		//bool inUnobstructedView = isInView(targetEyeRay.pos,ai_think.maxAggroDistance);
		bool inUnobstructedView = isInView(targetEyeRay.pos,160.0f);
		// If been one second and target not shadowed
		if ( aggroTarget.target_valid )
		{
			if ( aggroTarget.seen.time < 1.1f && !aggroTarget.target->IsShadowed() )
			{
				aggroTarget.seen.position = targetEyeRay.pos;
			}
		}

		// Do bothersome checks
		if ( aggroTarget.target_valid && inUnobstructedView )
		{
			if ( (ownerEyeRay.pos-targetEyeRay.pos).magnitude() < ai_think.maxAggroDistance )
			{
				aggroTarget.timeBothered += (i != ai_think.target) ? ai_think.aggroUpdateTime : Time::deltaTime;
			}
			else
			{
				aggroTarget.timeBothered = std::max<ftype>( 0, aggroTarget.timeBothered - ((i != ai_think.target) ? ai_think.aggroUpdateTime : Time::deltaTime) );
			}
		}
		else
		{
			aggroTarget.timeBothered = 0;
		}

		// If can see target, angry at target, angry in general, and target not hiding
		if ( ( aggroTarget.isAngryAt || ai_think.infostate == ai_think.AI_ANGRY || inViewAngle ) && inUnobstructedView )
		{ 
			// If seen, update the seen position!
			aggroTarget.seen.position = targetEyeRay.pos;
			aggroTarget.seen.time = 0;
			// Check if dead
			if ( aggroTarget.target_valid && !aggroTarget.target->IsAlive() )
			{
				aggroTarget.sawDead = true;
				aggroTarget.isAngryAt = false; // can't be angry if its dead
			}
			// If currently angry
			/*if ( ai_think.infostate == ai_think.AI_ANGRY )
			{
				if ( i == ai_think.target )
				{ // If this is the target, then be mad at it
					aggroTarget.isAngryAt = true;
				}
			}*/

			// When angry at target, canAnger is true. Otherwise, we just get alerted by characters with high enough aggro value.

			// Do checks for aggro counter
			bool needsAgression = false;
			bool needsAnger = false;
			switch ( ai_think.aggroType )
			{
			case AGGRO_ANIMAL:	// type 0: default, mostly peaceful animals
				// If was hurt by the ai_think target, then aggro to it	
				if ( aggroTarget.isAngryAt ) {
					if ( ai_think.target == i ) {
						needsAgression = true;
						needsAnger = true;
					}
				}
				// If annoyed by the ai_think target, then aggro to it
				else if ( aggroTarget.timeBothered > 10.0f ) {
					aggroTarget.isAngryAt = true;
					needsAgression = true;
				}
				break;
			case AGGRO_MONSTER: // type 1: enemy default, general enemies
				needsAgression = true;
				needsAnger = true;
				break;
			case AGGRO_CHARACTER: // type 2: character NPCs (don't bother)
				// If target is an animal
				if ( aggroTarget.targetType == 0 || aggroTarget.targetType == 1 )
				{
					// If being attacked by an animal, then attack the animal
					if ( aggroTarget.target_valid && aggroTarget.target->GetCombatTarget() == owner )
					{	// Get angry back at the animal
						aggroTarget.isAngryAt = true;
						needsAgression = true;
					}
				}
				// Check humanoid NPCs for a deathwish.
				else if ( aggroTarget.targetType == 2 )
				{
					// If the target is an enemy, aggro to it like a monster
					if ( aggroTarget.targetHostile )
					{	// Get angry at the target
						aggroTarget.isAngryAt = true;
						needsAgression = true;
					}
					else 
					{ // Act like a normal person
						// If totally angry at this guy, then be angry
						if ( aggroTarget.aggroLevel < info_aggro.angerAggro )
						{
							aggroTarget.isAngryAt = true;
							if ( ai_think.target == i ) {
								needsAgression = true;
							}
						}
						else if ( aggroTarget.aggroLevel < info_aggro.alertAggro )
						{
							aggroTarget.isAngryAt = false;
							if ( ai_think.target == i ) {
								needsAgression = true;
							}
						}
					}
				}
				// Get angry if angry at the character, though.
				if ( aggroTarget.isAngryAt ) {
					needsAnger = true;
				}
				break;
			}

			// If aggression is needed, up the aggro timer
			if ( needsAgression ) {
				hadAggro = true;
			}
			if ( needsAnger ) {
				canAnger = true;
			}
		}
		// Otherwise if target not hiding
		else if ( inUnobstructedView )
		{
			// for now, set heard to same as seen
			aggroTarget.heard.time = 0;
			aggroTarget.heard.position = targetEyeRay.pos;
		}
	}

	// Update Aggression, MGS Style
	AI_Aggro_State( hadAggro, canAnger );
}


void NPC::AIState::AI_Aggro_State ( const bool n_addAggro, const bool n_addAnger )
{
	// Edit aggro timers (that's how it works)
	if ( n_addAggro )
	{
		// Increment aggro counter when getting angry
		if ( ai_think.infostate == ai_think.AI_ALERT || ai_think.infostate == ai_think.AI_SEARCHING )
		{
			if ( n_addAnger ) // Only can get angry at a certain point. Alert should be fairly normal, esp. if characters walk with weapons out.
			{
				ai_think.aggroCounter += Time::deltaTime;
			}
			else 
			{
				ai_think.aggroCounter = 0.0f;
			}
		}
		else
		{
			ai_think.aggroCounter += Time::deltaTime;
		}
		// Decrement peace counter
		ai_think.peaceCounter = std::max<ftype>( ai_think.peaceCounter - Time::deltaTime, 0 );
	}
	else
	{
		// Decrement aggro counter when no targets to aggro to
		ai_think.peaceCounter += Time::deltaTime;
		ai_think.aggroCounter = std::max<ftype>( ai_think.aggroCounter - Time::deltaTime, 0 );
	}

	// Change aggro state based on the counter
	ai_think_state_t::eMainThinkAIState previousState = ai_think.infostate;
	if ( ai_think.infostate == ai_think.AI_RELAXED )
	{
		// Hit anger time
		if ( ai_think.aggroCounter > info_aggro.alertTime )
		{
			ai_think.infostate = ai_think.AI_ALERT; // OR AI_SEARCH!
			AI_OnAlert();
		}
		// Hit cooldown time
		if ( ai_think.peaceCounter > 0.0f )
		{
			ai_think.peaceCounter = 0.0f;
		}
	}
	else if ( ai_think.infostate == ai_think.AI_ALERT || ai_think.infostate == ai_think.AI_SEARCHING )
	{
		// Hit anger time
		if ( ai_think.aggroCounter > info_aggro.angerTime )
		{
			ai_think.infostate = ai_think.AI_ANGRY;
			AI_OnAngry();

			ai_think.aggroCounter = 0.0f;
		}
		// Hit cooldown time
		if ( ai_think.peaceCounter > info_aggro.alertCooldownTime )
		{
			ai_think.infostate = ai_think.AI_RELAXED;
			cout << "- : Not alerted." << endl;
		}
	}
	else if ( ai_think.infostate = ai_think.AI_ANGRY )
	{
		// Hit anger time
		if ( ai_think.aggroCounter > 0.0f )
		{
			ai_think.aggroCounter = 0.0f;
		}
		// Hit cooldown time
		if ( ai_think.peaceCounter > info_aggro.angerCooldownTime )
		{
			ai_think.infostate = ai_think.AI_ALERT; // OR AI_SEARCH!
			cout << "? : Not angry." << endl;
		}
	}

	// If state changed, reset counters
	if ( previousState != ai_think.infostate )
	{
		ai_think.aggroCounter = 0.0f;
		ai_think.peaceCounter = 0.0f;
	}
}


void NPC::AIState::AI_Perform_Angry ( void )
{
	// Check if should do Lua routine
	//if ( !routine_angry_use_c ) {
	{
		//cout << ai_think.state.waitOnAttackResult << " " << ai_think.state.attackResultHit << " pre" << endl;
		Lua::Controller->SetEnvironment( routine_angry_environment.c_str() );
		lua_State* L = Lua::Controller->GetState();
		
		// Check if _ready is false. If _ready is not true, then reinitialize values
		lua_getfield( L, -1, "_ready" );	// table, _ready
		if ( !lua_toboolean( L, -1 ) )		// table
		{	
			lua_pop(L,1);
			lua_pushlightuserdata( L, this );		lua_setfield( L, -2, "targetAI" );
			lua_pushlightuserdata( L, owner );		lua_setfield( L, -2, "mCharacter" );
			lua_pushboolean( L, true );				lua_setfield( L, -2, "_ready" );
		}
		else
		{
			lua_pop(L,1);
		}
		//lua_pushboolean( L, ai_think.isAngry );		lua_setfield( L, -2, "isAngry" );
		//lua_pushboolean( L, ai_think.isAlerted );	lua_setfield( L, -2, "isAlerted" );

		lua_pushinteger( L, ai_think.target );		lua_setfield( L, -2, "aiTarget" );

		lua_newtable( L ); // __ENV, table
			lua_pushnumber( L, info_combat.meleeRange );			lua_setfield( L, -2, "meleeRange" );
			lua_pushnumber( L, info_combat.rangedRange );			lua_setfield( L, -2, "rangedRange" );
			lua_pushnumber( L, info_combat.magicRange );			lua_setfield( L, -2, "magicRange" );
			lua_pushnumber( L, info_combat.hasMeleeWeapon );		lua_setfield( L, -2, "hasMeleeWeapon" );
			lua_pushnumber( L, info_combat.hasRangedWeapon );		lua_setfield( L, -2, "hasRangedWeapon" );
			lua_pushnumber( L, info_combat.hasMagicWeapon );		lua_setfield( L, -2, "hasMagicWeapon" );
			lua_setfield( L, -2, "atkRangeTable" );

		lua_newtable( L ); // __ENV, table
			lua_pushboolean( L, response_atk.atk_hit );				lua_setfield( L, -2, "atk_hit" );
			lua_pushboolean( L, response_atk.atk_performed );		lua_setfield( L, -2, "atk_performed" );
			lua_pushboolean( L, response_atk.atk_waitForResult );	lua_setfield( L, -2, "atk_waitForResult" );
			lua_setfield( L, -2, "atkFeedbackTable" );

		ai_memory_target_state_t& aggroTarget = ai_think.aggroTargets[ai_think.target];
		lua_newtable( L ); // __ENV, table
			lua_pushnumber( L, aggroTarget.seen.time );		lua_setfield( L, -2, "timeSinceSeen" );
			lua_pushnumber( L, aggroTarget.heard.time );	lua_setfield( L, -2, "timeSinceHeard" );
			lua_newtable( L ); // _ENV, table, table
				lua_pushnumber( L, aggroTarget.seen.position.x );	lua_setfield( L, -2, "x" );
				lua_pushnumber( L, aggroTarget.seen.position.y );	lua_setfield( L, -2, "y" );
				lua_pushnumber( L, aggroTarget.seen.position.z );	lua_setfield( L, -2, "z" );
				lua_setfield( L, -2, "lastSeenPosition" );
			lua_newtable( L );
				lua_pushnumber( L, aggroTarget.heard.position.x );	lua_setfield( L, -2, "x" );
				lua_pushnumber( L, aggroTarget.heard.position.y );	lua_setfield( L, -2, "y" );
				lua_pushnumber( L, aggroTarget.heard.position.z );	lua_setfield( L, -2, "z" );
				lua_setfield( L, -2, "lastHeardPosition" );
			lua_setfield( L, -2, "aiTargetTable" );

		// Execute routine
		Lua::Controller->CallPop( "Execute",1, Lua::FN_DOUBLE, (double)Time::deltaTime );
		
		// Grab outputs
		/*lua_getfield( L, -1, "isAngry" );	ai_think.isAngry = lua_toboolean( L,-1 );	lua_pop(L,1);
		lua_getfield( L, -1, "isAlerted" );	ai_think.isAlerted = lua_toboolean( L,-1 );	lua_pop(L,1);
		lua_getfield( L, -1, "aiPerformedAttack" );	ai_think.state.performedAttack = lua_toboolean( L,-1 );		lua_pop(L,1);
		lua_getfield( L, -1, "aiWaitOnAttack" );	ai_think.state.waitOnAttackResult = lua_toboolean( L,-1 );	lua_pop(L,1);
		lua_getfield( L, -1, "aiAttackHit" );		ai_think.state.attackResultHit = lua_toboolean( L,-1 );		lua_pop(L,1);
		lua_getfield( L, -1, "aiWaitResultTimer" );	ai_think.state.waitResultTimer = lua_tonumber( L,-1 );		lua_pop(L,1);*/

		//lua_pop( L, -1 );
		//cout << ai_think.state.waitOnAttackResult << " " << ai_think.state.attackResultHit << " post" << endl;

		Lua::Controller->ResetEnvironment();
		return;
	}
	// ======
	// First check there's a target, if there's no target, fucking leave!
	/*if ( ai_think.target == -1 ) {
		return;
	}
	// Get target
	ai_aggro_info_t& aggroTarget = ai_think.aggroTargets[ai_think.target];

	// Create the seek position
	Vector3d vSeekPosition;
	if ( aggroTarget.timeSinceSeen <= aggroTarget.timeSinceHeard ) {
		vSeekPosition = aggroTarget.lastSeenPosition; 
	}
	else {
		vSeekPosition = aggroTarget.lastHeardPosition;
	}

	// Seek out the target and get close enough to circle it
	if ( ai_think.state.mode == ai_think.state.Seeking )
	{
		ai_think.state.seekOffset = (owner->transform.position-vSeekPosition).normal() * 10.0f; // Move to within ten feet
		vSeekPosition += ai_think.state.seekOffset;

		ai_think.state.circleOffset = ai_think.state.seekOffset; // Set the next circling offset to the target position
		
		// Move to the seeking position
		bool result = AI_MoveTo( vSeekPosition );
		if ( result || ((owner->transform.position-vSeekPosition).magnitude() < 10.0f) ) {
			ai_think.state.mode = ai_think.state.Circling;
		}
	}
	// Attack the target
	else if ( ai_think.state.mode == ai_think.state.Attack )
	{
		if ( !ai_think.state.performedAttack ) {	// Haven't performed attack yet
			// Face the target
			ai_lookat.facingPos = vSeekPosition;
			// Move to target
			ai_think.state.seekOffset = (owner->transform.position-vSeekPosition).normal() * ai_think.attackRange; // Move to within attack range
			vSeekPosition += ai_think.state.seekOffset;
			
			// First move to target position
			bool result = AI_MoveTo( vSeekPosition );
			if ( result ) { // If reached target position, then attack
				if ( AI_Attack() ) {	// Only queue attack if can attack
					ai_think.state.performedAttack = true;
					ai_think.state.waitOnAttackResult = true;
					ai_think.state.waitResultTimer = 2.0f;
				}
			}
		}
		else {
			// Performed attack, wait for the result
			if ( !ai_think.state.waitOnAttackResult ) {
				if ( ai_think.state.attackResultHit ) { // If the attack hit, attack again
					ai_think.state.performedAttack = false;
					ai_think.state.waitOnAttackResult = false;
					ai_think.state.attackResultHit = false;
				}
				else {
					// Go back to circling if the attack missed
					ai_think.state.mode = ai_think.state.Circling;
				}
			}
			else {
				// Don't wait forever.
				ai_think.state.waitResultTimer -= Time::deltaTime;
				if ( ai_think.state.waitResultTimer <= 0 ) { // If waited too long, report failure
					ai_think.state.waitOnAttackResult = false;
					ai_think.state.attackResultHit = false;
				}
			}
		}
	}
	// Circle around the target
	else if ( ai_think.state.mode == ai_think.state.Circling )
	{
		ai_think.state.circleOffset = owner->transform.position - vSeekPosition;	// Circle around the target position
		ai_think.state.circleOffset += ai_think.state.circleOffset.normal().cross( Vector3d::up )
			* 9 * (ftype)ai_think.state.circlingDirection; // Don't multiply by anything to get following

		ftype circlingDistance = ai_think.state.circleOffset.magnitude();	// Limit the range of the circling
		if ( circlingDistance < 9.0f ) {
			ai_think.state.circleOffset = ai_think.state.circleOffset/circlingDistance * 9.0f;
		}
		else if ( circlingDistance > 13.0f ) {
			ai_think.state.circleOffset = ai_think.state.circleOffset/circlingDistance * 13.0f;
		}

		// Add the circling offset to the calculated seek position
		vSeekPosition += ai_think.state.circleOffset;

		// Move to the seeking position
		AI_MoveTo( vSeekPosition );

		// Attack after an amount of time
		ai_think.state.circlingTime += Time::deltaTime;
		if ( ai_think.state.circlingTime > ai_think.state.nextCirclingTime ) {
			ai_think.state.performedAttack = false;
			ai_think.state.waitOnAttackResult = false;
			ai_think.state.attackResultHit = false;
			ai_think.state.circlingTime = 0;
			ai_think.state.circlingDirection = (rand()%2)? -1 : 1;
			ai_think.state.nextCirclingTime = random_range( 2.5f, 5.5f );
			ai_think.state.mode = ai_think.state.Attack;
		}
	}*/
}

void NPC::AIState::AI_Perform_Alert ( void )
{
	// Check if should do Lua routine
	//if ( !routine_alert_use_c ) {
	{
		Lua::Controller->SetEnvironment( routine_alert_environment.c_str() );
		lua_State* L = Lua::Controller->GetState();

		// Check if _ready is false. If _ready is not true, then reinitialize values
		lua_getfield( L, -1, "_ready" );	// table, _ready
		if ( lua_toboolean( L, -1 ) == 0 )	// table
		{	
			lua_pop(L,1);
			lua_pushlightuserdata( L, this );		lua_setfield( L, -2, "targetAI" );
			lua_pushlightuserdata( L, owner );		lua_setfield( L, -2, "mCharacter" );
			lua_pushboolean( L, true );				lua_setfield( L, -2, "_ready" );
		}
		else
		{
			lua_pop(L,1);
		}
		//lua_pushboolean( L, ai_think.isAngry );		lua_setfield( L, -2, "isAngry" );
		//lua_pushboolean( L, ai_think.isAlerted );	lua_setfield( L, -2, "isAlerted" );

		lua_pushinteger( L, ai_think.target );		lua_setfield( L, -2, "aiTarget" );
		
		ai_memory_target_state_t& aggroTarget = ai_think.aggroTargets[ai_think.target];
		lua_newtable( L ); // __ENV, table
			lua_pushnumber( L, aggroTarget.seen.time );		lua_setfield( L, -2, "timeSinceSeen" );
			lua_pushnumber( L, aggroTarget.heard.time );	lua_setfield( L, -2, "timeSinceHeard" );
			lua_newtable( L ); // _ENV, table, table
				lua_pushnumber( L, aggroTarget.seen.position.x );	lua_setfield( L, -2, "x" );
				lua_pushnumber( L, aggroTarget.seen.position.y );	lua_setfield( L, -2, "y" );
				lua_pushnumber( L, aggroTarget.seen.position.z );	lua_setfield( L, -2, "z" );
				lua_setfield( L, -2, "lastSeenPosition" );
			lua_newtable( L );
				lua_pushnumber( L, aggroTarget.heard.position.x );	lua_setfield( L, -2, "x" );
				lua_pushnumber( L, aggroTarget.heard.position.y );	lua_setfield( L, -2, "y" );
				lua_pushnumber( L, aggroTarget.heard.position.z );	lua_setfield( L, -2, "z" );
				lua_setfield( L, -2, "lastHeardPosition" );
			lua_setfield( L, -2, "aiTargetTable" );

		// Execute routine
		int returns = Lua::Controller->Call( "Execute",1, Lua::FN_DOUBLE, (double)Time::deltaTime );
		int result = 0;
		if ( returns == 1 ) {
			result = lua_tointeger( L,-1 );	lua_pop( L,1 );
			Lua::Controller->PopValid( returns-1 );
		}
		else {
			Lua::Controller->PopValid( returns );
			Debug::Console->PrintError( "alerted::Execute needs to return one value!\n" );
		}
		if ( result ) {
			//ai_think.aggroCounter -= Time::deltaTime * 0.2f;
			ai_think.peaceCounter += Time::deltaTime;
		}

		// Grab outputs
		//lua_getfield( L, -1, "isAngry" );	ai_think.isAngry = lua_toboolean( L,-1 );	lua_pop(L,1);
		//lua_getfield( L, -1, "isAlerted" );	ai_think.isAlerted = lua_toboolean( L,-1 );	lua_pop(L,1);

		Lua::Controller->ResetEnvironment();
		return;
	}
	// ======
	// First check there's a target, if there's no target, fucking leave!
	/*if ( ai_think.target == -1 ) {
		return;
	}
	// Get target
	ai_aggro_info_t& aggroTarget = ai_think.aggroTargets[ai_think.target];

	// Create the seek position
	Vector3d vSeekPosition;
	if ( aggroTarget.timeSinceSeen <= aggroTarget.timeSinceHeard ) {
		vSeekPosition = aggroTarget.lastSeenPosition; 
	}
	else {
		vSeekPosition = aggroTarget.lastHeardPosition;
	}

	bool result;
	result = AI_MoveTo( vSeekPosition );
	if ( result ) {
		ai_think.aggroCounter -= Time::deltaTime * 0.2f; // Decrease delta time when can't find things
	}*/
}

void NPC::AIAccessor::PerformAttack ( void )
{
	if ( ai->AI_Attack() )
	{	// Only queue attack if can attack
		/*ai->ai_think.state.performedAttack = true;
		ai->ai_think.state.waitOnAttackResult = true;
		ai->ai_think.state.waitResultTimer = 2.0f;*/
		//ai->response_atk.atk_performed = true;
		ai->response_atk.atk_waittime = true;
		ai->response_atk.atk_performed = true;
		ai->response_atk.atk_waittime = 2.0f;

		/*if ( ai->routine_angry_use_c ) {
			lua_State* L = Lua::Controller->GetState();
			lua_pushboolean( L, ai->ai_think.state.performedAttack );		lua_setfield( L, -2, "aiPerformedAttack" );
			lua_pushboolean( L, ai->ai_think.state.waitOnAttackResult );	lua_setfield( L, -2, "aiWaitOnAttack" );
			lua_pushnumber( L, ai->ai_think.state.waitResultTimer );		lua_setfield( L, -2, "aiWaitResultTimer" );
		}*/
	}
}
void NPC::AIAccessor::PerformDefend ( void )
{
	if ( ai->owner->PerformDefend() ) {
		// bleh.
	}
}

void NPC::AIAccessor::FaceAt ( const Vector3d& vTargetPos )
{
	ai->ai_lookat.walkingFace = false;
	ai->ai_lookat.walkingLookTimer = 2.0f;
	ai->ai_lookat.facingPos = vTargetPos;
}