

#include "CCharacterMotion.h"
// Character and stats handling
#include "after/entities/character/CCharacter.h"
#include "after/entities/character/CAfterPlayer.h"
#include "after/states/CRacialStats.h"
#include "after/entities/CCharacterModel.h"
#include "CAfterCharacterAnimator.h"
#include "after/types/character/Dialogue.h"
// Physics
#include "engine/physics/collider/types/CCapsuleCollider.h"
#include "engine/physics/motion/CRigidBodyCharacter.h"
// Collision detection
#include "engine/physics/raycast/Raycaster.h"
#include "after/terrain/Zones.h"
#include "after/physics/water/Water.h"
// Input
#include "core-ext/input/CInputControl.h"
// Math Utilities
#include "core/math/Math.h"

#include "after/types/terrain/BlockType.h"

// =======================
// Camera sets
// =======================
#include "engine-common/entities/CPlayer.h"
FORCE_INLINE void CCharacterMotion::CamSetFOVTarget ( const float nOffsetTarget )
{
	if ( m_charIsPlayer ) {
		((CAfterPlayer*)(m_character))->camViewAngleSetPOffset( nOffsetTarget );
	}
}
FORCE_INLINE void CCharacterMotion::CamSetRollTarget ( const float nOffsetTarget )
{
	if ( m_charIsPlayer ) {
		((CAfterPlayer*)(m_character))->camRollSetOffset( nOffsetTarget );
	}
}
FORCE_INLINE void CCharacterMotion::CamSetShakeTarget ( const float nSizeTarget )
{
	if ( m_charIsPlayer ) {
		((CAfterPlayer*)(m_character))->camViewBobSetTargetSize( nSizeTarget );
	}
}
FORCE_INLINE void CCharacterMotion::CamSetAnimationStick ( const bool nGlueCamera )
{
	if ( m_charIsPlayer ) {
		if ( nGlueCamera ) {
			((CAfterPlayer*)(m_character))->SetCameraMode( NPC::CAMERA_SETTING_ANIMLOCK_ON );
		}
		else {
			((CAfterPlayer*)(m_character))->SetCameraMode( NPC::CAMERA_SETTING_ANIMLOCK_OFF );
		}
	}
}
FORCE_INLINE void CCharacterMotion::CamPunchView ( const Vector3d& nPunchAmount )
{
	if ( m_charIsPlayer ) {
		((CAfterPlayer*)(m_character))->PunchView( nPunchAmount );
	}
}

// =======================

// Define syntax shortcut
#define quickReturn(a) (updateMoveType( (motion_t)&CCharacterMotion::a ))
#define func(a) ((motion_t)&CCharacterMotion::a )

// Change the move state
FORCE_INLINE void* CCharacterMotion::updateMoveType ( motion_t n_newMoveType )
{
	m_moveTypePrevious = m_moveType;
	m_moveType = n_newMoveType;
	return NULL;
}

// Query move state
FORCE_INLINE CCharacterMotion::motion_t CCharacterMotion::getMotionState ( const NPC::eMovementEnumeration mode )
{
	switch ( mode )
	{
	case NPC::MOVEMENT_DEFAULT:
		return func(mvt_OnGround);
	case NPC::MOVEMENT_FALLING:
		return func(mvt_Falling);
	case NPC::MOVEMENT_WATER:
		return func(mvt_Swimming);
	case NPC::MOVEMENT_MINIDASH:
		return func(mvt_Minidash);
	case NPC::MOVEMENT_PARKOUR_SLIDE:
		return func(mvt_CombatSlide);
	case NPC::MOVEMENT_SAVIORIZE:
		return func(mvt_Saviorize);
	case NPC::MOVEMENT_DEAD:
		return func(mvt_Dead);
	case NPC::MOVEMENT_DEAD_RECOVER:
		return func(mvt_DeadRecover);
	case NPC::MOVEMENT_STUN_GENERAL:
		return func(mvt_Stunned);
	case NPC::MOVEMENT_STUN_FALL_BACKWARDS:
		return func(mvt_FellOnBackStart);
	case NPC::MOVEMENT_ATK_GROUND:
		return func(mvt_AtkGround);
	case NPC::MOVEMENT_ATK_AIR:
		return func(mvt_AtkAir);
	case NPC::MOVEMENT_ATK_SLIDE:
		return func(mvt_AtkSlide);
	default:
		throw Core::InvalidArgumentException();
	}
	return NULL;
}

void CCharacterMotion::SetMovementModeQueued ( const NPC::eMovementEnumeration mode )
{
	if ( mode == NPC::MOVEMENT_MINIDASH ) {
		fSlideCounter = 0;
	}
	motion_t nextState = getMotionState( mode );
	if ( nextState ) {
		m_moveTypeQueued = nextState;
	}
}
void CCharacterMotion::SetMovementModeImmediate ( const NPC::eMovementEnumeration mode )
{
	if ( mode == NPC::MOVEMENT_MINIDASH ) {
		fSlideCounter = 0;
	}
	motion_t nextState = getMotionState( mode );
	if ( nextState ) {
		m_moveType = nextState;
	}
}
void CCharacterMotion::SetMovementModeNext ( const NPC::eMovementEnumeration mode )
{
	if ( mode == NPC::MOVEMENT_MINIDASH ) {
		fSlideCounter = 0;
	}
	motion_t nextState = getMotionState( mode );
	if ( nextState ) {
		m_moveTypeNext = nextState;
	}
}

NPC::eMovementEnumeration CCharacterMotion::GetMovementMode ( void ) const
{
	if ( m_moveType == func(mvt_OnGround) ) {
		return NPC::MOVEMENT_DEFAULT;
	}
	else if ( m_moveType == func(mvt_Falling) ) {
		return NPC::MOVEMENT_FALLING;
	}
	else if ( m_moveType == func(mvt_Swimming) ) {
		return NPC::MOVEMENT_WATER;
	}
	else if ( m_moveType == func(mvt_Minidash) ) {
		return NPC::MOVEMENT_MINIDASH;
	}
	else if ( m_moveType == func(mvt_TerraSlide) ) {
		return NPC::MOVEMENT_SLIDING;
	}
	else if ( m_moveType == func(mvt_CombatSlide) ) {
		return NPC::MOVEMENT_PARKOUR_SLIDE;
	}
	else if ( m_moveType == func(mvt_Stunned) ) {
		return NPC::MOVEMENT_STUN_HEAVY;
	}
	else if ( m_moveType == func(mvt_Saviorize) ) {
		return NPC::MOVEMENT_SAVIORIZE;
	}
	else if ( m_moveType == func(mvt_Dead) ) {
		return NPC::MOVEMENT_DEAD;
	}
	else if ( m_moveType == func(mvt_DeadRecover) ) {
		return NPC::MOVEMENT_DEAD_RECOVER;
	}
	else if ( m_moveType == func(mvt_AtkGround) ) {
		return NPC::MOVEMENT_ATK_GROUND;
	}
	else if ( m_moveType == func(mvt_AtkAir) ) {
		return NPC::MOVEMENT_ATK_AIR;
	}
	else if ( m_moveType == func(mvt_AtkSlide) ) {
		return NPC::MOVEMENT_ATK_SLIDE;
	}
	return NPC::MOVEMENT_UNKNOWN;
}


// ======================
//  MCC Player Movetypes
// ======================

