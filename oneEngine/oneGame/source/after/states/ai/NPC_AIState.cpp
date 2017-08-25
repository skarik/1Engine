
// Unified AI code
// General usage routines

#include "NPC_AIState.h"

#include "core/math/random/Random.h"
#include "core/system/io/FileUtils.h"
#include "core-ext/system/io/Resources.h"

#include "physical/physics/CPhysics.h"

#include "engine/physics/raycast/Raycaster.h"
#include "engine/state/CGameState.h"

#include "renderer/debug/CDebugDrawer.h"

#include "after/entities/CCharacterModel.h"
#include "after/entities/character/npc/zoned/CZonedCharacter.h"

using std::cout;
using std::endl;
using std::max;
using std::min;

NPC::AIState::AIState ( void )
{
	ai_think.aggroUpdateTime = 1.0f;
	ai_think.aggroTimer = ai_think.aggroUpdateTime;

	ai_think.target		= -1;

	ai_think.infostate	= ai_think.AI_RELAXED;

	ai_think.aggroCounter	= 0;
	ai_think.peaceCounter	= 0;

	ai_think.isDead		= false;
	ai_think.isStunned	= false;

	/*ai_think.state.mode = ai_think.state.Seeking;

	ai_think.state.seekOffset	= Vector3d();
	ai_think.state.circleOffset	= Vector3d();

	ai_think.state.circlingTime = 0;
	ai_think.state.circlingDirection = ((rand()%2) ? -1 : 1); // either -1 or 1
	ai_think.state.nextCirclingTime = 4.0f;*/

	//ai_think.attackRange = 6.0f;
	info_combat.meleeRange = 6.0f;
	info_combat.rangedRange = 16.0f;
	info_combat.magicRange = 16.0f;

	info_combat.hasMeleeWeapon = true;
	info_combat.hasRangedWeapon = false;
	info_combat.hasMagicWeapon = false;

	//info_aggro.alertAggro = -72;
	info_aggro.alertAggro = -124;
	info_aggro.angerAggro = -125;

	info_aggro.alertCooldownTime = 10.0f;
	info_aggro.alertTime = 1.0f;

	info_aggro.angerCooldownTime = 10.0f;
	info_aggro.angerTime = 1.0f;

	// init AI state constants
	ai_think.aggroType = AGGRO_ANIMAL;
	ai_think.maxAggroDistance = 12.0f;

	// init AI lookat values
	ai_lookat.target	= NULL;
	ai_lookat.targetInRange	= false;
	ai_lookat.targetInView	= false;

	ai_lookat.isLookingAtCharacter = false;
	ai_lookat.isTalkingAtCharacter = false;

	ai_lookat.lookatPos			= Vector3d();
	ai_lookat.lookAwayTimer		= 0;
	ai_lookat.lookatOverride	= 0;

	ai_lookat.glanceAwayAngle	= Vector3d();
	ai_lookat.glanceAwayTimer	= 0;

	ai_lookat.facingPos			= Vector3d();

	ai_lookat.currentGlanceAwayAngle = Vector3d();
	ai_lookat.currentLookatPos	= Vector3d();
	ai_lookat.currentFacingPos	= Vector3d();

	ai_lookat.walkingLookatPos	= Vector3d();
	ai_lookat.walkingFacingPos	= Vector3d();
	ai_lookat.walkingLook		= false;
	ai_lookat.walkingFace		= false;
	ai_lookat.walkingLookTimer	= 0;

	ai_lookat.lookAwayWanderTimer	= 0;
	ai_lookat.lookAwayWanderOffset	= Vector3d();

	// init AI lookat constants
	ai_lookat.maxTalkDistance	= 16.0f;
	ai_lookat.minDotView		= 0.1f; // This is around 170 degrees fov, human fov should be 0.25 for around 150 degrees

	// init AI move values
	ai_move.state.isRunning = false;
	ai_move.state.isWalking = false;
	ai_move.state.wantsCrouch = false;
	ai_move.state.wantsProne	= false;
	ai_move.wants_reset_timer = 0;

	ai_move.target		= Vector3d( 0,0,0 );
	ai_move.validTarget	= false;

	ai_move.path_pos	= 0;
	ai_move.path_target = ai_move.target;
	ai_move.path_time	= -1.0f;
	ai_move.path.clear();

	// init AI move constants
	//ai_move.walkSpeed = 6.3f;
	//ai_move.runSpeed = 6.3f;

	// init AI wander values
	ai_wander.wanderHeading			= Random.PointOnUnitSphere();
	ai_wander.wanderHeadingOffset	= ai_wander.wanderHeading;
	ai_wander.avoidingObstacle = false;
	ai_wander.avoidDirection = ((rand()%2) ? -1 : 1); // either -1 or 1
	ai_wander.avoidTime = 0;

	// init AI greet values
	ai_think.relax_state = ai_think_state_t::Focus;
	ai_talk.greetTimer	= 0;
	ai_talk.followTarget= NULL;

	// init AI focus
	ai_focus = NULL;

	// init AI routines
	routine_alert = "default_alert";
	//routine_alert_use_c = true;
	routine_angry = "default_angry";
	//routine_angry_use_c = true;
}

