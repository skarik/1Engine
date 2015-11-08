
// Includes
#include "after/entities/character/CAfterPlayer.h"
#include "after/entities/CCharacterModel.h"
#include "core-ext/input/CInputControl.h"
#include "after/interfaces/CCharacterMotion.h"

// Needed misc includes
#include "core/time/time.h"

#include "after/physics/water/Water.h"
#include "Math.h"
#include "core/math/random/Random.h"
#include "after/terrain/Zones.h"

#include "renderer/debug/CDebugDrawer.h"

#include "engine/physics/motion/CRigidbody.h"

// Define
/*#ifndef mvtQuickReturn
#define mvtQuickReturn(a) (updateMoveType( (stateFunc_t)&CAfterPlayer::a ))
#endif*/

// Using
using namespace NPC;

// ========================
//  MCC Movetype Constants
// ========================
/*void CAfterPlayer::InitMovetypeConstants ( void )
{
	// Falling tracker
	vFallingVelocity = Vector3d();

	// Unobstruction
	bMoveUnobstructed = true;

	// Autojump
	bOverrideAutojump = false;
}*/
/*void CAfterPlayer::SetMovetypeConstants ( void )
{
	// Parkour info
	fWallSlideTime = 2.0f;
	fWallClimbStepTime = 0.3f;

	// Slidedown speed
	fSlideSpeedConstant = 8.0f;

	// Ledge grabbing
	fLedgeGrabMaxHeight = 1.6f;
	fLedgeGrabMinHeight = -2.3f;
	fLedgeGrabMaxDistance = 3.0f;
	fLedgeTargetHangHeight = -0.5f;

	// Friction and bounce properties
	fRigidbodyNormalFriction = 0.01f;
	fRigidbodySlideFriction = 0.01f;

	fRigidbodyNormalRestitution = 0.01f;
	fRigidbodyMovementRestitution = 0.00f;

	// Movement stats
	// == Player Hull Sizes ==
	fPlayerRadius		= pl_race_stats->fPlayerRadius;
	fNormalPlayerHeight = pl_race_stats->fStandingHeight;
	fCrouchPlayerHeight	= pl_race_stats->fCrouchingHeight;
	fPronePlayerHeight	= pl_race_stats->fProneHeight;

	// == Movement ==
	// Speed and acceleration
	fRunSpeed		= pl_race_stats->fRunSpeed;
	fSprintSpeed	= pl_race_stats->fSprintSpeed;
	fCrouchSpeed	= pl_race_stats->fCrouchSpeed;
	fProneSpeed		= pl_race_stats->fProneSpeed;
	fSwimSpeed		= pl_race_stats->fSwimSpeed;
	fGroundAccelBase= pl_race_stats->fGroundAccelBase;
	fAirAccelBase	= pl_race_stats->fAirAccelBase;
	fWaterAccelBase	= pl_race_stats->fWaterAccelBase;
	// Special moves
	bAutojumpEnabled= pl_race_stats->bCanAutojump;
	bCanSprint		= pl_race_stats->bCanSprint;
	bCanCrouchSprint= pl_race_stats->bCanCrouchSprint;

	fSlideTime		= pl_race_stats->fSlideTime;
}*/

// ======================
//  MCC Movetype Queuing
// ======================
/*void CAfterPlayer::SetMovementMode ( const NPC::eMovementEnumeration mode )
{
	if ( m_motion ) {
		m_motion->SetMovementModeQueued( mode );
	}
}
// Getter
NPC::eMovementEnumeration CAfterPlayer::GetMovementMode ( void )
{
	if ( m_motion ) {
		return m_motion->GetMovementMode();
	}
	else {
		return NPC::MOVEMENT_UNKNOWN;
	}
}*/

// Return current motion of the player rigidbody
Vector3d CAfterPlayer::GetMotionVelocity ( void )
{
	if ( m_motion && m_motion->m_rigidbody ) {
		return m_motion->m_rigidbody->GetVelocity();
	}
	return Vector3d::zero;
}
/*
// Get the current rigidbody
CRigidBody*	CAfterPlayer::GetRigidbody ( void )
{
	if ( m_motion ) {
		return m_motion->m_rigidbody;
	}
	return NULL;
}*/