// ------
// Ground movement (includes sprinting)
// ------
void*	CCharacterMotion::mvt_OnGround ( void )
{
	MvtCommonCrouching();
	CamSetFOVTarget( 0 );
	CamSetRollTarget( 0 );

	const uint32_t hitFilter = Physics::GetCollisionFilter(Layers::PHYS_SWEPTCOLLISION,0,31);

	float groundDistance = 0.0f;
	bool onGround = OnGround( groundDistance );
	if ( !onGround )
	{
		return quickReturn( mvt_Falling );
	}
	else
	{
		m_rigidbody->SetRestitution( fRigidbodyNormalRestitution );
		m_rigidbody->SetFriction( fRigidbodyNormalFriction );

		// Turn first
		ftype turnAmount;
		if ( !bIsProne ) {
			// Horizontal turn
			turnAmount = std::min<ftype>( fabs(vTurnInput.x), 720.0f * Time::deltaTime ) * Math.sgn<ftype>(vTurnInput.x);
			vCharRotation.z += turnAmount;
			vTurnInput.x -= turnAmount;
			// Vertical turn
			turnAmount = std::min<ftype>( fabs(vTurnInput.y), 720.0f * Time::deltaTime ) * Math.sgn<ftype>(vTurnInput.y);
			vHeadRotation.y -= turnAmount;
			vTurnInput.y -= turnAmount;
		}
		else { // turn slower if prone
			// Horizontal turn
			turnAmount = std::min<ftype>( fabs(vTurnInput.x), 360.0f * Time::deltaTime ) * Math.sgn<ftype>(vTurnInput.x);
			vCharRotation.z += turnAmount * 0.1f;
			vTurnInput.x -= turnAmount;
			// Vertical turn
			turnAmount = std::min<ftype>( fabs(vTurnInput.y), 360.0f * Time::deltaTime ) * Math.sgn<ftype>(vTurnInput.y);
			vHeadRotation.y -= turnAmount * 0.5f;
			vTurnInput.y -= turnAmount;
		}
		// Remove head rotation
		{
			turnAmount = 90.0f * Time::deltaTime;
			vHeadRotation.z = std::max<ftype>( 0, fabs( vHeadRotation.z ) - turnAmount ) * Math.sgn<ftype>( vHeadRotation.z );
		}

		// In Water
		if ( WaterTester::Get()->PositionInside( m_character->transform.position+Vector3d( 0,0,2.5f ) ) )
		{
			return quickReturn( mvt_Swimming );
		}

		//Vector3d moveVector(0,0,0);
		Vector3d inputMoveVector(0,0,0);
		inputMoveVector = m_input->vDirInput;
		inputMoveVector.z = 0;

		// Rotate the move vector to match the camera
		Matrix4x4 rotMatrix;
		rotMatrix.setRotation( vCharRotation );
		inputMoveVector = rotMatrix*inputMoveVector;

		// Get rigidbody's velocity
		Vector3d vRigidbodySourceVelocity = m_rigidbody->GetVelocity();
		//vMoveVelocity = vRigidbodySourceVelocity;
		// Unrotate the rigidbody's velocity.
		{
			//Debug::Drawer->DrawLine( transform.position + Vector3d(0,0,1), transform.position + vMoveVelocity + Vector3d(0,0,1), Color(0.6,0.3,0.3,1) );
			/*Real speed = vMoveVelocity.magnitude();
			vMoveVelocity.z = 0;
			Real postspeed = vMoveVelocity.magnitude();
			if ( speed > FTYPE_PRECISION && postspeed > FTYPE_PRECISION ) {
				vMoveVelocity.normalize();
				vMoveVelocity *= speed;
			}*/
			//Debug::Drawer->DrawLine( transform.position + Vector3d(0,0,1), transform.position + vMoveVelocity + Vector3d(0,0,1), Color(1,0,0,1) );
		}

		// Change friction based on block
		ftype slowdownMultiplier = 1.0f;
		switch ( iTerrainCurrentBlock )
		{
			case Terrain::EB_SAND:
			case Terrain::EB_MUD:	slowdownMultiplier = 0.4f; break;
			case Terrain::EB_ICE:	slowdownMultiplier = 0.08f; break;
		}
		// Slow down movement
		if ( (inputMoveVector.sqrMagnitude() < 0.1f) )
		{
			Real deltaSpeed = Time::deltaTime * m_stats->fGroundAccelBase * std::min<ftype>(slowdownMultiplier+0.1f,1.0f);
			vMoveVelocity += (-vMoveVelocity.normal()) * deltaSpeed;
			if ( vMoveVelocity.magnitude() < deltaSpeed*1.2f ) {
				vMoveVelocity = Vector3d( 0,0,0 );
			}
		}
		// Add to move velocity
		else {
			if ( vMoveVelocity.z <= 0.0f ) {
				vMoveVelocity += inputMoveVector * Time::deltaTime * m_stats->fGroundAccelBase * slowdownMultiplier;
			}
		}

		// Get the move speed limit
		ftype limit;
		if ( bIsProne )
			limit = m_stats->fProneSpeed;
		else if ( bIsCrouching )
			limit = m_stats->fCrouchSpeed;
		else
			limit = m_stats->fRunSpeed;
		if ( bIsSprinting )
			limit *= m_stats->fSprintSpeed/m_stats->fRunSpeed;
		
		// Limit the xy movement while moving
		Vector3d testCase = Vector3d( vMoveVelocity.x, vMoveVelocity.y, 0 );
		{
			Real currentSpeed = testCase.magnitude();
			if ( currentSpeed > m_stats->fSprintSpeed )
			{
				testCase = testCase.normal() * m_stats->fSprintSpeed;
				vMoveVelocity.x = testCase.x;
				vMoveVelocity.y = testCase.y;
			}
			if ( currentSpeed > limit )
			{
				currentSpeed = std::max( limit, currentSpeed - Time::deltaTime*m_stats->fGroundAccelBase*2.0f );
				testCase = testCase.normal() * currentSpeed;
				vMoveVelocity.x = testCase.x;
				vMoveVelocity.y = testCase.y;
			}
		}

		// Rotate the XY component of the move velocity (to move up slopes)
		Vector3d targetVelocity ( 0,0,vMoveVelocity.z );
		{
			Vector3d rawXY ( -vMoveVelocity.y, vMoveVelocity.x, 0 );
			Vector3d nextXY = rawXY.cross(vMoveLastGroundNormal).normal();
			Real speed = Vector3d( vMoveVelocity.x, vMoveVelocity.y, 0 ).magnitude();
			targetVelocity += nextXY * speed;
		}

		// Slope Sliding
		switch ( iTerrainCurrentBlock )
		{
		case Terrain::EB_SAND:
		case Terrain::EB_ICE:
		case Terrain::EB_MUD:
			if ( vMoveVelocity.sqrMagnitude() > sqr(fSlideSpeedConstant) )
			{
				if ((( vTerrainTangent.z < -0.4f )&&( vTerrainTangent.dot( vMoveVelocity ) > 0.3f ))
					||(( vMoveLastGroundNormal.z < -0.4f )&&( vMoveLastGroundNormal.dot( vMoveVelocity ) > 0.3f ))
					)
				{
					return quickReturn( mvt_TerraSlide );
				}
			}
			break;
		default:
			if ( vMoveVelocity.sqrMagnitude() > sqr(fSlideSpeedConstant) )
			{
				if (( random_range( 0.0f, 1.0f ) < 0.01f )&&
					((( vTerrainTangent.z < -0.5f )&&( vTerrainTangent.dot( vMoveVelocity ) > 0.5f ))
					||(( vMoveLastGroundNormal.z < -0.5f )&&( vMoveLastGroundNormal.dot( vMoveVelocity ) > 0.5f )))
					)
				{
					return quickReturn( mvt_TerraSlide );
				}
			}
			break;
		};

		// Raytrace for low wall and attempt to reduce horizontal motion
		/*if ( true )
		{
			// Raytrace forward
			RaycastHit hitResult;
			Ray ray;
			ray.pos = m_character->transform.position + Vector3d( 0,0,0.08f );
			ray.dir = vMoveVelocity.normal();

			if ( Raycaster.Raycast( ray, 3.0f, &hitResult, hitFilter ) && ( fabs( hitResult.hitNormal.z ) < 0.2f ) 
				&& ( hitResult.hitNormal.dot(ray.dir) < -0.5f ) ) // Also check the normal of this wall face
			{
				if ( hitResult.distance < m_stats->fPlayerRadius * 1.1f )
				{
					// Perform the slide
					vMoveVelocity += hitResult.hitNormal * hitResult.hitNormal.dot(vMoveVelocity) * 0.95f;
				}
			}
		}*/

		// Autojump (blockhop)
		/*bOverrideAutojump = false;
		if (( m_stats->bCanAutojump )&&( !bIsCrouching )&&( !bIsProne )
			&&( Vector2d( m_input->vDirInput ).sqrMagnitude() > 0.8f ))
		{
			// Raytrace forward
			RaycastHit hitResult;
			Ray ray;
			ray.pos = m_character->transform.position + Vector3d( 0,0,0.08f );
			ray.dir = vMoveVelocity.normal();
			if ( Raycaster.Raycast( ray, 3.0f, &hitResult, hitFilter ) && ( fabs( hitResult.hitNormal.z ) < 0.2f ) 
				&& ( hitResult.hitNormal.dot(ray.dir) < -0.5f ) ) // Also check the normal of this wall face
			{
				ray.pos.z += 2; // Increase trace height for trace that looks for upper wall
				ftype distanceToStep = hitResult.distance;
				// Create upwards ray trace to look for open space
				Ray nextRay ( hitResult.hitPos-hitResult.hitNormal, Vector3d(0,0,1) );
				nextRay.pos.z += 2;
				// Check for first open block
				bool blockOpen = !Raycaster.Raycast( ray, hitResult.distance + 1.3f, &hitResult, hitFilter ); // Check horizontal
				if ( blockOpen && !Raycaster.Raycast( nextRay, std::max<ftype>( 0.5f, fPlayerHeight-1.9f ), &hitResult, hitFilter ) ) // Check upwards
				{
					// Jump if far enough
					if ( distanceToStep > m_stats->fPlayerRadius+0.4f )
					{
						vMoveVelocity.z = 9.4f + testCase.sqrMagnitude() * 0.012f;

						// Subtract stamina if sprinting
						if ( bIsSprinting )
						{
							m_character->stats.fStamina -= 2.0f;
						}
					}
					else // If too close, then just rise up
					{
						//vMoveVelocity.z = 0.0f;
						//m_rigidbody->SetPosition( m_rigidbody->GetPosition() + (Vector3d( -vMoveVelocity.x,-vMoveVelocity.y,8.0f ) * Time::deltaTime) );
						vMoveVelocity.x *= 0.6f;
						vMoveVelocity.y *= 0.6f;
						vMoveVelocity.z = 3.7f; // Manually set velocity
					}
				}
				// Set overriding autojump on
				bOverrideAutojump = true;
			}
		}*/

		// Object vaulting
		if ( (!bIsProne) && (!bIsCrouching) && m_input->vDirInput.x > 0.6f && m_input->axes.jump.pressed() && CheckVaultableObject() )
		{
			return quickReturn( mvt_VaultStart );
		}
		else if ( (!bIsProne) && (!bIsCrouching) && m_input->axes.jump.pressed() && CheckVaultableObject() )
		{
			bVaultObject = false;
			return quickReturn( mvt_VaultStart );
		}

		// Ledge grabbing
		if ( (!bIsProne) && (!bIsCrouching) && CheckLedgeGrabbing() )
		{
			if ( m_input->axes.jump ) //if ( fInput[iJump] )
				return quickReturn( mvt_ClimbStart );
		}

		// Remove stamina if sprinting
		if (( bIsSprinting )&&( vRigidbodySourceVelocity.sqrMagnitude() > (m_stats->fRunSpeed*0.9f) ))
		{
			m_character->stats.fStamina -= Time::deltaTime * 8.0f;
		}

		// Combat slide
		if ( fSlideCounter > 0.01f )
		{
			fSlideCounter -= Time::deltaTime;
		}
		else
		{
			fSlideCounter = 0.0f;
			if ( m_canCombatSlide && bIsSprinting && bStartedCrouching )
			{
				if ( vRigidbodySourceVelocity.magnitude() > m_stats->fSprintSpeed*0.63f ) {
					if (( m_input->vDirInput.x > 0.4f )&&( fabs(m_input->vDirInput.y) < 0.6f )) {
						return quickReturn( mvt_CombatSlide );
					}
				}
			}
		}

		// Perform stair stepup code
		{
			Vector2d flatMovespeed = Vector2d(vMoveVelocity);
			Real moveSpeed = flatMovespeed.magnitude();
			if ( moveSpeed > m_stats->fPlayerRadius*0.5f )
			{
				Ray c_trace;
				RaycastHit c_trace_result;

				// Trace downward at the edge of collision movement
				c_trace = Ray( m_character->transform.position + Vector3d( (flatMovespeed/moveSpeed)*m_stats->fPlayerRadius * 1.5f, m_stats->fPlayerRadius*2 ), Vector3d(0,0,-1) );
				// Raycast
				Raycaster.Raycast( c_trace, m_stats->fPlayerRadius*2, &c_trace_result, 0x00, m_character );
				// If it hits, do further checks for a step
				if ( c_trace_result.hit && c_trace_result.hitNormal.z > 0.766f ) // limit ~40 degrees (must be facing mostly upward)
				{
					Real stepHeight = m_stats->fPlayerRadius*2 - c_trace_result.distance;
					//if ( (stepHeight > m_stats->fPlayerRadius*0.707f) && (stepHeight < m_stats->fPlayerRadius*(0.707f+c_trace_result.hitNormal.z-0.45f)) ) 
					if ( stepHeight > m_stats->fPlayerRadius*(0.707f+(1-c_trace_result.hitNormal.z)) ) 
					{
						// Raise feet up
						/*Real currentFootheight = m_collider->GetFootOffset();
						if ( currentFootheight < stepHeight*1.4f ) {
							currentFootheight = stepHeight*1.4f;
							m_collider->SetFootOffset( currentFootheight );
						}*/
						m_rigidbody->AddToPosition( Vector3d( 0,0,stepHeight ) );
					}
				}
			}
		}

		// Do jumping+landing code
		vMoveVelocity.z = std::max( -1.0f, vMoveVelocity.z - m_stats->fJumpVelocity*4*Time::deltaTime );
		if ( m_moveTypePrevious != m_moveType ) { // When landing, can jump (almost) immediately and slow down movement
			vMoveVelocity.x *= 0.45f;
			vMoveVelocity.y *= 0.45f;
			vMoveVelocity.z = m_stats->fJumpVelocity * 0.1f;
		}
		// If get input to jump and can jump....
		if ( m_canJump && m_input->axes.jump.pressed() && vMoveVelocity.z <= 0.0f )
		{
			Real sourceZSpeed = std::max( vRigidbodySourceVelocity.z, targetVelocity.z );

			m_input->axes.jump.Skip(); // Skip next frame's jump

			if ( bIsProne )
			{
				bWantsProne = false;
			}
			else
			{
				Vector3d jumpVector = (vMoveLastGroundNormal+Vector3d(0,0,1)).normal() * m_stats->fJumpVelocity;
				//jumpVector.z = std::max<Real>( m_stats->fJumpVelocity - vMoveVelocity.z, jumpVector.z );
				jumpVector.z = std::min<Real>( m_stats->fJumpVelocity - sourceZSpeed, jumpVector.z );
				vMoveVelocity.z = jumpVector.z; // set movement velocity
				targetVelocity += jumpVector;

				// Subtract stamina if sprinting
				if ( bIsSprinting )
				{
					m_character->stats.fStamina -= 3.0f;
				}
				else
				{
					m_character->stats.fStamina -= 2.0f;
				}

				if ( !bIsSprinting ) {
					m_model->PlayAnimation( "jump" );
				}
				else {
					m_model->PlayAnimation( "jump_sprint" );
				}

				m_character->DoSpeech( NPC::SpeechType_Jump );

				// Reset slide timer
				fSlideCounter = -0.01f;
			}
		}

		// Set rigidbody's velocity
		m_rigidbody->SetVelocity( targetVelocity );

		// View bob!
		//camViewBobSetSpeed( 22.0f + testCase.magnitude()/m_stats->fSprintSpeed + ( bIsSprinting ? 6.0f : 0.0f )  );
		//camViewBobSetTargetSize( testCase.sqrMagnitude()*0.006f );
		CamSetShakeTarget( 0.0 );
		CamSetAnimationStick( false );

		// Animation set!
		ftype sqrSpeed = vRigidbodySourceVelocity.sqrMagnitude();
		if ( bIsProne )
		{
			if ( sqrSpeed > sqr(m_stats->fProneSpeed*0.8f) ) {
				m_animator->SetMoveAnimation( NPC::MoveAnimProneMove );
			}
			else {
				m_animator->SetMoveAnimation( NPC::MoveAnimProneIdle );
			}
		}
		else if ( bIsCrouching )
		{
			if ( sqrSpeed > sqr(m_stats->fCrouchSpeed*(m_stats->fSprintSpeed/m_stats->fRunSpeed)*0.8f) ) {
				m_animator->SetMoveAnimation( NPC::MoveAnimCrouchSprint );
			}
			else if ( sqrSpeed > sqr(m_stats->fCrouchSpeed*0.8f*0.6f) ) {
				m_animator->SetMoveAnimation( NPC::MoveAnimCrouchMove );
			}
			else {
				m_animator->SetMoveAnimation( NPC::MoveAnimCrouchIdle );
			}
		}
		else
		{
			if ( sqrSpeed > sqr(m_stats->fSprintSpeed*0.8f) ) {
				m_animator->SetMoveAnimation( NPC::MoveAnimWalkSprint );
			}
			else if ( sqrSpeed > sqr(m_stats->fRunSpeed*0.8f*0.8f) ) {
				m_animator->SetMoveAnimation( NPC::MoveAnimWalkMove );
			}
			else if ( sqrSpeed > sqr(m_stats->fRunSpeed*0.8f*0.4f) ) {
				m_animator->SetMoveAnimation( NPC::MoveAnimWalkWalk );
			}
			else {
				m_animator->SetMoveAnimation( NPC::MoveAnimWalkIdle );
			}
		}
	}

	// Set wallrun ready
	bWallRunReady = true;

	return quickReturn( mvt_OnGround );
}