NPC::AIState::~AIState ( void )
{
	try
	{
		for ( uint i = 0; i < ai_think.aggroTargets.size(); ++i ) {
			if ( CGameState::Active()->ObjectExists( ai_think.aggroTargets[i].target ) ) {
				ai_think.aggroTargets[i].target->RemoveNotiftyWhenAttack( this );
			}
		}
	}
	catch (...) {
		cout << "error in AI state free" << endl;
	}
}

#include "engine-common/lua/CLuaController.h"

//	Initialize()
// Startup routine for the AI. Should be called after all the custom options have been set.
void NPC::AIState::Initialize ( void )
{
	// Give AI_Move proper values to start with
	if ( owner ) {
		ai_move.target		= owner->transform.position;
	}
	else {
		ai_move.target		= Vector3d( 0,0,0 );
	}

/*
	if ( routine_alert.length() > 0 ) {
		routine_alert_use_c = false;
	}
	if ( routine_angry.length() > 0 ) {
		routine_angry_use_c = false;
	}
	*/
	// If using lua scripts for behavior, load them in as objects.
	//if ( !routine_alert_use_c )
	{
		arstring<256> environment;
		arstring<256> file;

		sprintf( environment, "AIR_%s", routine_alert.c_str() );
		sprintf( routine_alert_environment, "AIR_%s_%x", routine_alert.c_str(), this );
		sprintf( file, "lua/ai/attack/%s.lua", routine_alert.c_str() );
		///printf( "%s %s %s %s", routine_alert.c_str(), file.c_str(), routine_alert_environment.c_str(), environment.c_str() );
		file = Core::Resources::PathTo( file ).c_str();
		if ( !IO::FileExists(file) ) {
			throw "No AI file of given alert type exists!";
		}

		sprintf( file, "ai/attack/%s.lua", routine_alert.c_str() );

		//Lua::Controller->LoadLuaFile( file.c_str(), environment.c_str(), routine_alert_environment.c_str() );
		Lua::Controller->LoadLuaFile( file.c_str(), routine_alert_environment.c_str(), routine_alert_environment.c_str() );

		Lua::Controller->SetEnvironment( routine_alert_environment.c_str() );
		Lua::Controller->CallPop( "Initialize",0 );
		Lua::Controller->ResetEnvironment();
	}
	//if ( !routine_angry_use_c )
	{
		arstring<256> environment;
		arstring<256> file;

		sprintf( environment, "AIR_%s", routine_angry.c_str() );
		sprintf( routine_angry_environment, "AIR_%s_%x", routine_angry.c_str(), this );
		sprintf( file, "lua/ai/attack/%s.lua", routine_angry.c_str() );

		file = Core::Resources::PathTo( file ).c_str();
		if ( !IO::FileExists(file) ) {
			throw "No AI file of given angry type exists!";
		}

		sprintf( file, "ai/attack/%s.lua", routine_angry.c_str() );

		//Lua::Controller->LoadLuaFile( file.c_str(), environment.c_str(), routine_angry_environment.c_str() );
		Lua::Controller->LoadLuaFile( file.c_str(), routine_angry_environment.c_str(), routine_angry_environment.c_str() );

		Lua::Controller->SetEnvironment( routine_angry_environment.c_str() );
		Lua::Controller->CallPop( "Initialize",0 );
		Lua::Controller->ResetEnvironment();
	}

}


