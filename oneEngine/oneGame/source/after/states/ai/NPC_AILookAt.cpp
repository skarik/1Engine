
// Unified AI code
// Lookat Routines

#include "NPC_AIState.h"

#include "core/math/Math.h"
#include "core/math/random/Random.h"
#include "core/system/io/FileUtils.h"

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

//
// ai_lookat_properties_t settings
// Initial values for lookat
NPC::ai_lookat_properties_t::ai_lookat_properties_t ( void )
	:
	glanceAway(true), glanceAwayStart(5.0f), glanceAwayRate(1.0f),
	lookAway(true), lookAwayStart(2.0f), lookAwayRate(1.0f),
	headTurnSpeed(2.0f), eyeTurnSpeed(8.0f)
{
	;
}
//


// Checks
bool NPC::AIState::isInView ( const Vector3d& nTargetPos, const ftype nViewDistance )
{
	Ray castRay; RaycastHit hitResult;
	castRay.pos = ownerEyeRay.pos;
	castRay.dir = (nTargetPos - ownerEyeRay.pos).normal();
	return  ( (nTargetPos-ownerEyeRay.pos).sqrMagnitude() < sqr(nViewDistance) )
			&& ( !Raycaster.Raycast( castRay, (nTargetPos - ownerEyeRay.pos).magnitude(), &hitResult, Physics::GetCollisionFilter( Layers::PHYS_WORLD_TRACE ) ) );
}
bool NPC::AIState::isInViewAngle ( const Vector3d& nTargetPos, const ftype nViewAngleDot )
{
	return (ownerEyeRay.dir.dot( (nTargetPos-ownerEyeRay.pos).normal() ) > nViewAngleDot);
}

// Return the current facing target
Vector3d NPC::AIState::QueryFacingPosition ( void ) const
{
	return ai_lookat.facingPos;
}
// Return the current facing target
Vector3d NPC::AIState::QueryLookatPosition ( void ) const
{
	return ai_lookat.lookatPos;
}
// Return the current facing target
Vector3d NPC::AIState::QueryEyeGlanceAngles ( void ) const
{
	return ai_lookat.currentGlanceAwayAngle;
}

// Request a position to look at, and override the AI lookat position
// Returns true pretty much always, unless stunned.
bool NPC::AIState::RequestLookatOverride ( const Vector3d& n_targetPosition )
{
	if ( !ai_think.isStunned )
	{
		ai_lookat.lookatPos = n_targetPosition;
		ai_lookat.lookatOverride = 1;
		return true;
	}
	return false;
}