// ------
// Falling
// ------
void*	CCharacterMotion::mvt_Falling ( void )
{
	bWantsProne = false;
	MvtCommonCrouching();
	CamSetRollTarget( 0 );

	// Reset offset
	m_model->SetVerticalOffset( 0 ); 

	// Turn first
	ftype turnAmount;
	{	// Horizontal turning
		turnAmount = std::min<ftype>( fabs(vTurnInput.x), 720.0f * Time::deltaTime ) * Math.sgn<ftype>(vTurnInput.x);
		vCharRotation.z += turnAmount;
		vTurnInput.x -= turnAmount;
		// Vertical turning
		turnAmount = std::min<ftype>( fabs(vTurnInput.y), 720.0f * Time::deltaTime ) * Math.sgn<ftype>(vTurnInput.y);
		vHeadRotation.y -= turnAmount;
		vTurnInput.y -= turnAmount;
	}
	// Remove head rotation
	{
		turnAmount = 90.0f * Time::deltaTime;
		vHeadRotation.z = std::max<ftype>( 0, fabs( vHeadRotation.z ) - turnAmount * Time::deltaTime ) * Math.sgn<ftype>( vHeadRotation.z );
	}

	m_rigidbody->SetRestitution( fRigidbodyNormalRestitution );

	// In Water
	if ( WaterTester::Get()->PositionInside( m_character->transform.position+Vector3d( 0,0,2.5f ) ) )
	{
		// Punch the camera based on the vertical speed
		float fallVar = EndFalling();
		float punchAmt = sqr( fallVar );
		CamPunchView( Vector3d( Random.Range(-0.0010f,0.0010f)*punchAmt,-punchAmt*0.0030f,Random.Range(-0.0015f,0.0015f)*punchAmt )*0.6f );

		// Generate fall damage and all that
		Damage hurt;
		hurt.type = DamageType::Fall|DamageType::Reflect|DamageType::Drown;
		hurt.amount = sqr(punchAmt*0.0037f)*Random.Range(0.1f,0.4f); // Decrease damage by random value

		// If the fall is a large enough fall
		if ( punchAmt*0.0060f > 3.4f ) {
			m_character->OnDamaged( hurt );
			// And stun if it's big enough!
			/*if ( hurt.amount > stats.fHealthMax*0.3f ) {
				return quickReturn( mvt_Stunned );
			}*/
		}

		m_rigidbody->SetRestitution( fRigidbodyMovementRestitution );
		return quickReturn( mvt_Swimming );
	}

	const uint32_t hitFilter = Physics::GetCollisionFilter(Layers::PHYS_SWEPTCOLLISION,0,31);

	float groundDistance = 0.0f;
	bool onGround = OnGround( groundDistance );
	if ( onGround )
	{
		m_rigidbody->SetRestitution( fRigidbodyMovementRestitution );

		// Set wallrun ready
		bWallRunReady = true;

		// Punch the camera based on the vertical speed
		float fallVar = EndFalling();
		//float punchAmt = sqr( vMoveVelocity.z );
		float punchAmt = sqr( fallVar );
		CamPunchView( Vector3d( Random.Range(-0.0010f,0.0010f)*punchAmt,-punchAmt*0.0030f,Random.Range(-0.0015f,0.0015f)*punchAmt )*0.6f );

		// Generate fall damage and all that
		Damage hurt;
		hurt.type |= DamageType::Fall;
		hurt.amount = sqr(punchAmt*0.0037f);

		// If the fall is a large enough fall
		if ( punchAmt*0.0060f > 3.4f )
		{
			Matrix4x4 rotMatrix;
			rotMatrix.setRotation( vCharRotation );
			Vector3d facingDirection = rotMatrix*Vector3d::forward;
			ftype forwardAmount = facingDirection.dot( m_rigidbody->GetVelocity().normal() );
			// If damage below 45% of max health and is crouching
			if (( hurt.amount < m_character->stats.fHealthMax * 0.45f )&&( bWantsCrouch ))
			{
				// Then roll if facing the proper direction
				if ( forwardAmount > 0.1f )
				{
					fSlideCounter = 0.0f;
					return quickReturn( mvt_CombatRoll );
				}
				else {
					// But do minor damage
					hurt.amount = std::max<ftype>( std::min<ftype>( hurt.amount*0.4f, m_character->stats.fHealthMax*0.27f ), m_character->stats.fHealthMax*0.15f );
					hurt.stagger_chance = 0;
					//fScreenBlurAmount += 0.4f;
					m_character->OnDamaged( hurt );
					return quickReturn( mvt_FellOnBackStart );
				}
			}
			else
			{
				// On all other accounts, do damage
				m_character->OnDamaged( hurt );
				/*if ( hurt.amount > stats.fHealthMax*0.3f ) {
					if ( forwardAmount > 0 ) {
						return quickReturn( mvt_Stunned );
					}
					else {
						fScreenBlurAmount += 0.4f;
						return quickReturn( mvt_FellOnBackStart );
					}
				}*/
			}
		}
		// Either start sliding or go to normal ground
		switch ( iTerrainCurrentBlock )
		{
		case Terrain::EB_SAND:
		case Terrain::EB_ICE:
		case Terrain::EB_MUD:
			if ( vMoveVelocity.sqrMagnitude() > sqr(fSlideSpeedConstant) )
			{
				if (( vTerrainTangent.z < -0.4f )&&( vTerrainTangent.dot( vMoveVelocity ) > 0.3f ))
				{
					return quickReturn( mvt_TerraSlide );
				}
			}
			break;
		default:
			if ( vMoveVelocity.sqrMagnitude() > sqr(fSlideSpeedConstant) )
			{
				if (( vTerrainTangent.z < -0.45f )&&( vTerrainTangent.dot( vMoveVelocity ) > 0.5f )&&( random_range( 0.0f, 1.0f ) < 0.2f ))
				{
					return quickReturn( mvt_TerraSlide );
				}
			}
			break;
		};
		// Play landing animations
		if ( hurt.amount > 0.2f ) {
			m_model->PlayAnimation( "land" );
		}
		// Play landing sounds
		if ( hurt.amount > 5.0f ) {
			m_character->DoSpeech( NPC::SpeechType_JumpLand );
			if ( Random.Range( 0,1 ) > 0.50f ) {
				m_character->DoSpeech( NPC::SpeechType_JumpLandHard );
			}
		}
		else if ( hurt.amount > 0.45f ) {
			m_character->DoSpeech( NPC::SpeechType_JumpLand );
		}
		else if ( hurt.amount > 0.15f ) {
			if ( Random.Range( 0,1 ) > 0.40f ) {
				m_character->DoSpeech( NPC::SpeechType_JumpLand );
			}
		}
		// Do the landing effect
		m_character->OnAnimationEvent( Animation::Event_Footstep_Left, Animation::EventTag_NoTag );
		m_character->OnAnimationEvent( Animation::Event_Footstep_Right, Animation::EventTag_NoTag );
		// Return ground speed
		return quickReturn( mvt_OnGround );
	}

	Vector3d moveVector(0,0,0);
	moveVector = m_input->vDirInput;
	moveVector.z = 0;

	// Rotate the move vector to match the camera
	Matrix4x4 rotMatrix;
	rotMatrix.setRotation( vCharRotation );
	moveVector = rotMatrix*moveVector;

	// Enable gravity
	m_rigidbody->SetGravity( true );
	vMoveVelocity = m_rigidbody->GetVelocity();
	UpdateFalling( vMoveVelocity );

	// If falling velocity is too great and is below the earth, or nothing collidable is in sight, then kill the player
	if ( (m_character->transform.position.z < -120.0f) && ((vFallingVelocity.z < -80) || (!Raycaster.Raycast( Ray(m_character->transform.position,Vector3d(0,0,-1)), 2000, NULL, 0x00 ))) ) {
		Damage hurt;
		hurt.type |= DamageType::Fall;
		hurt.amount = m_character->stats.fHealthMax * Time::deltaTime;
		m_character->OnDamaged( hurt );
	}

	// Limit the horizontal acceleration
	Vector3d testCase = Vector3d( vMoveVelocity.x, vMoveVelocity.y, 0 );
	Vector3d testCase2 = testCase + (Vector3d( moveVector.x, moveVector.y, 0 ) * Time::deltaTime * m_stats->fAirAccelBase);
	if (( testCase.magnitude() <= m_stats->fRunSpeed*1.1f )||( testCase2.magnitude() < testCase.magnitude() ))
	{
		vMoveVelocity += moveVector * Time::deltaTime * m_stats->fAirAccelBase;
	}

	// Raytrace for low wall and attempt to reduce horizontal motion
	if ( true )
	{
		// Raytrace forward
		RaycastHit hitResult;
		Ray ray;
		ray.pos = m_character->transform.position + Vector3d( 0,0,0.08f );
		ray.dir = testCase.normal();

		if ( Raycaster.Raycast( ray, 3.0f, &hitResult, hitFilter ) && ( fabs( hitResult.hitNormal.z ) < 0.2f ) 
			&& ( hitResult.hitNormal.dot(ray.dir) < -0.5f ) ) // Also check the normal of this wall face
		{
			if ( hitResult.distance < m_stats->fPlayerRadius * 1.1f ) {
				// Perform the slide
				vMoveVelocity += hitResult.hitNormal * hitResult.hitNormal.dot(vMoveVelocity) * 0.95f;
			}
		}
	}

	// Autojump (blockhop)
	if (( m_stats->bCanAutojump )&&( !bIsCrouching )&&( !bIsProne )&&( vMoveVelocity.z < 0 )
		&&( Vector3d( m_input->vDirInput.x,m_input->vDirInput.y,0 ).sqrMagnitude() > 0.8f )
		&&(( Vector3d( vMoveVelocity.x,vMoveVelocity.y,0 ).sqrMagnitude() > sqr(m_stats->fCrouchSpeed*0.8f) )|| bOverrideAutojump ))
	{
		// Raytrace forward
		RaycastHit hitResult;
		Ray ray;
		ray.pos = m_character->transform.position + Vector3d( 0,0,0.08f );
		ray.dir = testCase.normal();
		if ( Raycaster.Raycast( ray, 3.0f, &hitResult, hitFilter ) && ( fabs( hitResult.hitNormal.z ) < 0.2f )
			&& ( hitResult.hitNormal.dot(ray.dir) < -0.5f ) ) // Also check the normal of this wall face
		{
			ray.pos.z += 2; // Increase trace height for trace that looks for upper wall
			ftype distanceToStep = hitResult.distance;
			// Create upwards ray trace to look for open space
			Ray nextRay ( hitResult.hitPos-hitResult.hitNormal, Vector3d(0,0,1) );
			nextRay.pos.z += 2;
			// Check for first open block
			bool blockOpen = !Raycaster.Raycast( ray, hitResult.distance + 1.3f, &hitResult, hitFilter ); // Check horizontal
			if ( blockOpen && !Raycaster.Raycast( nextRay, std::max<ftype>( 0.5f, fPlayerHeight-1.9f ), &hitResult, hitFilter ) ) // Check upwards
			{
				// Jump if far enough
				if ( distanceToStep > m_stats->fPlayerRadius+0.4f )
				{
					vMoveVelocity.z = 8.4f;
				}
				else // If too close, then just rise up
				{
					//vMoveVelocity.z = 0.0f;
					m_rigidbody->SetPosition( m_rigidbody->GetPosition() + (Vector3d( -vMoveVelocity.x,-vMoveVelocity.y,8.0f ) * Time::deltaTime) );
					//m_rigidbody->SetPosition( m_rigidbody->GetPosition() + (Vector3d( 0,0,8.0f ) * Time::deltaTime) );
					//vMoveVelocity.z = 3.2f;
					vMoveVelocity.x *= Time::TrainerFactor( 1.0f );
					vMoveVelocity.y *= Time::TrainerFactor( 1.0f );
					vMoveVelocity.z = 3.2f;
				}
			}
		}
	}

	// Ledge grabbing
	if ( CheckLedgeGrabbing() )
	{
		if (( (vMoveVelocity.z > 0)&& m_input->axes.jump.pressed() ) || ( (vMoveVelocity.z < 0)&&(m_input->axes.jump) )) {
			m_input->axes.jump.Skip();

			// Punch the camera based on the vertical speed
			float fallVar = EndFalling();
			float punchAmt = sqr( fallVar );
			CamPunchView( Vector3d( random_range(-0.0015f,0.0015f)*punchAmt,-punchAmt*0.0030f,random_range(-0.0010f,0.0010f)*punchAmt ) );

			// Generate fall damage and all that
			Damage hurt;
			hurt.type |= DamageType::Fall;
			hurt.amount = sqr(punchAmt*0.0027f);

			// If damage is smaller than 35% of health
			if (( hurt.amount < m_character->stats.fHealthMax * 0.35f ))
			{
				// Just grab
				return quickReturn( mvt_ClimbStart );
			}
			else
			{
				m_character->OnDamaged( hurt ); // Do damage
				// Chance to miss the grab
				if ( rand() % 3 == 0 ) {
					return quickReturn( mvt_ClimbStart );
				}
				else {
					// Todo: Play fumble animation
				}
			}
		}
	}

	// Wall running
	fWallrunCooldown -= Time::deltaTime;
	if ( (!bIsProne) && (!bIsCrouching) && (m_character->stats.fStamina>=1.0f) && (bWallRunReady || fSlideCounter <= 0.01f || fWallrunCooldown <= 0.01f) && CheckWallRunLateral() && m_input->axes.jump && (m_input->vDirInput.x > 0.707f) )
	{
		// Reset counters
		fWallClimbCounter = 0;
		iWallRunStepCount = 0;
		fWallrunCooldown = 0.2f;
		// Start climbing up
		return quickReturn( mvt_WallRun );
	}
	// Wall climbing
	else if ( (!bIsProne) && (!bIsCrouching) && bWallRunReady && CheckWallRunClimbing() && (vMoveVelocity.z > m_stats->fJumpVelocity*0.6f) && m_input->axes.jump )
	{
		// Reset counters
		fWallClimbCounter = 0;
		iWallRunStepCount = 0;
		// Start climbing up
		return quickReturn( mvt_WallClimb );
	}
	// Wall sliding
	else if ( CheckWallSliding() )
	{
		if ( fSlideCounter > 0.01f ) {
			fSlideCounter -= Time::deltaTime;
		}
		else
		{
			fSlideCounter = 0.0f;
			if (( (vMoveVelocity.z > 0)&& m_input->axes.jump.pressed() ) || ( (vMoveVelocity.z < 0)&&((m_input->axes.jump)||(m_input->vDirInput.x > 0.5f)) ))
			{
				return quickReturn( mvt_WallSlide );
			}
		}
	}

	// Set new velocity
	m_rigidbody->SetVelocity( vMoveVelocity );

	// View shake!
	//camViewBobSetSpeed( vMoveVelocity.sqrMagnitude() * 0.05f * ( (vMoveVelocity.z<0) ? 2.0f : 1.0f )  );
	CamSetShakeTarget( vMoveVelocity.sqrMagnitude()*0.002f );
	// FOV widen at high speed!
	CamSetFOVTarget( vMoveVelocity.sqrMagnitude()*0.004f );
	CamSetAnimationStick( false );

	// Play falling animation
	//if (( groundDistance > 2.1f )||( vMoveVelocity.z > 4.0f ))
	if (( groundDistance > 1.1f )||( vMoveVelocity.z > 2.0f ))
	{
		m_animator->SetMoveAnimation( NPC::MoveAnimFall );
	}
	// Offset the player height
	//model->SetVerticalOffset( fPlayerHeight-fNormalPlayerHeight );

	return quickReturn( mvt_Falling );
}

void	CCharacterMotion::UpdateFalling ( const Vector3d& velocity )
{
	// Only save the largest amount of components
	if ( velocity.z < vFallingVelocity.z ) {
		vFallingVelocity.z = velocity.z;
	}
	if ( fabs( velocity.x ) > fabs( vFallingVelocity.x ) ) {
		vFallingVelocity.x = velocity.x;
	}
	if ( fabs( velocity.y ) > fabs( vFallingVelocity.y ) ) {
		vFallingVelocity.y = velocity.y;
	}
}
ftype	CCharacterMotion::EndFalling ( void )
{
	ftype fall_value = vFallingVelocity.z;
	vFallingVelocity = Vector3d::zero;
	return fall_value;
}	

// ------
// Swimming
// ------
void*	CCharacterMotion::mvt_Swimming ( void )
{
	CamSetFOVTarget( 0 );
	CamSetRollTarget( 0 );

	// Check if still in water
	if ( !WaterTester::Get()->PositionInside( m_character->transform.position+Vector3d( 0,0,2.5f ) ) )
	{
		// Set the gravity to true
		m_rigidbody->SetGravity( true );
		// Return no longer in water
		return quickReturn( mvt_Falling );
	}
	else
	{
		// Set the gravity to nil
		m_rigidbody->SetGravity( false );

		// Set player height to crouch height on default, and prone if ducking
		if ( m_input->axes.crouch ) {
			MvtSetPlayerHeightStick( m_stats->fProneHeight );
		}
		else {
			MvtSetPlayerHeightStick( m_stats->fCrouchingHeight );
		}

		// Turn first
		ftype turnAmount;
		{	// Horizontal turning
			turnAmount = std::min<ftype>( fabs(vTurnInput.x), 360.0f * Time::deltaTime ) * Math.sgn<ftype>(vTurnInput.x);
			vCharRotation.z += turnAmount;
			vTurnInput.x -= turnAmount;
			// Vertical turning
			turnAmount = std::min<ftype>( fabs(vTurnInput.y), 360.0f * Time::deltaTime ) * Math.sgn<ftype>(vTurnInput.y);
			vHeadRotation.y -= turnAmount;
			vTurnInput.y -= turnAmount;
		}
		// Remove head rotation
		{
			turnAmount = 90.0f * Time::deltaTime;
			vHeadRotation.z = std::max<ftype>( 0, fabs( vHeadRotation.z ) - turnAmount * Time::deltaTime ) * Math.sgn<ftype>( vHeadRotation.z );
		}

		Vector3d moveVector(0,0,0);
		moveVector = m_input->vDirInput;
		moveVector.z = 0;

		// Rotate the move vector to match the camera
		Matrix4x4 rotMatrix;
		//rotMatrix.setRotation( transform.rotation );
		//rotMatrix.setRotation( pCamera->transform.rotation );
		rotMatrix.setRotation( Vector3d( 0,vHeadRotation.y,vCharRotation.z ) );
		moveVector = rotMatrix*moveVector;

		// Tread water
		Vector3d vWaterGravity = Vector3d( 0,0,-1.0f );
		if ( bIsSprinting ) {
			vWaterGravity = Vector3d( 0,0,sin( Time::deltaTime*0.6f )*2.4f );
		}

		// Slow down movement
		if ( moveVector.sqrMagnitude() < 0.1f )
			vMoveVelocity += (vWaterGravity - vMoveVelocity) * Time::TrainerFactor( 0.2f );
		// Add to move velocity
		else
			vMoveVelocity += moveVector * Time::deltaTime * m_stats->fWaterAccelBase;

		// Vertical movement
		//vMoveVelocity.z += m_rigidbody->GetVelocity().z;
		if ( m_input->axes.jump ) //if ( fInput[iJump] )
			vMoveVelocity.z = 12.0f;

		// Get the move speed limit
		ftype limit;
		//if ( bIsCrouching )
			//limit = fRunSpeed * fCrouchSpeed/fRunSpeed;
		//else
			//limit = fRunSpeed;
		limit = m_stats->fSwimSpeed;
		if ( bIsSprinting )
			limit *= m_stats->fSprintSpeed/m_stats->fRunSpeed;
		
		//limit *= 0.7f;
		// Limit the xyz movement while moving
		Vector3d testCase = Vector3d( vMoveVelocity.x, vMoveVelocity.y, vMoveVelocity.z );
		{
			Real currentSpeed = testCase.magnitude();
			if ( currentSpeed > limit )
			{
				currentSpeed = std::max( limit, currentSpeed - Time::deltaTime*m_stats->fWaterAccelBase*2.0f );
				testCase = testCase.normal() * currentSpeed;
				vMoveVelocity.x = testCase.x;
				vMoveVelocity.y = testCase.y;
				vMoveVelocity.z = testCase.z;
			}
		}

		// Subtract stamina based on the movement
		if ( vMoveVelocity.sqrMagnitude() > 1 )
		{
			m_character->stats.fStamina -= vMoveVelocity.sqrMagnitude() * 0.02f * Time::deltaTime;
		}

		// Set rigidbody's velocity
		m_rigidbody->SetVelocity( vMoveVelocity );

		// View bob!
		//camViewBobSetSpeed( (22.0f + testCase.magnitude()/fSprintSpeed + ( bIsSprinting ? 6.0f : 0.0f ))*0.6f  );
		//camViewBobSetTargetSize( testCase.sqrMagnitude()*0.006f );
		CamSetShakeTarget( 0.0f );
		CamSetAnimationStick( false );

		// Play swimming animations
		if ( bIsSprinting ) {
			if ( vMoveVelocity.magnitude() < 2.0f ) {
				//model->SetMoveAnimation( "swim_tread" );
				m_animator->SetMoveAnimation( NPC::MoveAnimSwimTread );
			}
			else {
				//model->SetMoveAnimation( "swim" );
				m_animator->SetMoveAnimation( NPC::MoveAnimSwimMove );
			}	
		}
		else {
			if ( vMoveVelocity.magnitude() < 2.0f ) {
				//model->SetMoveAnimation( "swim" );
				m_animator->SetMoveAnimation( NPC::MoveAnimSwimIdle );
			}
			else {
				m_animator->SetMoveAnimation( NPC::MoveAnimSwimMove );
			}
		}

	}

	return quickReturn( mvt_Swimming );
}