void NPC::AIState::AI_Wander ( void )
{
	//AI_Follow();
	//return;

	// Generate randomized heading
	ai_wander.wanderHeadingOffset += Random.PointOnUnitSphere() * Time::deltaTime * 4.0f;
	ai_wander.wanderHeadingOffset.normalize();

	ai_wander.wanderHeading += ai_wander.wanderHeadingOffset * Time::deltaTime;
	ai_wander.wanderHeading.z *= 0.5f;
	ai_wander.wanderHeading.normalize();

	// Move
	if ( !ai_wander.avoidingObstacle ) {
		AI_MoveTo( owner->transform.position + ai_wander.wanderHeading*32.0f );

		// Look at wander heading
		if ( !ai_lookat.target ) {
			ai_lookat.lookatPos = owner->transform.position + ai_wander.wanderHeading * 60.0f;
		}

		// Check for begin avoid
		// If there's a wall in front
		Ray castRay;
		castRay.pos = owner->transform.position + Vector3d( 0,0,3.0f );
		castRay.dir = ((ai_wander.wanderHeading*80.0f+owner->transform.position)-castRay.pos).normal();
		RaycastHit hitInfo;
		if ( Raycaster.Raycast( castRay, hull.radius*1.2f, &hitInfo, Physics::GetCollisionFilter( Layers::PHYS_WORLD_TRACE ) ) )
		{
			ai_wander.avoidTime += Time::deltaTime;
			// And been banging head against the wall for a solid half second
			if ( ai_wander.avoidTime >= 0.5f )
			{
				ai_wander.avoidTime = 0;
				//ai_wander.avoidDirection = (rand()%2) ? -1 : 1;
				ai_wander.avoidingObstacle = true;
			}
		}
		else {
			ai_wander.avoidTime = max<ftype>( 0.0f, ai_wander.avoidTime - Time::deltaTime );
		}
	}
	else {
		// Generate move direction based on wander position
		Vector3d targetHeading = Vector3d::up.cross( ai_wander.wanderHeading ) * (ftype)ai_wander.avoidDirection;

		AI_MoveTo( owner->transform.position + targetHeading*32.0f );

		// Look at main wander heading lerped with new heading
		if ( !ai_lookat.target ) {
			ai_lookat.lookatPos = owner->transform.position + (ai_wander.wanderHeading + targetHeading) * 30.0f;
		}

		// Go back to normal movement after five seconds
		ai_wander.avoidTime += Time::deltaTime;
		if ( ai_wander.avoidTime > 5.0f ) {
			ai_wander.avoidingObstacle = false;
		}

		// Check for begin avoid
		// If there's a wall in front
		Ray castRay;
		castRay.pos = owner->transform.position + Vector3d( 0,0,3.0f );
		castRay.dir = ((targetHeading*80.0f+owner->transform.position)-castRay.pos).normal();
		RaycastHit hitInfo;
		if ( Raycaster.Raycast( castRay, hull.radius*1.2f, &hitInfo, Physics::GetCollisionFilter( Layers::PHYS_WORLD_TRACE ) ) )
		{	// Flip direction
			ai_wander.avoidDirection = -ai_wander.avoidDirection;
		}
	}
}
void NPC::AIState::AI_Follow ( void )
{
	if ( ai_talk.followTarget ) {
		// If too far away from follow target, then go to it.
		const ftype followDistance = 7.0f;
		const ftype followToDistance = 3.0f;
		ftype distanceToFollowTarget = (ai_talk.followTarget->transform.position - owner->transform.position).magnitude();
		if ( distanceToFollowTarget > followDistance )
		{
			if ( (distanceToFollowTarget > followDistance*1.8f) || (ai_wander.wanderHeading-ai_talk.followTarget->transform.position).magnitude() > followDistance ) {
				ai_wander.wanderHeading = ai_talk.followTarget->transform.position + (owner->transform.position-ai_talk.followTarget->transform.position).normal()*followToDistance;
			}
		}
		AI_MoveTo( ai_wander.wanderHeading, true );
	}
	else {
		AI_MoveTo( owner->transform.position );
	}
}


