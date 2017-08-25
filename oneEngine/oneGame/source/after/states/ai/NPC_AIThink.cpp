
// Unified AI code
// Thinking routines

#include "NPC_AIState.h"

#include "core/math/random/Random.h"
#include "core/system/io/FileUtils.h"
#include "core/debug/CDebugConsole.h"

#include "physical/physics/CPhysics.h"

#include "engine/state/CGameState.h"
#include "engine/physics/raycast/Raycaster.h"

#include "renderer/debug/CDebugDrawer.h"

#include "after/entities/CCharacterModel.h"
#include "after/entities/character/npc/zoned/CZonedCharacter.h"

using std::cout;
using std::endl;
using std::max;
using std::min;

//	Think()
// Main routine for the AI.
void NPC::AIState::Think ( void )
{
	if ( !ai_think.isDead ) // isDead should not be applied in the AI, but in the character
	{
		if ( !owner )
		{
			Debug::Console->PrintError( "INVALID AI OWNER" );
			return;
		}
		AI_Lookat(); // Do lookat commands
		AI_Think(); // Do Target Aquisition, Aggro, Feedback update
		AI_Move(); // Should only do pathfinding, not touch rigidbody.
	}
}


CCharacter* NPC::AIState::QueryAggroTarget ( void ) const
{
	if ( this->haveTarget )
	{
		// Check for an existing target
		if ( ai_think.aggroTargets[ai_think.target].target_valid )
		{
			// If angry at target, set target
			if ( ai_think.aggroTargets[ai_think.target].isAngryAt )
			{
				return ai_think.aggroTargets[ai_think.target].target;
			}
		}
	}
	return NULL;
}