// ------
// Combat Slide
// ------
void*	CCharacterMotion::mvt_CombatSlide ( void )
{
	// Set player height rather low
	MvtSetPlayerHeightStick( (m_stats->fProneHeight+m_stats->fCrouchingHeight)*0.5f ); 

	float groundDistance = 0.0f;
	bool onGround = OnGround( groundDistance );
	if ( !onGround )
	{
		m_model->StopMoveAnimation( "slide" );
		m_animator->SetMoveAnimation( NPC::MoveAnimFall );
		m_character->OnAnimationEvent( Animation::Event_SlideEnd, Animation::EventTag_NoTag );
		return quickReturn( mvt_Falling );
	}
	else
	{
		// Get the move vector from the current direction
		Vector3d moveVector(0,0,0);
		// Grab the current velocity
		vMoveVelocity = m_rigidbody->GetVelocity();

		// Unrotate the rigidbody's velocity, save into testcase
		Vector3d testCase = vMoveVelocity;
		{
			Real speed = testCase.magnitude();
			testCase.z = 0;
			Real postspeed = testCase.magnitude();
			if ( speed > FTYPE_PRECISION && postspeed > FTYPE_PRECISION ) {
				testCase.normalize();
				testCase *= speed;
			}
		}

		// Turn first
		ftype turnAmount;
		{	// Horizontal turning
			turnAmount = std::min<ftype>( fabs(vTurnInput.x), 360.0f * Time::deltaTime ) * Math.sgn<ftype>(vTurnInput.x);
			vCharRotation.z += turnAmount * 0.1f;
			vHeadRotation.z += turnAmount * 0.4f;
			vTurnInput.x -= turnAmount;
			// Vertical turning
			turnAmount = std::min<ftype>( fabs(vTurnInput.y), 360.0f * Time::deltaTime ) * Math.sgn<ftype>(vTurnInput.y);
			vHeadRotation.y -= turnAmount * 0.5f;
			vTurnInput.y -= turnAmount;
		}

		// Limit head rotation
		if ( fabs( vHeadRotation.z ) > 45.0f ) {
			vHeadRotation.z = 45.0f * Math.sgn<ftype>( vHeadRotation.z );
		}

		// Offset the turning to the current facing direction
		{
			moveVector = m_input->vDirInput;
			moveVector.z = 0;
			moveVector.x += 0.7f;
			moveVector.y *= 0.5f;
			//moveVector.z = 0;
			//moveVector = Vector3d( 1,0,0 );
			// Rotate the move vector to match the camera
			Matrix4x4 rotMatrix;
			rotMatrix.setRotation( vCharRotation );
			moveVector = rotMatrix*moveVector;
			// Add the move vector to the test case
			testCase.x += moveVector.x*1.0f;
			testCase.y += moveVector.y*1.0f;
		}
		// Set the movespeed to constant sprintspeed
		//testCase = testCase.normal() * fSprintSpeed * 1.2f;
		testCase = testCase.normal() * m_stats->fSprintSpeed * (1.3f - (fSlideCounter/m_stats->fSlideTime)*0.9f);

		// Set the move vector at a constant value
		moveVector = testCase;

		// Set the horizontal velocity
		vMoveVelocity = moveVector;

		// Rotate the XY component of the move velocity (to move up slopes)
		{
			Vector3d rawXY ( -vMoveVelocity.y, vMoveVelocity.x, 0 );
			Vector3d nextXY = rawXY.cross(vMoveLastGroundNormal).normal();
			Real speed = rawXY.magnitude();
			vMoveVelocity.x = 0;
			vMoveVelocity.y = 0;
			vMoveVelocity += nextXY * speed;
		}

		// Set rigidbody's velocity
		m_rigidbody->SetVelocity( vMoveVelocity );

		// Check for if we hit a wall in front of us
		// Todo: move to a separate function
		const uint32_t hitFilter = Physics::GetCollisionFilter(Layers::PHYS_SWEPTCOLLISION,0,31);
		bool hitWall = false;
		Ray ray;
		ray.dir = vMoveVelocity.normal();
		ray.pos = m_character->transform.position + Vector3d( 0,0,1 );
		RaycastHit result;
		if ( Raycaster.Raycast( ray, 4.0f, &result, hitFilter ) )
		{
			if ( result.distance < 2.0f )
			{
				hitWall = true;
			}
		}

		// Increment slide counter
		fSlideCounter += Time::deltaTime * (1.2f - (fSlideCounter/m_stats->fSlideTime)*0.7f) * 0.9f;
		// If we hit a wall or the end of the slide time
		if (( fSlideCounter >= m_stats->fSlideTime )||( hitWall )||( vMoveVelocity.sqrMagnitude() < 0.9f )) {
			fSlideCounter = 0.5f;
			m_model->StopMoveAnimation( "slide" );
			m_animator->SetMoveAnimation( NPC::MoveAnimCrouchIdle );
			m_character->OnAnimationEvent( Animation::Event_SlideEnd, Animation::EventTag_NoTag );
			return quickReturn( mvt_OnGround );
		}

		// Reduce view bob to mostly nil, but very fast
		//camViewBobSetSpeed( 34.0f + testCase.magnitude()/fSprintSpeed );
		//camViewBobSetSpeed( 76.0f + testCase.magnitude()/fSprintSpeed*10.0f );
		CamSetShakeTarget( testCase.sqrMagnitude()*0.002f );
		CamSetFOVTarget( 7 * (1-fSlideCounter/m_stats->fSlideTime) );
		CamSetRollTarget( -7 );
		CamSetAnimationStick( true );

		m_model->SetMoveAnimation( "slide" );
		m_animator->SetMoveAnimation( NPC::MoveAnim_USER );
		m_character->OnAnimationEvent( Animation::Event_Slide, Animation::EventTag_NoTag );

		return quickReturn( mvt_CombatSlide );
	}
}

// ------
// Combat Roll
// ------
void*	CCharacterMotion::mvt_CombatRoll ( void )
{
	// Get the move vector from the current direction
	Vector3d moveVector(0,0,0);
	moveVector = m_rigidbody->GetVelocity();

	Vector3d testCase = moveVector;
	testCase.z = 0;

	// Increment slide counter
	fSlideCounter += Time::deltaTime;
	// If we hit a wall or the end of the slide time
	ftype fRollTime = m_stats->fSlideTime*0.9f;
	if ( fSlideCounter >= fRollTime )
	{
		//fSlideCounter = 0.0f;
		//vCameraAnimOffsetRotation = Vector3d( 0,0,0 );
		m_model->StopMoveAnimation( "roll" );
		m_animator->SetMoveAnimation( NPC::MoveAnimCrouchIdle );
		return quickReturn( mvt_OnGround );
	}
	else
	{
		//vCameraAnimOffsetRotation.y = -Math.Smoothlerp(fSlideCounter/fRollTime) * 360.0f;
	}

	// Set player height rather low (kinda)
	MvtSetPlayerHeightStick( m_stats->fCrouchingHeight ); 

	// Turn first
	ftype turnAmount;
	{	// Horizontal turning
		turnAmount = std::min<ftype>( fabs(vTurnInput.x), 360.0f * Time::deltaTime ) * Math.sgn<ftype>(vTurnInput.x);
		vCharRotation.z += turnAmount * 0.1f;
		vTurnInput.x -= turnAmount;
		// Vertical turning
		turnAmount = std::min<ftype>( fabs(vTurnInput.y), 360.0f * Time::deltaTime ) * Math.sgn<ftype>(vTurnInput.y);
		vHeadRotation.y -= turnAmount * 0.1f;
		vTurnInput.y -= turnAmount;
	}

	// Offset the turning to the current facing direction
	{
		moveVector = m_input->vDirInput;
		moveVector.z = 0;
		// Rotate the move vector to match the camera
		Matrix4x4 rotMatrix;
		rotMatrix.setRotation( vCharRotation );
		moveVector = rotMatrix*moveVector;
		// Add the move vector to the test case
		testCase.x += moveVector.x;
		testCase.y += moveVector.y;
	}
	// Set the movespeed to constant movespeed
	testCase = testCase.normal() * m_stats->fRunSpeed;

	// Set the move vector at a constant value
	moveVector = testCase;

	// Set the horizontal velocity
	vMoveVelocity = moveVector;
	// Set vertical velocity
	vMoveVelocity.z = m_rigidbody->GetVelocity().z;

	// Set rigidbody's velocity
	m_rigidbody->SetVelocity( vMoveVelocity );
	// Set camera viewbob effects
	//camViewBobSetSpeed( 76.0f + testCase.magnitude()/fSprintSpeed*10.0f );
	CamSetShakeTarget( testCase.sqrMagnitude()*0.002f );
	CamSetFOVTarget( 0 );
	CamSetRollTarget( (0.5f - fabs( (fSlideCounter/fRollTime) - 0.5f )) * 23.0f );
	CamSetAnimationStick( true );

	m_model->SetMoveAnimation( "roll" );
	m_animator->SetMoveAnimation( NPC::MoveAnim_USER );

	return quickReturn( mvt_CombatRoll );
}

// ------
// Terrain sliding
// ------
void*	CCharacterMotion::mvt_TerraSlide( void )
{
	bWantsProne = false;
	MvtCommonCrouching();

	// Turn first
	ftype turnAmount = std::min<ftype>( fabs(vTurnInput.x), 360.0f * Time::deltaTime ) * Math.sgn<ftype>(vTurnInput.x);
	vCharRotation.z += turnAmount * 0.5f;
	vTurnInput.x -= turnAmount;

	// In Water
	if ( WaterTester::Get()->PositionInside( m_character->transform.position+Vector3d( 0,0,2.5f ) ) )
	{
		return quickReturn( mvt_Swimming );
	}

	// Turn to falling if no ground can be seen
	float groundDistance = 0.0f;
	bool onGround = OnGround( groundDistance );
	if (( !onGround )&&( groundDistance > 2.5f )) // work on this!
	{
		return quickReturn( mvt_Falling );
	}

	// Get the rigidbody's current velocity
	vMoveVelocity = m_rigidbody->GetVelocity();

	// Add the slope to the velocity
	vMoveVelocity += vTerrainTangent * -vTerrainTangent.z * 63.0f * 2.0f * Time::deltaTime;
	// If crouching, add reverse velocity
	if ( bIsCrouching )
		vMoveVelocity += vTerrainTangent * vTerrainTangent.z * 63.0f * Time::deltaTime;
	// And add some player control to this
	{
		Vector3d moveVector(0,0,0);
		moveVector = m_input->vDirInput;
		moveVector.z = 0;

		// Rotate the move vector to match the camera
		Matrix4x4 rotMatrix;
		rotMatrix.setRotation( vCharRotation );
		moveVector = rotMatrix*moveVector;

		// Add small amount of move vector
		vMoveVelocity += moveVector * Time::deltaTime * m_stats->fAirAccelBase;
	}

	// Limit horizontal movement speed
	{
		Vector3d limit = vTerrainTangent * vMoveVelocity.magnitude();
		if ( fabs(vMoveVelocity.x) > fabs(limit.x) )
			vMoveVelocity.x = limit.x;
		if ( fabs(vMoveVelocity.y) > fabs(limit.y) )
			vMoveVelocity.y = limit.y;
		vMoveVelocity.z *= 1.1f;
	}

	// Set new velocity
	m_rigidbody->SetVelocity( vMoveVelocity );

	// Move downwards a little bit more.
	m_rigidbody->AddToPosition( Vector3d( 0,0,vTerrainTangent.z * 8.0f ) * Time::deltaTime );

	// If slowing down enough, then go to normal movement
	switch ( iTerrainCurrentBlock )
	{
	case Terrain::EB_SAND:
	case Terrain::EB_ICE:
	case Terrain::EB_MUD:
		if ( vMoveVelocity.magnitude() < (fSlideSpeedConstant * 0.6f) )
		{
			return quickReturn( mvt_OnGround );
		}
		break;
	default:
		if ( vMoveVelocity.magnitude() < (fSlideSpeedConstant * 0.8f) )
		{
			return quickReturn( mvt_OnGround );
		}
		break;
	};

	// todo: fall damage when entering from falling
	m_animator->SetMoveAnimation( NPC::MoveAnimFallSlide );

	// View shake!
	//camViewBobSetSpeed( vMoveVelocity.sqrMagnitude() * 0.05f * ( (vMoveVelocity.z<0) ? 2.0f : 1.0f )  );
	CamSetShakeTarget( vMoveVelocity.sqrMagnitude()*0.002f );

	// FOV widen at high speed!
	CamSetFOVTarget( vMoveVelocity.sqrMagnitude()*0.004f );

	return quickReturn( mvt_TerraSlide );
}
// ------
// Terrain ledge grabbing
// ------
bool	CCharacterMotion::CheckLedgeGrabbing ( void )
{
	Matrix4x4 rotMatrix;
	rotMatrix.setRotation( vCharRotation );

	// todo: want to check the distance to the ground as well (from ledge to ground, as that may make it ungrabbable)

	ftype fActualLedgeGrabMaxRange = fLedgeGrabMinHeight - std::min<ftype>( fabs(vFallingVelocity.z) * 0.08f, 6.0f );
	Ray c_trace;
	RaycastHit c_trace_result;
	RaycastHit c_first_ledgetrace_result;
	for ( ftype h_distance = fLedgeGrabMaxDistance; h_distance > 1.6f; h_distance *= 0.67f )
	{
		// At the highest grab height, raytrace down
		c_trace.dir = Vector3d( 0,0,-1 );
		c_trace.pos = m_character->transform.position + rotMatrix*Vector3d( h_distance, 0, fPlayerHeight + fLedgeGrabMaxHeight );

		Raycaster.Raycast( c_trace, fLedgeGrabMaxHeight-fActualLedgeGrabMaxRange, &c_trace_result, 0x00, m_character );
		if ( c_trace_result.hit && c_trace_result.hitNormal.z > 0.8 )
		{
			// If we hit something, make sure the grab area is free
			// To do this, we start at the collision point's height, go a little bit higher, and raytrace into that
			c_trace.dir = rotMatrix * Vector3d( 1,0,0 );
			c_trace.pos = m_character->transform.position + Vector3d( 0, 0, fPlayerHeight + fLedgeGrabMaxHeight - c_trace_result.distance + 0.1f );

			// Save the first ledge trace
			c_first_ledgetrace_result = c_trace_result;

			Raycaster.Raycast( c_trace, h_distance * 2.0f, &c_trace_result, 0x00, m_character );
			if (( !c_trace_result.hit )||( c_trace_result.distance > h_distance ))
			{
				// At this point, we know the grabbing area is free.
				// However, we now need to check the distance from the wall.
				// To do this, we start at the first collision point's height, go a little bit lower, and raytrace into that.
				c_trace.dir = rotMatrix * Vector3d( 1,0,0 );
				c_trace.pos = m_character->transform.position + Vector3d( 0, 0, fPlayerHeight + fLedgeGrabMaxHeight - c_first_ledgetrace_result.distance - 0.1f );

				Raycaster.Raycast( c_trace, h_distance, &c_trace_result, 0x00, m_character );
				if ( c_trace_result.hit )
				{
					// If the cast hit, that means the ledge is grabbable.

					// Save the wall's normal
					vHangingWall = c_trace_result.hitNormal;
					// And save the ledge's position
					vHangingPosition = c_trace_result.hitPos + Vector3d( 0,0, 0.1f );

					// However, we also want to check the floor in front of the wall. There has to be some room to actually grab, you know.
					c_trace.dir = Vector3d( 0,0,-1 );
					c_trace.pos = vHangingPosition + vHangingWall + Vector3d( 0,0, 0.1f );

					Raycaster.Raycast( c_trace, 5.0f, &c_trace_result, 0x00, m_character );
					if ( ( !c_trace_result.hit ) )
					{
						// If there's no hit, then the forward grabbing area is free
						return true;
					}

					/*ftype wallDistance = c_trace_result.distance;

					c_trace.dir = rotMatrix * Vector3d( 1,0,0 );
					c_trace.pos = transform.position + Vector3d( 0,0, 0.5f );

					Raycaster.Raycast( c_trace, wallDistance, &c_trace_result, 0x00 );
					if (( !c_trace_result.hit )||( c_trace_result.distance > wallDistance-0.5f ))
					{
						// If there's no hit, or the hit distance is far enough, then we can grab
						return true;
					}*/
				}
			}
		}
	}
	
	return false;
}