void NPC::AIState::AI_Greet ( void )
{
	// Look at target
	// if (wandering) get back to busines
	{
		ai_talk.greetTimer += Time::deltaTime;
		if ( ai_talk.greetTimer > 10.0f || !ai_lookat.isTalkingAtCharacter ) {
			ai_think.relax_state = ai_think_state_t::Focus;
			ai_lookat.isTalkingAtCharacter = false;
		}
	}
}

bool NPC::AIState::RequestActorInteract ( CCharacter* interactingCharacter, const bool n_interact )
{
	// Basically, you can distract characters who aren't angry.
	if ( ai_think.infostate == ai_think.AI_RELAXED || ai_think.infostate == ai_think.AI_ALERT || ai_think.infostate == ai_think.AI_SEARCHING )
	{
		ai_lookat.target = interactingCharacter;
		Ray eyeRay = ai_lookat.target->GetEyeRay();

		// Turn and look at the character
		ai_lookat.facingPos = eyeRay.pos;
		ai_lookat.lookatPos = ai_lookat.facingPos;

		// Don't glance away from character for a time (5 seconds)
		ai_lookat.glanceAwayTimer = 5.0f;
		ai_lookat.glanceAwayAngle = Vector3d::zero;

		// And please please turn to face the player
		ai_lookat.lookAwayTimer = 3.0f;

		// Now talking at character
		ai_lookat.isTalkingAtCharacter = true;
		ai_lookat.isLookingAtCharacter = true;

		if ( n_interact )
		{
			// Apply the greeting code
			OnGreet(ai_lookat.target);
		}
		return true;
	}
	return false;
}
void NPC::AIState::OnGreet ( CCharacter* pCharacter )
{
	if ( pCharacter )
	{
		if ( ai_think.relax_state != ai_think_state_t::Greet )
		{
			ai_think.relax_state = ai_think_state_t::Greet;
			ai_talk.greetTimer = 0;
			ai_lookat.isTalkingAtCharacter = true;
			AI_MoveTo( owner->transform.position );
		}
		else
		{
			// Go to talk state!
			ai_talk.greetTimer = 0;
			ai_lookat.isTalkingAtCharacter = true;
			AI_MoveTo( owner->transform.position );

			// Call talk on owner
			owner->OnTalkTo( pCharacter );

			// for now, just follow
			ai_talk.followTarget = ai_lookat.target;
		}
	}
}


bool NPC::AIState::AI_Attack ( void ) // TODO: Add cooldown (srsly)
{
	return owner->PerformAttack();
}


void NPC::AIState::OnGetDamaged ( Damage const& hitDamage )
{
	if ( hitDamage.actor )
	{
		ai_lookat.lookatPos = hitDamage.actor->transform.position;

		//ai.ai_think.aggroTarget = (CCharacter*)hitDamage.actor;
		//ai.ai_think.aggroAnger = true;
		// TODO: ADD AGGRO TARGET
		OnAquirePossibleTarget( (CCharacter*)hitDamage.actor, ai_think.infostate != ai_think.AI_ANGRY );
			
		// If damage source is visible or close enough
		if ( isInViewAngle( hitDamage.actor->transform.position,ai_lookat.minDotView ) || ((hitDamage.actor->transform.position - owner->transform.position).magnitude() < ai_think.maxAggroDistance) )
		{
			// Do angry swaps
			if ( !owner->IsAlliedToCharacter( (CCharacter*)hitDamage.actor ) )
			{
				// When attacker is not allied, get angry
				ai_think.aggroTimer = ai_think.aggroUpdateTime;
				if ( ai_think.infostate != ai_think.AI_ANGRY )
				{
					ai_think.infostate = ai_think.AI_ANGRY;
					AI_OnAngry();
				}
				ai_think.aggroCounter = 0.5f * info_aggro.angerTime;
				ai_think.peaceCounter = 0;
				// Get angry at this specific target
				for ( uint i = 0; i < ai_think.aggroTargets.size(); ++i )
				{
					if ( ai_think.aggroTargets[i].target == hitDamage.actor )
					{
						ai_think.aggroTargets[i].isAngryAt = true;
					}
				}
			}
		}
		// Damage source is not visible
		else
		{
			// Set to angry, and force a reaction (it should start attacking the most likely target)
			ai_think.aggroTimer = ai_think.aggroUpdateTime;
			if ( ai_think.infostate != ai_think.AI_ANGRY )
			{
				ai_think.infostate = ai_think.AI_ANGRY;
				AI_OnAngry();
			}
			ai_think.aggroCounter = 0.5f * info_aggro.angerTime;
			ai_think.peaceCounter = 0;
			// Should set to angry, force a reaction, but set most likely place to look
			ai_lookat.facingPos = hitDamage.actor->transform.position;
		}
	}
	else
	{
		ai_lookat.lookatPos = owner->transform.position + (hitDamage.direction.normal() * ai_think.maxAggroDistance);
		// Set to angry, and force a reaction (it should start attacking the most likely target)
		ai_think.aggroTimer = ai_think.aggroUpdateTime;
		if ( ai_think.infostate != ai_think.AI_ANGRY )
		{
			ai_think.infostate = ai_think.AI_ANGRY;
			AI_OnAngry();
		}
		ai_think.aggroCounter = 0.5f * info_aggro.angerTime;
		ai_think.peaceCounter = 0;
		// Should set to angry, force a reaction, but set most likely place to look
		ai_lookat.facingPos = owner->transform.position + (hitDamage.direction.normal() * ai_think.maxAggroDistance);
	}
}