void NPC::AIState::AI_Think ( void )
{
	// Get the eye ray of the owner
	ownerEyeRay = owner->GetEyeRay();
	// Update if have a target
	haveTarget = (ai_think.target >= 0) && (ai_think.target < (signed)ai_think.aggroTargets.size());

	// Increment last seen and last heard timers
	for ( uint i = 0; i < ai_think.aggroTargets.size(); ++i )
	{
		ai_think.aggroTargets[i].seen.time	+= Time::deltaTime;
		ai_think.aggroTargets[i].heard.time	+= Time::deltaTime;

		// Decrease threat level
		ai_think.aggroTargets[i].threatTimer += Time::deltaTime;
		if ( ai_think.aggroTargets[i].threatTimer > 4.0f )
		{
			ai_think.aggroTargets[i].threatLevel -= Time::deltaTime*0.5f;
			if ( ai_think.aggroTargets[i].threatLevel <= 0 )
			{
				ai_think.aggroTargets[i].threatLevel = 0.0f;
			}
		}
	}

	// Check for existance of targets
	for ( auto aggro_it = ai_think.aggroTargets.begin(); aggro_it != ai_think.aggroTargets.end(); )
	{
		// If have a valid pointer, check for invalid pointers
		if ( aggro_it->target_valid )
		{
			// Check for nonexistance
			if ( !CGameState::Active()->ObjectExists( aggro_it->target ) )
			{
				aggro_it->target_valid = false; // Pointer is invalid, mark it such
			}
		}

		// Now, forget about targets
		if ( aggro_it->target_valid )
		{
			if ( aggro_it->sawDead )
			{ // Erase it? No.
				//aggro_it = ai_think.aggroTargets.erase( aggro_it );
				++aggro_it;
			}
			else if ( aggro_it->heard.time > 40.0f && aggro_it->seen.time > 80.0f )
			{
				aggro_it = ai_think.aggroTargets.erase( aggro_it );
			}
			else
			{
				++aggro_it;
			}
		}
		else
		{
			if ( aggro_it->sawDead )
			{
				aggro_it = ai_think.aggroTargets.erase( aggro_it );
			}
			else if ( aggro_it->heard.time > 10.0f && aggro_it->seen.time > 30.0f )
			{
				aggro_it = ai_think.aggroTargets.erase( aggro_it );
			}
			else
			{
				++aggro_it;
			} 
		}
		//
	}

	// If not stunned, do the AI
	if ( !ai_think.isStunned )
	{
		// If have a target, then look at the target, and look for dead things
		if ( ai_think.infostate == ai_think.AI_ANGRY )
		{
			if ( haveTarget )
			{
				ai_memory_target_state_t& aggroTarget = ai_think.aggroTargets[ai_think.target];
				if ( aggroTarget.seen.time < 2.0f )
				{
					ai_lookat.target = aggroTarget.target;
					// Check for nonexistance
					if ( !CGameState::Active()->ObjectExists( aggroTarget.target ) ) {
						aggroTarget.target_valid = false; // Pointer is invalid, mark it such
					}
					if ( aggroTarget.target_valid )
					{
						Ray eyeRay = ai_lookat.target->GetEyeRay();
						// Look at the target
						ai_lookat.lookatPos = eyeRay.pos;
						// Check for death if it's visible
						if ( aggroTarget.seen.time < 0.5f ) {
							if ( !aggroTarget.target->IsAlive() ) {
								aggroTarget.sawDead = true;
							}
							else {
								aggroTarget.sawDead = false; // Because player respawns and we want to be able to keep killing them
							}
						}
					}
					else
					{	// Target is not valid right now, must have died.
						if ( aggroTarget.seen.time < 1.3f ) {
							aggroTarget.sawDead = true;
						}
						// Look at last position for it, anyways.
						ai_lookat.lookatPos = ai_lookat.lookatPos = aggroTarget.seen.position;
						ai_lookat.target = NULL;
					}
				}
				else if ( aggroTarget.seen.time <= aggroTarget.heard.time )
				{
					ai_lookat.lookatPos = aggroTarget.seen.position; 
				}
				else
				{
					ai_lookat.lookatPos = aggroTarget.heard.position;
				}
			}
			ai_lookat.glanceAwayTimer = 5.0f;	// Don't glance away from character for a time
			ai_lookat.glanceAwayAngle = Vector3d::zero;
			ai_lookat.lookAwayTimer = 3.0f; // And please please turn to face the player
		}
		else if ( ai_think.infostate == ai_think.AI_ALERT || ai_think.infostate == ai_think.AI_SEARCHING )
		{
			if ( haveTarget )
			{
				ai_memory_target_state_t& aggroTarget = ai_think.aggroTargets[ai_think.target];
				// Look at the most recent target time
				if ( aggroTarget.seen.time <= aggroTarget.heard.time ) {
					ai_lookat.lookatPos = aggroTarget.seen.position; 
				}
				else {
					ai_lookat.lookatPos = aggroTarget.heard.position;
				}
			}
			ai_lookat.glanceAwayTimer = 5.0f;	// Don't glance away from character for a time
			ai_lookat.glanceAwayAngle = Vector3d::zero;
			ai_lookat.lookAwayTimer = 3.0f; // And please please turn to face the player
		}
		// Do basic AI states
		else if ( ai_think.infostate == ai_think.AI_RELAXED )
		{
			// Is only Idle

			// Look for targets (for now, only player)
			std::vector<CCharacter*> targets;
			targets.push_back( (CCharacter*)owner->GetActivePlayer() );
			
			// Loop through targets
			for ( auto it = targets.begin(); it != targets.end(); ++it )
			{
				CCharacter* possibleTarget = *it;
				// If new target is better (for now, just get the single target)
				if ( (ai_think.target == NIL) && (possibleTarget != NULL) )
				{
					Ray targetEyeRay = possibleTarget->GetEyeRay(); 

					RaycastHit hitResult;	Ray castRay;
					castRay.pos = ownerEyeRay.pos;
					castRay.dir = (targetEyeRay.pos - ownerEyeRay.pos).normal();
					if ( !Raycaster.Raycast( castRay, (targetEyeRay.pos - ownerEyeRay.pos).magnitude(), &hitResult, Physics::GetCollisionFilter( Layers::PHYS_WORLD_TRACE ) ) )
					{
						if ( ai_think.target == NIL )
						{
							// If is a possible target, try to aquire it. AI_UpdateAggro will choose a target.
							OnAquirePossibleTarget( possibleTarget );
						}
					}
				}
			}
		}
		
		bool canBeAngry = true;
		switch ( ai_think.relax_state )
		{
		case ai_think_state_t::Focus:
			// Wander around
			//AI_Wander();
			if ( ai_focus ) {
				canBeAngry = ai_focus->Execute();
			}
			break;
		case ai_think_state_t::Greet:
			if ( ai_think.infostate != ai_think.AI_ANGRY ) {
				AI_Greet();
			}
			break;
		case ai_think_state_t::Talk:
			if ( ai_think.infostate != ai_think.AI_ANGRY ) {
				//AI_Talk();
			}
			break;
		}
		if ( canBeAngry )
		{
			// Perform the other AI
			if ( ai_think.infostate == ai_think.AI_ANGRY )
			{
				AI_Perform_Angry();
			}
			else if ( ai_think.infostate != ai_think.AI_RELAXED )
			{
				AI_Perform_Alert();
			}
		}

		AI_UpdateAggro();
	}


	// If stunned, need to stop moving
	if ( ai_think.isStunned )
	{
		ai_move.state.isRunning = false;
		ai_move.state.isWalking = false;
	}

	// The stun timer needs to decrement
	/*ai_think.stunTimer -= Time::deltaTime;
	if ( ai_think.stunTimer <= 0 )
	{
		if ( ai_think.isStunned )
		{
			//if ( ai_think.target != NULL && ai_think.timeSinceSeen < 5.0f ) {
			if ( ai_think.target != -1 && ai_think.aggroTargets[ai_think.target].timeSinceSeen < 5.0f )
			{
				cout << "! : Angry" << endl;
				if ( !ai_think.isAngry ) {
					AI_OnAngry();
					ai_think.isAngry = true;
				}
			}
			else
			{
				cout << "? : Alert" << endl;
				if ( !ai_think.isAlerted && !ai_think.isAngry )
				{
					AI_OnAlert();
					ai_think.isAlerted = true;
				}
			}
		}

		ai_think.stunTimer = 0;
		ai_think.isStunned = false;
	}*/
	
}


bool NPC::AIState::RequestStunPanic ( void )
{
	// If have target, and saw the target, then we're angry at the fucker.
	if ( haveTarget && ai_think.aggroTargets[ai_think.target].seen.time < 5.0f )
	{
		cout << "! : Angry" << endl;
		if ( ai_think.infostate != ai_think.AI_ANGRY )
		{
			AI_OnAngry();
			ai_think.infostate = ai_think.AI_ANGRY;
			return true;
		}
	}
	else
	{
		cout << "? : Alert" << endl;
		if ( ai_think.infostate == ai_think.AI_RELAXED )
		{
			AI_OnAlert();
			ai_think.infostate = ai_think.AI_ALERT;
			return true;
		}
		else if ( ai_think.infostate != ai_think.AI_ANGRY )
		{
			ai_think.peaceCounter = 0;
			return true;
		}
	}

	return false;
}


// Request a new infostate for the AI_Think module.
bool NPC::AIState::RequestInfoState ( const ai_think_state_t::eMainThinkAIState n_newstate )
{
	// Separate function as it's a request (possibly overridden. In that case, return false)
	ai_think.infostate = n_newstate;

	ai_think.peaceCounter = 0;
	ai_think.aggroCounter = 0;

	return true;
}