void*	CCharacterMotion::mvt_ClimbStart ( void )
{
	// First calculate the grabbing position and normal

	// Wait, this should already be calculated!

	// Instead, stop the rigidbody gravity and movement
	m_rigidbody->SetGravity( false );
	bMoveUnobstructed = false;

	// Reset hanging timer
	fHangingTimer = 0.7f;

	// Reset sliding cooldown
	fSlideCounter = 0.0f;

	// Go to the hanging state
	return quickReturn( mvt_ClimbHang );
}
void*	CCharacterMotion::mvt_ClimbHang ( void )
{
	// Set player to full height
	MvtSetPlayerHeightStick( m_stats->fStandingHeight );

	// Rotate the view to match the wall
	{
		// Based on wallTravelDirection, get a target value for vCharRotation.z.
		Quaternion currentRot;
		currentRot.SetEulerAngles( Vector3d(0,0,Math.Wrap(-vCharRotation.z,-180,180)) );
		Vector3d targetFacing = Vector3d( -vHangingWall.x, -vHangingWall.y, 0 ).normal();
		Quaternion targetRot = Quaternion::CreateRotationTo( Vector3d(1,0,0), targetFacing );
		currentRot = currentRot.Slerp( targetRot, 0.1f ); // todo: change this lerp to time
		vCharRotation.z = currentRot.GetEulerAngles().z;
	}

	// Turn head
	ftype turnAmount;
	{	// Horizontal turning
		turnAmount = std::min<ftype>( fabs(vTurnInput.x), 720.0f * Time::deltaTime ) * Math.sgn<ftype>(vTurnInput.x);
		vHeadRotation.z += turnAmount;
		vTurnInput.x -= turnAmount;
		// Vertical turning
		turnAmount = std::min<ftype>( fabs(vTurnInput.y), 720.0f * Time::deltaTime ) * Math.sgn<ftype>(vTurnInput.y);
		vHeadRotation.y -= turnAmount;
		vTurnInput.y -= turnAmount;
	}

	// Limit head rotation
	if ( fabs( vHeadRotation.z ) > 45.0f ) {
		vHeadRotation.z = 45.0f * Math.sgn<ftype>( vHeadRotation.z );
	}

	// No movement
	// Rather, slowly reduce movement (and move head to target position)
	//Vector3d targetPosition = vHangingPosition + Vector3d( 0,0, fLedgeTargetHangHeight-fPlayerHeight ) + vHangingWall*m_stats->fPlayerRadius;
	Vector3d targetPosition = vHangingPosition + Vector3d( 0,0, fLedgeTargetHangHeight-fPlayerHeight ) + vHangingWall * (8.0f/12.0f);
	//m_rigidbody->SetVelocity( m_rigidbody->GetVelocity() * (0.9f-Time::deltaTime) );
	m_rigidbody->SetVelocity( m_rigidbody->GetVelocity() * Time::SqrtTrainerFactor(0.5f) );
	//m_rigidbody->SetPosition( m_rigidbody->GetPosition()*0.8f + targetPosition*0.2f );
	m_rigidbody->SetPosition( m_rigidbody->GetPosition().lerp(targetPosition,Time::SqrtTrainerFactor(0.2f)) );

	// Chill on this hanging area

	// Limit z rotation
	vTurnInput.x = 0.0f; // Get rid of any horizontal turning

	// Do certain actions
	if ( fHangingTimer > 0 )	// Let player climb up after a half second
	{
		fHangingTimer -= Time::smoothDeltaTime;
	}
	else
	{
		fHangingTimer = 0.0f;
	}
	if ( m_input->vDirInput.x < 0.5f )	// Let player climb up if they stop pressing forward
	{
		fHangingTimer = 0.0f;
	}

	//if (( vDirInput.x > 0.5f )||( fInput[iJump] && !fInputPrev[iJump] ))
	if (( m_input->vDirInput.x > 0.5f )||( m_input->axes.jump.pressed() ))
	{
		m_input->axes.jump.Skip();
		if ( fHangingTimer <= 0.0f ) {
			// Play animation
			m_model->PlayAnimation( "hang_climb" );
			// Immediately set next step options (prevent falling)
			m_rigidbody->SetGravity( false );
			m_rigidbody->SetVelocity( Vector3d() );
			m_rigidbody->SetPosition( targetPosition );
			m_character->transform.position = targetPosition;
			return quickReturn( mvt_ClimbUp );
		}
	}
	else if (( m_input->vDirInput.x < -0.5f )||( m_character->stats.fStamina <= 1.0f ))
	{
		m_rigidbody->SetGravity( true );
		bMoveUnobstructed = true;
		return quickReturn( mvt_Falling );
	}

	// Slowly remove stamina
	m_character->stats.fStamina -= Time::deltaTime * 4.0f;

	// Set view angle effects
	//camViewBobSetSpeed( 0.8f );
	//camViewBobSetTargetSize( 3.1f );
	CamSetShakeTarget( 0.0f );
	CamSetRollTarget( 0.0f );

	// Play hanging animation
	m_model->SetMoveAnimation( "hang_idle" );
	m_animator->SetMoveAnimation( NPC::MoveAnim_USER );

	return quickReturn( mvt_ClimbHang );
}
void*	CCharacterMotion::mvt_ClimbUp ( void )
{
	if ( m_rigidbody ) {
		m_rigidbody->RemoveReference();
		delete m_rigidbody;
		m_rigidbody = NULL;
	}

	// Limit z rotation
	//vTurnInput.x = 0.0f; // Get rid of any horizontal turning

	// Turn head
	ftype turnAmount;
	{	// Horizontal turning
		turnAmount = std::min<ftype>( fabs(vTurnInput.x), 720.0f * Time::deltaTime ) * Math.sgn<ftype>(vTurnInput.x);
		vHeadRotation.z += turnAmount;
		vTurnInput.x -= turnAmount;
		// Vertical turning
		turnAmount = std::min<ftype>( fabs(vTurnInput.y), 720.0f * Time::deltaTime ) * Math.sgn<ftype>(vTurnInput.y);
		vHeadRotation.y -= turnAmount;
		vTurnInput.y -= turnAmount;
	}
	// Limit head rotation
	if ( fabs( vHeadRotation.z ) > 45.0f ) {
		vHeadRotation.z = 45.0f * Math.sgn<ftype>( vHeadRotation.z );
	}


	// Over time, need to move feet to (grabbing_position-normal)
	// To do this, increment the hanging timer
	fHangingTimer += Time::deltaTime * 1.5f;

	// And use the hanging timer for the lerp factor
	Vector3d targetPosition1 = vHangingPosition + Vector3d( 0,0, fLedgeTargetHangHeight-fPlayerHeight ) + vHangingWall*m_stats->fPlayerRadius;
	Vector3d targetPosition2 = vHangingPosition - vHangingWall*m_stats->fPlayerRadius + Vector3d( 0,0,0.1f );

	// Set the target position using lerp
	/*Vector3d finalPosition;
	finalPosition.x = Math.Lerp( cub(fHangingTimer), targetPosition1.x, targetPosition2.x );
	finalPosition.y = Math.Lerp( cub(fHangingTimer), targetPosition1.y, targetPosition2.y );
	finalPosition.z = Math.Smoothlerp( fHangingTimer, targetPosition1.z, targetPosition2.z );

	// Set the rigidbody to the new position
	m_rigidbody->SetPosition( finalPosition );*/

	// When fHangingTimer is 1, then at the end of the climb
	//if ( fHangingTimer >= 1.0f )
	//if ( m_model->GetAnimationState()->
	if ( fHangingTimer >= 3.333f*1.5f*0.9f )
	{
		//m_model->GetAnimationState()->FindAction( "hang_climb" )->Stop();
		m_model->StopMoveAnimation( "hang_climb" );

		delete m_collider;
		InitMovement();
		// Reenable gravity when feet there
		m_rigidbody->SetGravity( true );
		bMoveUnobstructed = true;
		// Also move to the falling state when there
		return quickReturn( mvt_OnGround );
	}

	// Quickly remove stamina
	//m_character->stats.fStamina -= Time::deltaTime * 15.0f;

	// Roll camera while climbing
	/*CamSetRollTarget( Math.Smoothlerp( (0.5f - fabs( fHangingTimer-0.5f )), 0, 19.0f ) );
	CamSetAnimationStick( true );*/
	// Stick camera to the animation
	CamSetRollTarget( 0 );
	CamSetAnimationStick( true );

	// Play climb-up animation
	//model->SetMoveAnimation( "hang_ledge_climb" );
	//m_model->PlayScriptedAnimation( "hang_climb" );
	m_model->PlayAnimation( "hang_climb" );

	// Trace upwards at target point to get the target collision size
	{
		RaycastHit upHitResult;
		Ray ray;
		ray.pos = targetPosition2 + Vector3d( 0,0,0.9f );
		ray.dir = Vector3d(0,0,1);
		Raycaster.Raycast( ray, 6.0f, &upHitResult, Physics::GetCollisionFilter(Layers::PHYS_WORLD_TRACE) ); 
		if ( upHitResult.hit )
		{
			if ( upHitResult.distance < (m_stats->fCrouchingHeight-1.0f) ) {
				//MvtSetPlayerHeightStick( m_stats->fProneHeight );
				fPlayerHeight = m_stats->fProneHeight;
				m_animator->SetMoveAnimation( NPC::MoveAnimProneIdle );
			}
			else if ( upHitResult.distance < (m_stats->fStandingHeight-1.0f) ) {
				//MvtSetPlayerHeightStick( m_stats->fCrouchingHeight );
				fPlayerHeight = m_stats->fCrouchingHeight;
				m_animator->SetMoveAnimation( NPC::MoveAnimCrouchIdle );
			}
			else {
				//MvtSetPlayerHeightStick( m_stats->fStandingHeight );
				fPlayerHeight = m_stats->fStandingHeight;
				//m_model->PlayScriptedAnimation( "hang_ledge_climb",45.0f );
				m_animator->SetMoveAnimation( NPC::MoveAnimWalkIdle );
				//m_animator->SetMoveAnimation( NPC::MoveAnim_USER );
			}
		}
		else {
			//MvtSetPlayerHeightStick( m_stats->fStandingHeight );
			fPlayerHeight = m_stats->fStandingHeight;
			//m_model->PlayScriptedAnimation( "hang_ledge_climb",45.0f );
			m_animator->SetMoveAnimation( NPC::MoveAnimWalkIdle );
			//m_animator->SetMoveAnimation( NPC::MoveAnim_USER );
		}
	}

	return quickReturn( mvt_ClimbUp );
}
// ------
// Tree/ladder climbing
// ------
void*	CCharacterMotion::mvt_Climbing ( void )
{
	// Check for the climbing

	// Check for the wall
	// check shit


	return quickReturn( mvt_Climbing );
}

// ------
// Vaulting objects
// ------
//  CheckVaultableObject()
// Checks ahead for a vaultable object.
// If the object is small enough to be vaulted up/over, returns true.
bool	CCharacterMotion::CheckVaultableObject ( void )
{
	// Check forward for an obstacle. This 
	Matrix4x4 rotMatrix;
	rotMatrix.setRotation( vCharRotation );

	// First raycast multiple rays forward.
	const ftype vSpacing = 0.25f;
	const ftype minVaultHeight = 2.0f;
	Ray c_trace;
	RaycastHit c_trace_result;
	RaycastHit c_closest_trace_result;
	c_closest_trace_result.hit = false;
	for ( ftype v_distance = fVaultTargetMaxHeight; v_distance > minVaultHeight; v_distance -= vSpacing )
	{
		// Trace forwards to find the closest collision
		c_trace.dir = rotMatrix * Vector3d( 1,0,0 );
		c_trace.pos = m_character->transform.position + Vector3d( 0,0,v_distance );

		Raycaster.Raycast( c_trace, fVaultTargetMaxDistance + m_stats->fPlayerRadius*0.5f, &c_trace_result, 0x00, m_character );
		if ( c_trace_result.hit && c_trace_result.hitNormal.z < 0.64 ) // limit ~40 degrees
		{
			if ( !c_closest_trace_result.hit || c_trace_result.distance < c_closest_trace_result.distance )
			{
				c_closest_trace_result = c_trace_result;
			}
		}
	}
	
	// Check the object height that was hit
	bool vault = false;
	if ( c_closest_trace_result.hit )
	{
		// Trace downwards
		c_trace.dir = Vector3d( 0,0,-1 );
		c_trace.pos = m_character->transform.position + rotMatrix*Vector3d( c_closest_trace_result.distance+0.05f, 0, fPlayerHeight - 0.1f );

		Raycaster.Raycast( c_trace, fPlayerHeight, &c_trace_result, 0x00, m_character );
		if ( c_trace_result.hit && c_trace_result.hitNormal.z > 0.64 ) // limit ~50 degrees
		{
			// Check the distance doesn't vary too wildly
			ftype horz_hitHeight = c_closest_trace_result.hitPos.z - m_character->transform.position.z;
			ftype down_hitHeight = c_trace_result.hitPos.z - m_character->transform.position.z;

			// Strange! The horizontal hit was higher than the downward check. Something is fuuucked
			if ( horz_hitHeight-0.05f > down_hitHeight )
			{
				return false;
			}
			// The downward check is way too high. V
			if ( down_hitHeight > horz_hitHeight+ 420 *0.004167f*vSpacing )
			{
				return false;
			}

			// Fuck. We're still here? Fucking vault then
			vault = true;
		}
	}

	// Check forward downward now to get the downward distance
	if ( vault )
	{
		RaycastHit c_downward_trace_result;

		// Trace downwards beyond the max width of a vaultable object
		c_trace.dir = Vector3d( 0,0,-1 );
		c_trace.pos = m_character->transform.position + rotMatrix*Vector3d( c_closest_trace_result.distance+1.8f, 0, fPlayerHeight - 0.1f );

		Raycaster.Raycast( c_trace, fPlayerHeight, &c_downward_trace_result, 0x00, m_character );
		// If there's no ground or the ground is lower, do a vault instead
		if ( !c_downward_trace_result.hit || ( c_trace_result.hitPos.z-c_downward_trace_result.hitPos.z > minVaultHeight ) )
		{
			bVaultObject = true;
		}
		else
		{
			bVaultObject = false;
		}
		// Set the vault height
		vVaultHeight = c_trace_result.hitPos.z - m_character->transform.position.z;
		return true;
	}

	// todo: want to check the distance to the ground as well (from ledge to ground, as that may make it ungrabbable)
	/*ftype fActualLedgeGrabMaxRange = fLedgeGrabMinHeight - std::min<ftype>( fabs(vFallingVelocity.z) * 0.08f, 6.0f );
	Ray c_trace;
	RaycastHit c_trace_result;
	RaycastHit c_first_ledgetrace_result;
	for ( ftype h_distance = fLedgeGrabMaxDistance; h_distance > 1.6f; h_distance *= 0.67f )
	{
		// At the highest grab height, raytrace down
		c_trace.dir = Vector3d( 0,0,-1 );
		c_trace.pos = m_character->transform.position + rotMatrix*Vector3d( h_distance, 0, fPlayerHeight + fLedgeGrabMaxHeight );

		Raycaster.Raycast( c_trace, fLedgeGrabMaxHeight-fActualLedgeGrabMaxRange, &c_trace_result, 0x00, m_character );
		if ( c_trace_result.hit && c_trace_result.hitNormal.z > 0.8 )
		{
			// If we hit something, make sure the grab area is free
			// To do this, we start at the collision point's height, go a little bit higher, and raytrace into that
			c_trace.dir = rotMatrix * Vector3d( 1,0,0 );
			c_trace.pos = m_character->transform.position + Vector3d( 0, 0, fPlayerHeight + fLedgeGrabMaxHeight - c_trace_result.distance + 0.1f );

			// Save the first ledge trace
			c_first_ledgetrace_result = c_trace_result;

			Raycaster.Raycast( c_trace, h_distance * 2.0f, &c_trace_result, 0x00, m_character );
			if (( !c_trace_result.hit )||( c_trace_result.distance > h_distance ))
			{
				// At this point, we know the grabbing area is free.
				// However, we now need to check the distance from the wall.
				// To do this, we start at the first collision point's height, go a little bit lower, and raytrace into that.
				c_trace.dir = rotMatrix * Vector3d( 1,0,0 );
				c_trace.pos = m_character->transform.position + Vector3d( 0, 0, fPlayerHeight + fLedgeGrabMaxHeight - c_first_ledgetrace_result.distance - 0.1f );

				Raycaster.Raycast( c_trace, h_distance, &c_trace_result, 0x00, m_character );
				if ( c_trace_result.hit )
				{
					// If the cast hit, that means the ledge is grabbable.

					// Save the wall's normal
					vHangingWall = c_trace_result.hitNormal;
					// And save the ledge's position
					vHangingPosition = c_trace_result.hitPos + Vector3d( 0,0, 0.1f );

					// However, we also want to check the floor in front of the wall. There has to be some room to actually grab, you know.
					c_trace.dir = Vector3d( 0,0,-1 );
					c_trace.pos = vHangingPosition + vHangingWall + Vector3d( 0,0, 0.1f );

					Raycaster.Raycast( c_trace, 5.0f, &c_trace_result, 0x00, m_character );
					if ( ( !c_trace_result.hit ) )
					{
						// If there's no hit, then the forward grabbing area is free
						return true;
					}
				}
			}
		}
	}*/

	return false;
}