void NPC::AIState::AI_Lookat ( void )
{
	if ( ai_lookat.target ) {
		if ( !CGameState::Active()->ObjectExists( ai_lookat.target ) ) {
			ai_lookat.target = NULL;
		}
	}
	if ( !ai_lookat.target ) {
		ai_lookat.target = (CCharacter*)owner->GetActivePlayer();
	}

	ai_lookat.targetInView = false;
	ai_lookat.targetInRange = false;

	// Check for if target is in view
	if ( ai_lookat.target )
	{
		Ray eyeRay = owner->GetEyeRay();
		Ray targetEyeRay = ai_lookat.target->GetEyeRay();
		if ( (targetEyeRay.pos-eyeRay.pos).sqrMagnitude() < sqr(ai_lookat.maxTalkDistance) ) {
			ai_lookat.targetInRange = true;
			if (  ( (targetEyeRay.pos-eyeRay.pos).sqrMagnitude() < sqr(2) )
				||( eyeRay.dir.dot( (targetEyeRay.pos-eyeRay.pos).normal() ) > ai_lookat.minDotView ))
			{
				ai_lookat.targetInView = true;
			}
		}
	}
	
	if ( !ai_lookat.targetInRange ) {	// Seriously, if the character is far enough
		ai_lookat.isTalkingAtCharacter = false;	// THEN DON'T GIVE A FUCCCKKKKK
	}

	// look at follow code
	ai_lookat.lookatOverride -= Time::deltaTime;

	if ( ai_lookat.isLookingAtCharacter )
	{
		if ( ai_lookat.lookatOverride <= 0 ) {
			if ( ai_lookat.target ) {
				ai_lookat.lookatPos = ai_lookat.target->GetEyeRay().pos;
				/*if ( ai_lookat.glanceAwayTimer > 0 ) { // FAce character for a moment
					ai_lookat.facingPos = ai_lookat.lookatPos;
				}*/
			}
		}

		// Check if person not in view
		if ( !ai_lookat.targetInView ) {
			if ( ai_lookat.isTalkingAtCharacter ) {
				// Turn to face the character
				if ( ai_lookat.target ) {
					ai_lookat.facingPos = ai_lookat.target->GetEyeRay().pos;
				}
			}
			else {
				ai_lookat.isLookingAtCharacter = false;
				ai_lookat.lookAwayTimer = 2.0f;
			}
		}

		// Face walking direction when moving and not talking to someone, though switch between looking at people and move target
		if ( ai_move.state.isRunning || ai_move.state.isWalking ) {
			// Change what we're doing when time has passed
			ai_lookat.walkingLookTimer -= Time::deltaTime;
			if ( ai_lookat.walkingLookTimer <= 0 ) {
				// Toggle lookat state
				ai_lookat.walkingLook = !ai_lookat.walkingLook;
				ai_lookat.walkingFace = !ai_lookat.walkingFace;
				// But bias it towards not looking where walking
				if ( Random.Next()%3 == 0 ) {
					ai_lookat.walkingLook = false;
				}
				if ( Random.Next()%3 == 0 ) {
					ai_lookat.walkingFace = false;
				}
				// Set timer, again biased to not looking where walking
				if ( ai_lookat.walkingLook ) {
					ai_lookat.walkingLookTimer = random_range(1.0f,3.0f);
				}
				else {
					ai_lookat.walkingLookTimer = random_range(3.0f,5.0f);
				}
			}
			if ( ai_lookat.walkingLook && !ai_lookat.isTalkingAtCharacter ) {
				if ( ai_lookat.lookatOverride <= 0 ) {
					ai_lookat.lookatPos = ai_lookat.walkingLookatPos;
				}
			}
			if ( ai_lookat.walkingFace ) {
				Vector3d horizontalDelta = owner->transform.position - ai_lookat.walkingFacingPos;
				horizontalDelta.z = 0;
				Real horizontalDistance = horizontalDelta.magnitude();
				if ( horizontalDistance > hull.radius+0.4f ) {
					ai_lookat.facingPos = ai_lookat.walkingFacingPos;
				}
				else {
					ai_lookat.facingPos = owner->transform.position + owner->transform.rotation * Vector3d( 1,0,0 ) * ( hull.radius+3.5f );
				}
			}
		}
	}
	else
	{
		// Look at our things
		Ray eyeRay = owner->GetEyeRay();
		XTransform headTransform;
		owner->GetCharModel()->GetHeadTransform( headTransform );
		// Look back forwards when time has passed
		ai_lookat.lookAwayTimer -= Time::deltaTime;
		if ( ai_lookat.lookAwayTimer <= 0 ) {
			// Face forward
			//ai_lookat.facingPos = transform.position + transform.rotation * -Vector3d::left * 24 + Vector3d( 0,0,3 );
			// Don't change facing pos
			//DebugD::DrawLine( transform.position, transform.position + transform.rotation * Vector3d::left * 4 );
			//ai_lookat.lookatPos = eyeRay.pos + eyeRay.dir*8;
			if ( ai_lookat.lookatOverride <= 0 ) {
				if ( (Vector2d(ai_lookat.facingPos.x,ai_lookat.facingPos.y) - Vector2d(owner->transform.position.x,owner->transform.position.y)).magnitude() < 0.3f )
				{
					ai_lookat.facingPos = headTransform.position + owner->transform.rotation * Vector3d( 1,0,0 ) * ( hull.radius+3.5f );
				}
				//ai_lookat.lookatPos = ai_lookat.lookatPos.lerp( eyeRay.pos + Vector3d(eyeRay.dir.x,eyeRay.dir.y,eyeRay.dir.z*0.1f)*13, 0.2f );
				//ai_lookat.lookatPos = eyeRay.pos + Vector3d(eyeRay.dir.x,eyeRay.dir.y,eyeRay.dir.z*0.1f)*13;
				ai_lookat.lookatPos = headTransform.position + Vector3d(eyeRay.dir.x,eyeRay.dir.y,eyeRay.dir.z*0.1f)*13;
				//ai_lookat.lookatPos.z = (ai_lookat.lookatPos.z + owner->transform.position.z)*0.5f;
				//ai_lookat.lookatPos.z = Math.lerp( 0.2f, ai_lookat.lookatPos.z, (ai_lookat.lookatPos.z + (owner->transform.position.z+hull.height*0.7f))*0.5f );
				//ai_lookat.lookatPos.z = (ai_lookat.lookatPos.z + (owner->transform.position.z+hull.height*0.7f))*0.5f;
				ai_lookat.lookatPos.z = (ai_lookat.lookatPos.z + (owner->transform.position.z+hull.height*0.7f))*0.5f;
				//ai_lookat.lookatPos = transform.position + Vector3d::forward * transform.rotation;
			}

			// Have our view wander
			ai_lookat.lookAwayWanderTimer -= Time::deltaTime * info_lookat.lookAwayRate;
			if ( ai_lookat.lookAwayWanderTimer <= 0 && info_lookat.lookAway ) {
				ai_lookat.lookAwayWanderTimer = random_range( 0.8f, 8.0f );
				if ( Random.Next()%4 == 0 ) {
					// Reset view (it happens)
					ai_lookat.lookAwayWanderOffset = Vector3d(0,0,0);
				}
				else {
					// Have the gaze wander some
					ai_lookat.lookAwayWanderOffset += Vector3d( random_range(-10,10),random_range(-10,10),random_range(-5,5) );
					// Limit the wander amount, though.
					ftype max_size = ai_lookat.lookatPos.magnitude() * 0.9f;
					if ( ai_lookat.lookAwayWanderOffset.magnitude() > max_size ) {
						ai_lookat.lookAwayWanderOffset = ai_lookat.lookAwayWanderOffset.normal() * max_size;
					}
				}
			}
			if ( ai_lookat.lookatOverride <= 0 ) {
				// Final lookat position is lookat plus the wander offset
				ai_lookat.lookatPos += ai_lookat.lookAwayWanderOffset;
			}
		}
		else {
			// If not looking away, we don't want to wander immediately
			ai_lookat.lookAwayWanderTimer = info_lookat.lookAwayStart;
		}

		// Look at character 
		if ( ai_lookat.targetInView )
		{
			//if ( !ai_lookat.isLookingAtCharacter )
			{ // Note this will always be true when alerted
				ai_lookat.glanceAwayTimer = info_lookat.glanceAwayStart;	// Don't glance away from character for a time
				ai_lookat.glanceAwayAngle = Vector3d::zero;
				ai_lookat.isLookingAtCharacter = true;
			}
			/*if ( ai_lookat.isTalkingAtCharacter ) // Is this true?
			{
				ai_lookat.facingPos = ai_lookat.lookatPos;
			}*/
		}

		// Face walking direction when moving, though switch looking between looking at people and move target
		if ( ai_move.state.isRunning || ai_move.state.isWalking ) {
			if ( (ai_lookat.walkingFacingPos-owner->transform.position).sqrMagnitude() > 9.0f ) {
				ai_lookat.walkingFace = true;
			}
			// Change what we're doing when time has passed
			ai_lookat.walkingLookTimer -= Time::deltaTime;
			if ( ai_lookat.walkingLookTimer <= 0 ) {
				// Toggle lookat state
				ai_lookat.walkingLook = !ai_lookat.walkingLook;
				ai_lookat.walkingFace = !ai_lookat.walkingFace;
				// But bias it towards not looking where walking
				if ( Random.Next()%3 == 0 ) {
					ai_lookat.walkingLook = false;
				}
				// Set timer, again biased to not looking where walking
				if ( ai_lookat.walkingLook ) {
					ai_lookat.walkingLookTimer = random_range(1.0f,4.0f);
				}
				else {
					ai_lookat.walkingLookTimer = random_range(3.0f,9.0f);
				}
			}
			// Apply facing direction
			if ( ai_lookat.walkingLook ) {
				ai_lookat.lookatPos = ai_lookat.walkingLookatPos;
			}
			if ( ai_lookat.walkingFace ) {
				ai_lookat.facingPos = ai_lookat.walkingFacingPos;
			}
		}
		
	}
	
	// Do the glance effect
	ai_lookat.glanceAwayTimer -= Time::deltaTime * info_lookat.glanceAwayRate;
	if ( ai_lookat.glanceAwayTimer < 0 && info_lookat.glanceAway ) {
		ai_lookat.glanceAwayAngle += Vector3d( 0,random_range(-20.0f,20.0f),random_range(-20.0f,20.0f) );
		if ( ai_lookat.glanceAwayAngle.magnitude() > 36 ) {
			ai_lookat.glanceAwayAngle = ai_lookat.glanceAwayAngle.normal() * 36;
		}
		if ( ai_lookat.glanceAwayAngle.z < -10 ) {
			ai_lookat.glanceAwayAngle.z = -10;
		}
		if ( ai_lookat.glanceAwayAngle.z > 20 ) {
			ai_lookat.glanceAwayAngle.z = 20;
		}
		ai_lookat.glanceAwayTimer = random_range( 0.2f, 3.0f );
	}
	else {
		ai_lookat.glanceAwayAngle += Vector3d( 0,random_range(-4.0f,4.0f),random_range(-4.0f,4.0f) ) * Time::deltaTime;
	}
	ai_lookat.currentGlanceAwayAngle = ai_lookat.currentGlanceAwayAngle.lerp( ai_lookat.glanceAwayAngle, Time::TrainerFactor( 0.3f ) );


	// Check for invalid values
	if ( !VALID_FLOAT(ai_lookat.lookatPos.x) || !VALID_FLOAT(ai_lookat.lookatPos.y) || !VALID_FLOAT(ai_lookat.lookatPos.z) )
	{
		ai_lookat.lookatPos = Vector3d(0,0,0);
	}
}