void NPC::AIState::OnAquirePossibleTarget ( CCharacter* character, bool nforceAggro )
{
	if ( character == NULL ) {
		return;
	}
	// ADD TO LIST
	for ( uint i = 0; i < ai_think.aggroTargets.size(); ++i ) 
	{
		if ( ai_think.aggroTargets[i].target == character ) {
			if ( nforceAggro ) {
				ai_think.aggroTargets[i].isAngryAt = true;
			}
			return; // NOTHING TO DO HERE.
		}
	}
	//cout << "Aquired possible target: " << character->GetTypeName() << " " << character->GetName() << endl;

	// ADD TO LIST SINCE NOT IN LIST
	ai_memory_target_state_t newAggro;
	newAggro.target = character;
	newAggro.target_valid = true;

	newAggro.targetAllied = owner->IsAlliedToCharacter( character );
	newAggro.targetHostile = owner->IsEnemyToCharacter( character );
	newAggro.targetType = character->GetAggroType();

	newAggro.isAngryAt = nforceAggro;
	newAggro.targetSuspicious = false;
	newAggro.sawDead = false;

	newAggro.seen.position = character->transform.position;
	newAggro.heard.position = character->transform.position;
	newAggro.seen.time = 0;
	newAggro.heard.time = 0;

	newAggro.timeBothered = 0;

	newAggro.threatLevel = 0;
	newAggro.aggroLevel = 0;
	newAggro.aggroCounter = 0;

	// Add this AI to be notified when this character attacks
	newAggro.target->AddNotifyWhenAttack( this, (arCallback_Clb_Ptr)&AIState::cb_AIState_OnTargetAttack );

	ai_think.aggroTargets.push_back( newAggro );
	//cout << "Added target to list." << endl;
}

// Callback for on target attack. Takes a void* of the character.
int NPC::AIState::cb_AIState_OnTargetAttack ( void* ncharacter )
{
	CCharacter* character = (CCharacter*)ncharacter;

	// Get the aggro target
	uint target = -1;
	for ( uint i = 0; i < ai_think.aggroTargets.size(); ++i ) {
		if ( ai_think.aggroTargets[i].target == character ) {
			target = i;
			break;
		}
	}
	if ( target == -1 ) {
		return -1;
	}
	ai_memory_target_state_t& aggroTarget = ai_think.aggroTargets[target];
	
	// Get lookat position for tha aggro target.
	Ray targetEyeRay = aggroTarget.target->GetEyeRay();

	// Check if target is in view
	if ( isInViewAngle(targetEyeRay.pos,ai_lookat.minDotView) ) {
		// Increase the threat of it
		aggroTarget.threatLevel = 2.0f;
		aggroTarget.threatTimer = 0.0f;
	}

	// No issues
	return 0;
}

//
void NPC::AIState::PartyCmdFollowMe ( CCharacter* ncharacter )
{
	if ( ai_focus != NULL ) {
		ai_focus->PartyCmdFollowMe( ncharacter );
	}
}
void NPC::AIState::PartyCmdMoveTo ( const Vector3d & nposition )
{
	if ( ai_focus != NULL ) {
		ai_focus->PartyCmdMoveTo( nposition );
	}
}