void*	CCharacterMotion::mvt_VaultStart ( void )
{
	// Turn off gravity, remove vertical motion
	m_rigidbody->SetGravity( false );

	Vector3d vRigidbodySourceVelocity = m_rigidbody->GetVelocity();
	vRigidbodySourceVelocity.x *= 0.5;
	vRigidbodySourceVelocity.y *= 0.5;
	vRigidbodySourceVelocity.z = 0;
	m_rigidbody->SetVelocity( vRigidbodySourceVelocity );

	// Set the player height immediately now
	Real tTargetHeight = m_stats->fStandingHeight - vVaultHeight;
	/*while ( fabs( fPlayerHeight - tTargetHeight ) > 0.1f ) {
		MvtSetPlayerHeight( tTargetHeight );
	}*/
	m_collider->SetFootOffset( vVaultHeight );
	m_collider->SetHeight( tTargetHeight );

	// Calculate movespeed percentage
	Real t_movespeedPercent = ( vMoveVelocity.magnitude() - m_stats->fRunSpeed*0.35f )/( m_stats->fSprintSpeed - m_stats->fRunSpeed*0.35f );
	t_movespeedPercent = Math.Clamp( t_movespeedPercent, 0.0f, 1.0f );

	// Result vault timer (1 seconds)
	Real t_animationSpeed = 70.0f + 15.0f * t_movespeedPercent;
	vVaultTime = 1.0f - (25.0f/t_animationSpeed) * t_movespeedPercent;

	if ( bVaultObject )
	{
		//m_model->PlayScriptedAnimation( "vault_4h",t_animationSpeed );
		//m_model->GetAnimationState()->FindAction("vault_4h")->frame = 25.0f * t_movespeedPercent;
		m_model->PlayAnimation( "vault_4h", 1.0f + 25.0f * t_movespeedPercent, t_animationSpeed );
		m_animator->SetMoveAnimation( NPC::MoveAnim_USER );
		// if ground ahead
		return quickReturn( mvt_VaultHop );
	}
	else
	{
		// if no ground ahead
		return quickReturn( mvt_VaultClimb );
	}
}
void*	CCharacterMotion::mvt_VaultHop ( void )
{
	Real tTargetHeight = m_stats->fStandingHeight - vVaultHeight;
	//MvtSetPlayerHeight( tTargetHeight );

	CamSetFOVTarget( 0 );
	//CamSetRollTarget( 7 );
	CamSetRollTarget( -2 );
	CamSetAnimationStick( true );

	{
		// Turn first
		ftype turnAmount;
		{	// Horizontal turning
			turnAmount = std::min<ftype>( fabs(vTurnInput.x), 360.0f * Time::deltaTime ) * Math.sgn<ftype>(vTurnInput.x);
			vCharRotation.z += turnAmount * 0.1f;
			vHeadRotation.z += turnAmount * 0.4f;
			vTurnInput.x -= turnAmount;
			// Vertical turning
			turnAmount = std::min<ftype>( fabs(vTurnInput.y), 360.0f * Time::deltaTime ) * Math.sgn<ftype>(vTurnInput.y);
			vHeadRotation.y -= turnAmount * 0.5f;
			vTurnInput.y -= turnAmount;
		}

		// Limit head rotation
		if ( fabs( vHeadRotation.z ) > 45.0f ) {
			vHeadRotation.z = 45.0f * Math.sgn<ftype>( vHeadRotation.z );
		}
	}

	// Decrement vaulting time
	vVaultTime -= Time::deltaTime;
	// Check the ground
	float groundDistance = 0.0f;
	bool onGround = OnGround( groundDistance ); 
	// If on ground, then hit the thing we're passing over (or going too long, then limit it)
	if ( onGround || groundDistance < 0.5f || vVaultTime < 0.0f ) {
		bVaultObject = false; // Leaving object faulting
	}

	// Rotate the move vector to match the camera
	Matrix4x4 rotMatrix;
	rotMatrix.setRotation( vCharRotation );
	Vector3d vForwardDirection = rotMatrix * Vector3d( 1,0,0 );

	// Move forward based on the current facing direction + sprinting speed
	/*Vector3d vRigidbodySourceVelocity = m_rigidbody->GetVelocity();
	vRigidbodySourceVelocity.z = 0.0f;//bVaultObject ? 7.0f : -6.0f;
	vRigidbodySourceVelocity += vForwardDirection*5;
	vRigidbodySourceVelocity = vRigidbodySourceVelocity.normal() * m_stats->fSprintSpeed;//(bVaultObject ? m_stats->fSprintSpeed : m_stats->fRunSpeed);
	m_rigidbody->SetVelocity( vRigidbodySourceVelocity );*/
	
	/*Vector3d vRigidbodySourceVelocity = m_rigidbody->GetVelocity();
	vRigidbodySourceVelocity.z = 0.0f;
	vRigidbodySourceVelocity = vRigidbodySourceVelocity.normal() * std::max( vRigidbodySourceVelocity.magnitude() - 50.0f * Time::deltaTime, 0.0f );
	m_rigidbody->SetVelocity( vRigidbodySourceVelocity );*/

	Vector3d vRigidbodySourceVelocity = m_rigidbody->GetVelocity();
	vRigidbodySourceVelocity.z = 0.0f;//bVaultObject ? 7.0f : -6.0f;
	vRigidbodySourceVelocity += vForwardDirection*5;
	//vRigidbodySourceVelocity = vRigidbodySourceVelocity.normal() * m_stats->fRunSpeed * 0.35f;//(bVaultObject ? m_stats->fSprintSpeed : m_stats->fRunSpeed);
	vRigidbodySourceVelocity = vRigidbodySourceVelocity.normal() * std::max<Real>( vMoveVelocity.magnitude() * 0.20f, m_stats->fRunSpeed * 0.10f );
	m_rigidbody->SetVelocity( vRigidbodySourceVelocity );

	// If vaulted over the object, expand the collision to the ground
	if ( (!bVaultObject)&&(!onGround || groundDistance > 0.5f ) ) {
		ftype footOffset = 0;
		if ( m_collider ) {
			footOffset = m_collider->GetFootOffset();
		}
		Real feetHeight = footOffset - groundDistance;
		m_collider->SetFootOffset( feetHeight );
		m_collider->SetHeight( m_stats->fStandingHeight - feetHeight );
	}

	// Calculate movespeed percentage
	Real t_movespeedPercent = ( vMoveVelocity.magnitude() - m_stats->fRunSpeed*0.35f )/( m_stats->fSprintSpeed - m_stats->fRunSpeed*0.35f );
	t_movespeedPercent = Math.Clamp( t_movespeedPercent, 0.0f, 1.0f );
	Real t_animationSpeed = 70.0f + 15.0f * t_movespeedPercent;

	// Keep playing the animation
	m_model->PlayAnimation( "vault_4h" );

	//bool ending = (!bVaultObject)&&(!onGround || groundDistance > 0.5f || vVaultTime < -0.5f);
	bool ending = ( vVaultTime < ( 1.0f - (100-30.0f*t_movespeedPercent)/t_animationSpeed ) );
	if ( ending )
	{
		//m_model->GetAnimationState()->FindAction("vault_4h")->weight = 0.95f;
		//m_model->GetAnimationState()->FindAction("vault_4h")->isPlaying = false; // stop playing
		//m_model->GetAnimationState()->FindAction("vault_4h")->end_behavior = 2; // fade out at end
		m_model->StopMoveAnimation( "vault_4h" ); // this will fade out automatically
		m_rigidbody->SetGravity( true );
		return quickReturn( mvt_OnGround );
	}

	return quickReturn( mvt_VaultHop );
}
void*	CCharacterMotion::mvt_VaultClimb ( void )
{
	Real tTargetHeight = m_stats->fStandingHeight - vVaultHeight;
	//MvtSetPlayerHeight( tTargetHeight );

	CamSetFOVTarget( 0 );
	CamSetRollTarget( 7 );
	CamSetAnimationStick( true );

	// Decrement vaulting time
	vVaultTime -= Time::deltaTime;
	// Check the ground
	float groundDistance = 0.0f;
	bool onGround = OnGround( groundDistance ); 
	// If on ground, then hit the thing we're passing over (or going too long, then limit it)
	/*if ( onGround || groundDistance < 0.5f || vVaultTime < 0.0f ) {
		bVaultObject = false; // Leaving object faulting
	}*/

	// Rotate the move vector to match the camera
	Matrix4x4 rotMatrix;
	rotMatrix.setRotation( vCharRotation );
	Vector3d vForwardDirection = rotMatrix * Vector3d( 1,0,0 );

	// Move forward based on the current facing direction + sprinting speed
	Vector3d vRigidbodySourceVelocity = m_rigidbody->GetVelocity();
	vRigidbodySourceVelocity.z = 0.5f;
	vRigidbodySourceVelocity += vForwardDirection*5;
	vRigidbodySourceVelocity = vRigidbodySourceVelocity.normal() * m_stats->fRunSpeed;
	m_rigidbody->SetVelocity( vRigidbodySourceVelocity );

	bool ending = ( onGround || vVaultTime < -0.5f || groundDistance < 0.5f );
	if ( ending )
	{
		m_rigidbody->SetGravity( true );
		return quickReturn( mvt_OnGround );
	}

	return quickReturn( mvt_VaultClimb );
}

// ------
// Player death animation
// ------
void*	CCharacterMotion::mvt_Saviorize ( void )
{
	// Saviorize is where the stumble animation plays. The player then can press inputs repeatedly to get back up.
	/*float groundDistance = 0.0f;
	bool onGround = OnGround( groundDistance );
	if ( !onGround )
	{
		//model->StopMoveAnimation( "falldown_idle_0" );
		//model->StopMoveAnimation( "falldown_idle_1" );
		//return quickReturn( mvt_Falling );
		// Then we're fucked. Fall some shit, can't really move.
		return quickReturn( mvt_Saviorize );
	}
	else
	{
		// Set vertical velocity
		vMoveVelocity = m_rigidbody->GetVelocity();
		vMoveVelocity.x = 0;
		vMoveVelocity.y = 0;

		// Reset turn input
		//vTurnInput.x = 0;
		ftype turnAmount;
		turnAmount = std::min<ftype>( fabs(vTurnInput.x), 720.0f * Time::deltaTime ) * Math.sgn<ftype>(vTurnInput.x);
		vTurnInput.x -= turnAmount;
		vCharRotation.z += turnAmount * 0.5f;

		// Set rigidbody's velocity
		m_rigidbody->SetVelocity( vMoveVelocity );

		// If not going to die for 0.3 seconds, then go to other mode.
		if ( fBleedTime > 0.3f )
		{
			fOnGroundMoveThreshold = 0;
			fOnGroundTurnThreshold = 0;
			iOnGroundState = 0;

			return quickReturn( mvt_FellOnBack );
		}
	
		// Play on ground animation
		m_model->PlayScriptedAnimation( "falldown_idle_1",30.0f );
		m_model->SetMoveAnimation( "falldown_idle_1" );
		m_animator->SetMoveAnimation( NPC::MoveAnim_USER );

		return quickReturn( mvt_Saviorize );
	}*/
	return quickReturn( mvt_FellOnBackStart );
}
//#include "CScreenFade.h"
void*	CCharacterMotion::mvt_Dead ( void )
{
	if ( m_rigidbody )
	{
		delete m_rigidbody;
		m_rigidbody = NULL;
		//fDeathTimer = 0;	// Reset death timer
		
		// Create fadeout screen
		//new CScreenFade( false, 2.1f, 0.0f );
	}

	// Disable input
	//bHasInput = false;

	// Move the body down until dead
	float groundDistance = 0.0f;
	bool onGround = OnGround( groundDistance );
	m_character->transform.position.z -= groundDistance*0.5f*Time::deltaTime;

	// Play on ground animation for now
	//model->SetMoveAnimation( "prone_idle" );
	m_animator->SetMoveAnimation( NPC::MoveAnim_USER );
	// For now, just play anim (done in OnDeath)

	// Reset view angle effects
	CamSetShakeTarget( 0.0f );
	CamSetRollTarget( 0.0f );
	CamSetFOVTarget( 0.0f );
	CamSetAnimationStick( true ); // Enable animation sticking

	// Increment death timer
	/*fDeathTimer += Time::deltaTime;

	// After 2 seconds of death, reset the player
	if ( fDeathTimer > 2.0f ) {
		return quickReturn( mvt_DeadRecover );
	}*/

	return quickReturn( mvt_Dead );
}

void*	CCharacterMotion::mvt_DeadRecover ( void )
{
	/*if ( !bAlive )
	{
		// stop death animation
		{
			CAnimation* anim = m_model->GetAnimationState();
			CAnimAction* death;
			if ( death = anim->FindAction( "die_forward" ) ) {
				death->Stop();
			}
			if ( death = anim->FindAction( "die_backward" ) ) {
				death->Stop();
			}
		}

		// Create fadeout screen
		//new CScreenFade( true, 1.0f, 1.0f );

		m_character->stats.fHealth	= m_character->stats.fHealthMax;
		m_character->stats.fStamina	= m_character->stats.fStaminaMax * 0.5f;
		m_character->stats.fMana	= m_character->stats.fManaMax * 0.5f;
		//charHealth = stats.fHealth;

		GenerateSpawnpoint();

		// If there's collision, then set alive to true (so the collision may be created)
		if ( Zones.IsCollidableArea( transform.position ) ) {
			OnSpawn();
			bAlive = true;
		}

		return quickReturn( mvt_DeadRecover );
	}
	else
	{
		delete pPlayerCollision;
		InitMovement();

		return quickReturn( mvt_OnGround );
	}*/
	if ( !m_character->IsAlive() )
	{
		return quickReturn( mvt_DeadRecover );
	}
	else
	{
		delete m_collider;
		InitMovement();

		return quickReturn( mvt_OnGround );
	}
}


// ------
// Player stun
// ------
void*	CCharacterMotion::mvt_Stunned ( void )
{
	// Set vertical velocity
	vMoveVelocity = m_rigidbody->GetVelocity();
	vMoveVelocity.x = 0;
	vMoveVelocity.y = 0;

	// Reset turn input
	vTurnInput.x = 0;

	// Set rigidbody's velocity
	m_rigidbody->SetVelocity( vMoveVelocity );

	// Reset view angle effects
	CamSetShakeTarget( 0.0f );
	CamSetRollTarget( 0.0f );
	CamSetFOVTarget( 0.0f );
	CamSetAnimationStick( true );

	// Play anim
	m_model->SetMoveAnimation( "anim" );
	m_animator->SetMoveAnimation( NPC::MoveAnim_USER );

	// Work the stun timer
	//bIsStunned = true;
	fStunTimer -= Time::deltaTime;
	if ( fStunTimer <= 0 ) {
		//bIsStunned = false;
		m_model->BlendToAnimation( 0.4f );
		if ( m_moveTypeNext ) {
			motion_t temp = m_moveTypeNext;
			m_moveTypeNext = NULL;
			return updateMoveType( temp );
		}
		else {
			return quickReturn( mvt_OnGround );
		}
	}

	return quickReturn( mvt_Stunned );
}

// ------
// Fallen
// ------
void*	CCharacterMotion::mvt_FellOnBackStart ( void )
{
	m_model->PlayScriptedAnimation( "falldown_idle_0",30.0f );
	m_character->ApplyStun( "falldown_start", -1.0f );
	m_moveTypeNext = func(mvt_FellOnBack);
	fOnGroundTurnThreshold = 0;
	fOnGroundMoveThreshold = 0;
	iOnGroundState = 0;
	//model->SetMoveAnimation( "falldown_idle_0" );
	return quickReturn( mvt_Stunned );
}
void*	CCharacterMotion::mvt_FellOnBack ( void )
{
	float groundDistance = 0.0f;
	bool onGround = OnGround( groundDistance );
	if ( !onGround )
	{
		m_model->StopMoveAnimation( "falldown_idle_0" );
		m_model->StopMoveAnimation( "falldown_idle_1" );
		return quickReturn( mvt_Falling );
	}
	else
	{
		// Set vertical velocity
		vMoveVelocity = m_rigidbody->GetVelocity();
		vMoveVelocity.x = 0;
		vMoveVelocity.y = 0;

		// Reset turn input
		//vTurnInput.x = 0;
		ftype turnAmount;
		turnAmount = std::min<ftype>( fabs(vTurnInput.x), 720.0f * Time::deltaTime ) * Math.sgn<ftype>(vTurnInput.x);
		vTurnInput.x -= turnAmount;

		if ( iOnGroundState == 0 ) {
			// On ground, turn to sit up
			fOnGroundMoveThreshold += turnAmount * 0.1f;
		}
		else if ( iOnGroundState == 1 ) {
			vCharRotation.z += turnAmount * 0.5f;
			fOnGroundTurnThreshold += turnAmount * 0.5f;
		}

		// Apply directional inputs
		fOnGroundMoveThreshold += m_input->vDirInput.x * 15.0f * Time::deltaTime;

		// Set rigidbody's velocity
		m_rigidbody->SetVelocity( vMoveVelocity );

		// Decrease threshold over time
		if ( fOnGroundMoveThreshold > 0.0f ) {
			fOnGroundMoveThreshold -= Time::deltaTime;
		}
		// If threshold big enough, then move
		if ( iOnGroundState == 0 ) {
			if ( fOnGroundMoveThreshold > 4.0f ) {
				iOnGroundState = 1;
				fOnGroundMoveThreshold = 0;
			}	
		}
		else {
			if ( fOnGroundMoveThreshold > 4.0f ) {
				fOnGroundMoveThreshold = 0;
				m_model->StopMoveAnimation( "falldown_idle_0" );
				m_model->StopMoveAnimation( "falldown_idle_1" );
				return quickReturn( mvt_FellOnBackUp );
			}
		}
	
		// If turning threshold high enough, then go to prone
		if ( fOnGroundTurnThreshold > 90.0f ) {
			bIsProne = true;
			m_model->StopMoveAnimation( "falldown_idle_0" );
			m_model->StopMoveAnimation( "falldown_idle_1" );
			return quickReturn( mvt_OnGround );
		}

		// Play on ground animation
		if ( iOnGroundState == 0 ) {
			m_model->SetMoveAnimation( "falldown_idle_0" );
		}
		else {
			m_model->SetMoveAnimation( "falldown_idle_1" );
		}
		m_animator->SetMoveAnimation( NPC::MoveAnim_USER );

		return quickReturn( mvt_FellOnBack );
	}
}
void*	CCharacterMotion::mvt_FellOnBackUp ( void )
{
	m_model->PlayScriptedAnimation( "falldown_end",30.0f );
	m_animator->SetMoveAnimation( NPC::MoveAnim_USER );

	fOnGroundMoveThreshold += Time::deltaTime;
	if ( fOnGroundMoveThreshold > 1.0f ) {
		return quickReturn( mvt_OnGround );
	}

	return quickReturn( mvt_FellOnBackUp );
}

// ------
// Mobility
// ------
bool	CCharacterMotion::CheckWallSliding ( void )
{
	Matrix4x4 rotMatrix;
	rotMatrix.setRotation( vCharRotation );

	// todo: want to check the distance to the ground as well (from ledge to ground, as that may make it ungrabbable)
	vHangingWall = Vector3d(0,0,0);

	Ray c_trace;
	RaycastHit c_trace_result;
	RaycastHit c_first_ledgetrace_result;

	c_trace.dir = rotMatrix * Vector3d( 1,0,0 );
	for ( ftype v_distance = fPlayerHeight-0.9f; v_distance > 0.2f; v_distance -= 1.7f )
	{
		// raytrace towards a wall
		c_trace.pos = m_character->transform.position + Vector3d( 0,0,v_distance );

		Raycaster.Raycast( c_trace, m_stats->fPlayerRadius+0.4f, &c_trace_result, 0x00, m_character );
		if ( c_trace_result.hit ) {
			// Save the wall's normal
			vHangingWall += c_trace_result.hitNormal;
			// And save the hit's position
			vHangingPosition = c_trace_result.hitPos;
		}
		else {
			return false;
		}
	}

	// Generate the sliding position based off the wall's normal and player radius
	vHangingWall.normalize();
	vHangingPosition.z = m_character->transform.position.z;
	vHangingPosition += vHangingWall*(m_stats->fPlayerRadius+0.06f);

	// At this point, we know the sliding area is free.
	// However, we also want to check the floor in front of the wall. There has to be some room to actually slide.
	c_trace.dir = Vector3d( 0,0,-1 );
	c_trace.pos = vHangingPosition + vHangingWall + Vector3d( 0,0, 0.1f );

	Raycaster.Raycast( c_trace, 2.1f, &c_trace_result, 0x00 );
	if ( !c_trace_result.hit )
	{
		// If there's no hit, then the area to grab is free

		// reset input
		fWallSlideTurnThreshold = 0;
		fWallSlideTurnSum = 0;
		bWallSlideTurned = false;
		return true;
	}

	return false;
}
void*	CCharacterMotion::mvt_WallSlide ( void )
{
	// Turn first
	//vCharRotation.z += min<ftype>( vTurnInput.x*0.1f, 45.0f * Time::deltaTime ); //but at a slower rate
	ftype turnAmount = std::min<ftype>( fabs(vTurnInput.x), 360.0f * Time::deltaTime ) * Math.sgn<ftype>(vTurnInput.x);
	fWallSlideTurnThreshold += turnAmount * 0.1f; //but at a slower rate
	if ( fabs(fWallSlideTurnThreshold) > 15.0f ) {
		fWallSlideTurnSum += 180 * Math.sgn<ftype>(fWallSlideTurnThreshold);
		fWallSlideTurnThreshold = 0;
		bWallSlideTurned = !bWallSlideTurned;

		fSlideCounter -= 0.2f;
	}
	vTurnInput.x -= turnAmount;

	vCharRotation.z += fWallSlideTurnSum * Time::TrainerFactor( 0.11f );
	fWallSlideTurnSum -= fWallSlideTurnSum * Time::TrainerFactor( 0.11f );

	// Set player to full height
	MvtSetPlayerHeightStick( m_stats->fStandingHeight );

	// Increment slide counter
	fSlideCounter += Time::deltaTime;
	// If at end of the slide time
	if ( fSlideCounter >= fWallSlideTime ) {
		fSlideCounter *= 0.5f; // Decrease cooldown
		return quickReturn( mvt_Falling );
	}
	// In Water
	if ( WaterTester::Get()->PositionInside( m_character->transform.position+Vector3d( 0,0,2.5f ) ) ) {
		fSlideCounter = 0.15f; // Set cooldown
		return quickReturn( mvt_Swimming );
	}
	// Check if on ground
	float groundDistance = 0.0f;
	bool onGround = OnGround( groundDistance );
	if ( onGround ) { // Decrease time much quicker if on ground
		fSlideCounter += Time::deltaTime*5.0f;
	}

	// Step away from wall if press backwards
	if ( !bWallSlideTurned ) {
		if ( m_input->vDirInput.x < -0.5f ) {
			fSlideCounter *= 0.33f; // Decrease cooldown
			return quickReturn( mvt_Falling );
		}
	}

	// No movement
	// Rather, slowly reduce movement (and move head to target position)
	Vector3d targetPosition = Vector3d( vHangingPosition.x, vHangingPosition.y, m_rigidbody->GetPosition().z );
	m_rigidbody->SetPosition( m_rigidbody->GetPosition()*0.8f + targetPosition*0.2f );

	// Get the move vector from the current direction
	vMoveVelocity = m_rigidbody->GetVelocity();
	vMoveVelocity.x *= (0.9f-Time::deltaTime);
	vMoveVelocity.y *= (0.9f-Time::deltaTime);
	// Set vertical velocity
	//vMoveVelocity.z *= (0.95f-Time::deltaTime*0.5f);
	//vMoveVelocity.z *= Time::TrainerFactor( 0.4f );
	vMoveVelocity.z *= Time::SqrtTrainerFactor( 0.5f );
	vMoveVelocity.z += -0.13f * Time::deltaTime;

	// Reset fall damage
	vFallingVelocity = vMoveVelocity;

	// If jump, then jump off the wall
	if ( m_input->axes.jump.pressed() && (fSlideCounter > 0.4f) )
	{
		m_input->axes.jump.Skip();
		if ( bWallSlideTurned ) {
			vMoveVelocity = Vector3d(vHangingWall.x,vHangingWall.y,vHangingWall.z+1.3f).normal()*m_stats->fJumpVelocity*1.6f;
			m_rigidbody->SetVelocity( vMoveVelocity );

			// Subtract stamina
			m_character->stats.fStamina -= 3.0f;

			// Play longjump animation
			m_model->PlayAnimation( "jump_sprint" );

			// Do jump sound
			m_character->DoSpeech( NPC::SpeechType_Jump );

			// Reset slide timer
			//fSlideCounter = -0.01f;
			fSlideCounter = fWallSlideTime*0.125f;

			// Reset fall damage counter
			EndFalling();

			return quickReturn( mvt_Falling );
		}
		else {
			vHeadRotation.y -= 10;
			fWallSlideTurnSum += 180;
			fWallSlideTurnThreshold = 0;
			bWallSlideTurned = true;

			fSlideCounter -= 0.2f;
		}
	}

	// Set rigidbody's velocity
	m_rigidbody->SetVelocity( vMoveVelocity );
	// Set camera viewbob effects
	//camViewBobSetSpeed( vMoveVelocity.sqrMagnitude() * 0.05f * ( (vMoveVelocity.z<0) ? 2.0f : 1.0f )  );
	CamSetShakeTarget( vMoveVelocity.sqrMagnitude()*0.002f );
	CamSetFOVTarget( 5 );
	//camRollSetOffset( (0.5f - fabs( (fSlideCounter/fRollTime) - 0.5f )) * 23.0f );
	CamSetAnimationStick( true );

	m_animator->SetMoveAnimation( NPC::MoveAnimFall );

	return quickReturn( mvt_WallSlide );
}

void*	CCharacterMotion::mvt_WallClimb ( void ) // run up wall
{
	bWallRunReady = false;
	// Lock player facing to face the wall after second step

	// need to link vertical movement with footstep event
	// (footstep event, increment the footstep count, and move upwards)

	// Count footstep timer
	fWallClimbCounter += Time::deltaTime;
	// On footstep, move upwards
	if ( fWallClimbCounter >= fWallClimbStepTime ) {
		//fWallClimbCounter -= fWallClimbStepTime*1.2f; // Decrement timer
		fWallClimbCounter = 0.0f;
		iWallRunStepCount += 1;
	}
	
	// Set velocity upwards
	vMoveVelocity = Vector3d( 0,0, m_stats->fJumpVelocity * 1.1f * ((fWallClimbStepTime-fWallClimbCounter*1.2f)/fWallClimbStepTime) );
	m_rigidbody->SetVelocity( vMoveVelocity );

	// Create effect stuff
	if ( iWallRunStepCount >= 1 ) {
		// Roll camera alternating between feet
		CamSetFOVTarget( vMoveVelocity.sqrMagnitude()*0.002f );
		CamSetRollTarget( 3.0f * ((iWallRunStepCount%2) ? 1 : -1) );
		// Bob view in relation to hitting the wall
		//camViewBobSetSpeed( vMoveVelocity.sqrMagnitude() * 0.05f * 2 );
		CamSetShakeTarget( vMoveVelocity.sqrMagnitude()*0.002f );
	}

	// Ledge grabbing
	if ( CheckLedgeGrabbing() )
	{
		if ( m_input->axes.jump ) {
			return quickReturn( mvt_ClimbStart );
		}
	}

	// When the wall that we're running on no longer has a place to run
	if ( !CheckWallRunClimbing() ) {
		// Boost forward, and return falling movement
		return quickReturn( mvt_Falling );
	}

	// When wall run step count is equal to max step, move to falling state
	if ( iWallRunStepCount >= m_stats->iWallRunStepCount ) {
		return quickReturn( mvt_Falling );
	}

	return quickReturn( mvt_WallClimb );
}

bool	CCharacterMotion::CheckWallRunClimbing ( void )
{
	Matrix4x4 rotMatrix;
	rotMatrix.setRotation( vCharRotation );

	// Result hanging wall normal
	vHangingWall = Vector3d(0,0,0);

	Ray c_trace;
	RaycastHit c_trace_result;
	RaycastHit c_first_ledgetrace_result;
	bool	hasHit = false;

	c_trace.dir = rotMatrix * Vector3d( 1,0,0 );
	for ( ftype v_distance = 0.6f; v_distance < 1.8f; v_distance += 0.7f )
	{
		// raytrace towards a wall
		c_trace.pos = m_character->transform.position + Vector3d( 0,0,v_distance );

		Raycaster.Raycast( c_trace, m_stats->fPlayerRadius+0.4f, &c_trace_result, 0x00, m_character );
		if ( c_trace_result.hit ) {
			// Check that the normal of the wall is close to a right angle
			if ( c_trace.dir.dot( c_trace_result.hitNormal ) < -0.8f ) {
				hasHit = true; // Flag that there's a foothold
				// Save the wall's normal
				vHangingWall += c_trace_result.hitNormal;
			}
		}
	}

	// Return failure if no footholds
	if ( !hasHit ) {
		return false;
	}

	// Normalize wall normal
	vHangingWall.normalize();

	return true;
}

//
void* CCharacterMotion::mvt_WallRun ( void )
{
	// Count footstep timer
	fWallClimbCounter += Time::deltaTime;

	// Subtract stamina at sprinting speed
	{
		m_character->stats.fStamina -= Time::deltaTime * 8.0f;
	}

	// On footstep, move upwards
	while ( fWallClimbCounter >= fWallClimbStepTime ) {
		//fWallClimbCounter -= fWallClimbStepTime*1.2f; // Decrement timer
		fWallClimbCounter -= fWallClimbStepTime;
		iWallRunStepCount += 1;
	}

	// When hit step limit, go to falling (to hold the forward momentum!)
	if ( iWallRunStepCount >= m_stats->iWallRunStepCount*3 )
	{
		m_rigidbody->SetGravity( true );

		// Reset slide timer
		bWallRunReady = false;
		fSlideCounter = -1;//fWallSlideTime*0.125f;
		
		// Start falling
		return quickReturn( mvt_Falling );
	}

	// Set motion based on the wall
		// Set velocity upwards
	//vMoveVelocity = //Vector3d( 0,0, pl_race_stats->fJumpVelocity * 1.1f * ((fWallClimbStepTime-fWallClimbCounter*1.2f)/fWallClimbStepTime) );
	
	// Update wallrunning normal and position
	bool onWall = CheckWallRunLateral();
	if ( !onWall ) {
		fWallClimbCounter += Time::deltaTime * 10.0f;
	}

	// Set the movement based on the wall. (Need to be constantly checking the wall, so as to be able to curve around surfaces)

	// First use wall normal and current facing normal to create the perpendicular move direction
	Vector3d wallTravelDirection;

	Matrix4x4 rotMatrix;
	rotMatrix.setRotation( vCharRotation ); // Facing direction controls movement direction
	Vector3d forwardDir = rotMatrix * Vector3d( 1,0,0 );
	Vector3d sideDir = rotMatrix * Vector3d( 0,1,0 );
	Vector3d perpendicularDir = vHangingWall.cross( Vector3d::up );
	// Choose the proper direction
	if ( forwardDir.dot( perpendicularDir ) > 0 ) {
		wallTravelDirection = perpendicularDir;
	}
	else {
		wallTravelDirection = -perpendicularDir;
	}
	// Flatten the travel direction
	wallTravelDirection.z = 0;
	wallTravelDirection.normalize();

	// Move the screen around
	ftype rollDirection = ( (sideDir.dot(vHangingWall)>0) ? 1.0f : -1.0f );
	ftype turnAmount;
	{
		// Horizontal turn
		turnAmount = std::min<ftype>( fabs(vTurnInput.x), 720.0f * Time::deltaTime ) * Math.sgn<ftype>(vTurnInput.x);
		vHeadRotation.z += turnAmount;
		vTurnInput.x -= turnAmount;
		// Vertical turn
		turnAmount = std::min<ftype>( fabs(vTurnInput.y), 720.0f * Time::deltaTime ) * Math.sgn<ftype>(vTurnInput.y);
		vHeadRotation.y -= turnAmount;
		vTurnInput.y -= turnAmount;
	}
	// Remove head rotation
	if ( fabs( vHeadRotation.y ) > 80 ) {
		vHeadRotation.y = 80.0f * Math.sgn<ftype>( vHeadRotation.y );
	}
	turnAmount = -60 - 30*rollDirection;
	if ( vHeadRotation.z < turnAmount ) {
		vHeadRotation.z = turnAmount;
	}
	turnAmount = +60 - 30*rollDirection;
	if ( vHeadRotation.z > turnAmount ) {
		vHeadRotation.z = turnAmount;
	}

	// Move the character to match to the wall
	if ( onWall )
	{
		Vector3d targetPosition = Vector3d( vHangingPosition.x, vHangingPosition.y, m_rigidbody->GetPosition().z );
		m_rigidbody->AddToPosition( (targetPosition - m_rigidbody->GetPosition()) * 0.3f );
	}

	// Change move velocity based on wall run
	{
		m_rigidbody->SetGravity( false );
		vMoveVelocity = m_rigidbody->GetVelocity();

		// Add a small amount of gravity
		if ( vMoveVelocity.z > 6.0f ) { 
			vMoveVelocity.z = 6.0f;
		}
		else if ( vMoveVelocity.z < -2.0f ) {
			vMoveVelocity.z = -2.0f;
		}
		vMoveVelocity.z -= Time::deltaTime * 9.0f;
		//vMoveVelocity.z = 0;
		// Slow down movement over time
		Real t_speed = std::min<Real>( 1.0f, 1.5f - ((fWallClimbCounter + fWallClimbStepTime*iWallRunStepCount) / (fWallClimbStepTime*m_stats->iWallRunStepCount*3))*0.5f );
		vMoveVelocity.x = wallTravelDirection.x * m_stats->fSprintSpeed * t_speed;
		vMoveVelocity.y = wallTravelDirection.y * m_stats->fSprintSpeed * t_speed;
		//std::cout << "TV: " << wallTravelDirection << std::endl;
		//std::cout << "MV: " << vMoveVelocity << std::endl;
		//std::cout << "SD: " << t_speed << std::endl;
		m_rigidbody->SetVelocity( vMoveVelocity );
	}

	// Rotate the view
	{
		// Based on wallTravelDirection, get a target value for vCharRotation.z.
		Quaternion currentRot;
		currentRot.SetEulerAngles( Vector3d(0,0,Math.Wrap(-vCharRotation.z,-180,180)) );
		Quaternion targetRot = Quaternion::CreateRotationTo( Vector3d(1,0,0), wallTravelDirection );
		//std::cout << "TR: " << targetRot << std::endl;
		//std::cout << "PR: " << currentRot << std::endl;
		//std::cout << "VZ: " << vCharRotation.z << std::endl;
		currentRot = currentRot.Slerp( targetRot, 0.1f );
		vCharRotation.z = currentRot.GetEulerAngles().z;
	}

	// If input is jump, do a jump off the wall
	if ( m_input->axes.jump.pressed() && ((fWallClimbCounter + fWallClimbStepTime*iWallRunStepCount) > 0.25f) )
	{
		m_input->axes.jump.Skip();

		Matrix4x4 rotLookMatrix;
		rotLookMatrix.setRotation( vCharRotation+vHeadRotation ); // Facing direction controls movement direction
		Vector3d lookForward = rotLookMatrix * Vector3d( 1,0,0 );
		if ( fabs(lookForward.dot( wallTravelDirection )) > 0.707 ) {
			vMoveVelocity = Vector3d(vHangingWall.x+lookForward.x,vHangingWall.y+lookForward.y,vHangingWall.z+1.3f).normal()*m_stats->fJumpVelocity*1.6f;
		}
		else {
			vMoveVelocity = Vector3d(lookForward.x,lookForward.y,lookForward.z+1.3f).normal()*m_stats->fJumpVelocity*1.6f;
		}
		m_rigidbody->SetVelocity( vMoveVelocity );

		// Subtract stamina
		m_character->stats.fStamina -= 4.0f;

		// Play longjump animation
		m_model->PlayAnimation( "jump_sprint" );
		// Do jump sound
		m_character->DoSpeech( NPC::SpeechType_Jump );

		// Reset slide timer
		bWallRunReady = false;
		//fSlideCounter = -0.01f;
		fSlideCounter = fWallSlideTime*0.125f;

		// Reset fall damage counter
		EndFalling();
		// Reenable gravity
		m_rigidbody->SetGravity( true );

		// Rotate body
		vCharRotation.z += vHeadRotation.z * 0.9f;
		vHeadRotation.z *= 0.1f;

		return quickReturn( mvt_Falling );
	}

	// Set camera effects
	{
		// Roll camera based on wall
		CamSetFOVTarget( vMoveVelocity.sqrMagnitude()*0.002f );
		CamSetRollTarget( 14.0f * rollDirection );
		// Bob view in relation to hitting the wall
		//camViewBobSetSpeed( vMoveVelocity.sqrMagnitude() * 0.05f * 2 );
		CamSetShakeTarget( vMoveVelocity.sqrMagnitude()*0.002f );
	}

	if ( rollDirection > 0 ) {
		m_animator->SetMoveAnimation( NPC::MoveAnimMoveWallrunRight );
	}
	else {
		m_animator->SetMoveAnimation( NPC::MoveAnimMoveWallrunLeft );
	}

	// Camera needs a separate LOOK rotation. So camera rotation is both LOOK and FACE.

	return quickReturn( mvt_WallRun );
}

bool CCharacterMotion::CheckWallRunLateral ( void )
{
	Matrix4x4 rotMatrix;
	rotMatrix.setRotation( vCharRotation );

	Ray c_trace;
	RaycastHit c_trace_result;
	RaycastHit c_first_ledgetrace_result;
	bool	hasHit = false;

	int hangingHitCount;
	Vector3d nextHangingPosition;
	Vector3d nextHangingWall;
	ftype	maxDistance;

	// Check left wall
	nextHangingPosition = Vector3d( 0,0,0 );
	nextHangingWall = Vector3d( 0,0,0 );
	hangingHitCount = 0;
	maxDistance = 0;
	c_trace.dir = rotMatrix * Vector3d( 0,1,0 );
	for ( ftype v_distance = 0.6f; v_distance < 1.8f; v_distance += 0.7f )
	{
		// raytrace towards a wall
		c_trace.pos = m_character->transform.position + Vector3d( 0,0,v_distance );

		Raycaster.Raycast( c_trace, m_stats->fPlayerRadius+0.4f, &c_trace_result, 0x00, m_character );
		if ( c_trace_result.hit ) {
			// Check that the normal of the wall is close to a right angle
			if ( c_trace.dir.dot( c_trace_result.hitNormal ) < -0.85f ) {
				hasHit = true; // Flag that there's a foothold
				// Save the wall's normal
				nextHangingWall += c_trace_result.hitNormal;
				// Save the hit position to build the foothold placement
				if ( c_trace_result.distance > maxDistance ) {
					maxDistance = c_trace_result.distance;
					nextHangingPosition = c_trace_result.hitPos;
					//hangingHitCount += 1;
				}
			}
		}
	}
	// We hit the left wall. Success.
	if ( hasHit ) {
		// Normalize wall normal
		nextHangingWall.normalize();
		vHangingWall = nextHangingWall;
		// Build hanging spot
		//nextHangingPosition /= hangingHitCount;
		nextHangingPosition += vHangingWall*(m_stats->fPlayerRadius+0.06f);
		nextHangingPosition.z = m_character->transform.position.z;
		vHangingPosition = nextHangingPosition;
		return true;
	}

	// Check right wall
	nextHangingPosition = Vector3d( 0,0,0 );
	nextHangingWall = Vector3d( 0,0,0 );
	hangingHitCount = 0;
	maxDistance = 0;
	c_trace.dir = rotMatrix * Vector3d( 0,-1,0 );
	for ( ftype v_distance = 0.6f; v_distance < 1.8f; v_distance += 0.7f )
	{
		// raytrace towards a wall
		c_trace.pos = m_character->transform.position + Vector3d( 0,0,v_distance );

		Raycaster.Raycast( c_trace, m_stats->fPlayerRadius+0.4f, &c_trace_result, 0x00, m_character );
		if ( c_trace_result.hit ) {
			// Check that the normal of the wall is close to a right angle
			if ( c_trace.dir.dot( c_trace_result.hitNormal ) < -0.85f ) {
				hasHit = true; // Flag that there's a foothold
				// Save the wall's normal
				nextHangingWall += c_trace_result.hitNormal;
				// Save the hit position to build the foothold placement
				if ( c_trace_result.distance > maxDistance ) {
					maxDistance = c_trace_result.distance;
					nextHangingPosition = c_trace_result.hitPos;
					//hangingHitCount += 1;
				}
			}
		}
	}
	// We hit the right wall. Success.
	if ( hasHit ) {
		// Normalize wall normal
		nextHangingWall.normalize();
		vHangingWall = nextHangingWall;
		// Build hanging spot
		//nextHangingPosition /= hangingHitCount;
		nextHangingPosition += vHangingWall*(m_stats->fPlayerRadius+0.06f);
		nextHangingPosition.z = m_character->transform.position.z;
		vHangingPosition = nextHangingPosition;
		return true;
	}

	// Return failure if no footholds
	return false;
}

// ------
// Skills
// ------
void* CCharacterMotion::mvt_Minidash ( void )
{
	// Get the move vector from the current direction
	Vector3d moveVector(0,0,0);
	//moveVector = ;
	//ftype moveSpeed = moveVector.magnitude();
	Vector3d currentMoveVector = m_rigidbody->GetVelocity();

	Vector3d testCase = currentMoveVector;
	testCase.z = 0;

	// Check for if we hit a wall in front of us
	// Todo: move to a separate function
	const uint32_t hitFilter = Physics::GetCollisionFilter(Layers::PHYS_SWEPTCOLLISION,0,31);
	bool hitWall = false;
	Ray ray;
	ray.dir = testCase.normal();
	ray.pos = m_character->transform.position + Vector3d( 0,0,1 );
	RaycastHit result;
	if ( Raycaster.Raycast( ray, 4.0f, &result, hitFilter ) )
	{
		if ( result.distance < 2.0f )
		{
			hitWall = true;
		}
	}

	// Increment slide counter
	fSlideCounter += Time::deltaTime;
	// If we hit a wall or the end of the slide time
	//if (( fSlideCounter >= 0.4f )||( hitWall )||( currentMoveVector.sqrMagnitude() < 1.7f )) {
	if ( fSlideCounter > 0.3f || hitWall ) {
		fSlideCounter = 0.5f;
		return quickReturn( mvt_OnGround );
	}
	// Do actual slide movement

	// Override crouch input
	//fInput[iCrouch] = 1.0f;
	//MvtCommonCrouching();
	//MvtSetPlayerHeightStick( (fPronePlayerHeight+fCrouchPlayerHeight)*0.5f ); // Set player height rather low
	MvtSetPlayerHeightStick( (m_stats->fCrouchingHeight+m_stats->fStandingHeight)*0.5f ); // Set player height to between 2 and 3 blocks

	// Turn first
	ftype turnAmount = std::min<ftype>( fabs(vTurnInput.x), 360.0f * Time::deltaTime ) * Math.sgn<ftype>(vTurnInput.x);
	vCharRotation.z += turnAmount * 0.1f; //but at a slower rate
	vTurnInput.x -= turnAmount;

	// Offset the turning to the current facing direction
	{
		moveVector = m_input->vDirInput;
		moveVector.z = 0;
		moveVector.x += 0.8f;
		moveVector.y *= 0.5f;
		//moveVector.z = 0;
		//moveVector = Vector3d( 1,0,0 );
		// Rotate the move vector to match the camera
		Matrix4x4 rotMatrix;
		rotMatrix.setRotation( vCharRotation );
		moveVector = rotMatrix*moveVector;
		// Add the move vector to the test case
		testCase.x += moveVector.x*1.0f;
		testCase.y += moveVector.y*1.0f;
	}
	// Set the movespeed to constant sprintspeed
	//testCase = testCase.normal() * fSprintSpeed * 1.2f;
	testCase = testCase.normal() * m_stats->fSprintSpeed * 1.2f;// * (1.2f - (fSlideCounter/fSlideTime)*0.8f);

	// Set the move vector at a constant value
	moveVector = testCase;

	// Set the horizontal velocity
	vMoveVelocity = moveVector;
	// Set vertical velocity
	vMoveVelocity.z = currentMoveVector.z;

	// Set rigidbody's velocity
	m_rigidbody->SetVelocity( vMoveVelocity );
	// Reduce view bob to mostly nil, but very fast
	//camViewBobSetSpeed( 34.0f + testCase.magnitude()/fSprintSpeed );
	//camViewBobSetSpeed( 76.0f + testCase.magnitude()/m_stats->fSprintSpeed*10.0f );
	CamSetShakeTarget( testCase.sqrMagnitude()*0.002f );
	CamSetFOVTarget( 7 );
	//camRollSetOffset( -7 );
	//bUseAnimCamMode = true;

	//model->SetMoveAnimation( "dash" );
	//SetMoveAnimation( MoveAnim_USER );

	return quickReturn( mvt_Minidash );
}


// ------
// Combat
// ------
void* CCharacterMotion::mvt_AtkGround( void )
{
	// Call the movement type
	quickReturn( mvt_OnGround );
	quickReturn( mvt_OnGround );
	mvt_OnGround();

	// Change camera motion style
	CamSetAnimationStick( true ); // Animation does stick

	// If still on ground, then return this
	if ( m_moveType == func(mvt_OnGround) )
	{
		return quickReturn( mvt_AtkGround );
	}
	else
	{
		// Else, don't change a thing
		return NULL;
	}
}

void* CCharacterMotion::mvt_AtkAir( void )
{
	// Call the movement type
	mvt_Falling();

	// Change camera motion style
	CamSetAnimationStick( true ); // Animation does stick

	// If still on ground, then return this
	if ( m_moveType == func(mvt_Falling) )
	{
		return quickReturn( mvt_AtkAir );
	}
	else
	{
		// Else, don't change a thing
		return NULL;
	}
}

void* CCharacterMotion::mvt_AtkSlide( void )
{
	// Call the movement type
	mvt_CombatSlide();

	// Change camera motion style
	CamSetAnimationStick( true ); // Animation does stick

	// If still on ground, then return this
	if ( m_moveType == func(mvt_CombatSlide) )
	{
		return quickReturn( mvt_AtkSlide );
	}
	else
	{
		// Else, don't change a thing
		return NULL;
	}
}

#undef quickReturn
#undef func
