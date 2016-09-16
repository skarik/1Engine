

#include "CCharacterMotion.h"
// Animation
#include "core-ext/animation/CAnimation.h"
// Character and stats handling
#include "after/entities/character/CCharacter.h"
#include "after/states/CRacialStats.h"
#include "after/entities/CCharacterModel.h"
// Physics
#include "engine/physics/collider/types/CCapsuleCollider.h"
#include "engine/physics/motion/CRigidBodyCharacter.h"
// Collision detection
#include "engine/physics/raycast/Raycaster.h"
// Input
#include "core-ext/input/CInputControl.h"
// Math utilities
#include "core/math/Math.h"

// Define syntax shortcut
#define quickReturn(a) (updateMoveType( (motion_t)&CCharacterMotion::a ))
#define func(a) ((motion_t)&CCharacterMotion::a )

// Constructor to initialize system bits
CCharacterMotion::CCharacterMotion ( void )
	: m_charIsPlayer(true), // default to player (will check once)
	m_canWallSlide(true), m_canWallRun(true), m_canWallClimb(true), m_canLedgeClimb(true), m_canCombatSlide(true), m_canSprint(true), m_canJump(true)
{
	SetMovetypeVariables();
	SetMovetypeConstants();
}
// Movement initialization. Initializes the rigidbody, collider, and floor triggers.
//  Called in the constructor of CPlayer.
void	CCharacterMotion::InitMovement ( void )
{
	// Initialize movetype variables
	SetMovetypeVariables();
	// Setup movetype constants
	SetMovetypeConstants();

	//pPlayerCollision = new CCylinderCollider ( fPlayerHeight, fPlayerRadius );
	m_collider = new CCapsuleCollider ( fPlayerHeight, m_stats->fPlayerRadius );
	//m_rigidbody = new CRigidBody ( m_collider, m_character, 30 ); //weight should be 30
	m_rigidbody = new CRigidBodyCharacter ( m_collider, m_character, 30 ); //weight should be 30
	m_rigidbody->SetRotationEnabled( false );
	m_rigidbody->SetQualityType( HK_COLLIDABLE_QUALITY_CHARACTER );
	//m_rigidbody->SetQualityType( HK_COLLIDABLE_QUALITY_CRITICAL ); //causes weird bouncing effect
	m_rigidbody->SetRestitution( 0.01f );
	m_rigidbody->SetFriction( 0.02f );
	m_rigidbody->SetCollisionLayer( Layers::PHYS_SWEPTCOLLISION, 2 );

	// Start out with ground
	m_moveType = func(mvt_OnGround);
	m_moveTypeQueued = NULL;
	m_moveTypeNext = NULL;
}
void CCharacterMotion::SetMovetypeVariables ( void )
{
	// Falling tracker
	vFallingVelocity = Vector3d();
	// Unobstruction
	bMoveUnobstructed = true;
	// Autojump
	bOverrideAutojump = false;

	// Initialize states
	bIsCrouching		= false;
	bStartedCrouching	= false;
	bWantsCrouch		= false;

	bIsProne			= false;
	bWantsProne			= false;
	bStartedProne		= false;

	bIsSprinting		= false;
	bCanSprint			= true;

	fPlayerHeight		= 5.7f;

	// Movement states
	vMoveVelocity			= Vector3d( 0,0,0 );
	vMoveLastGroundNormal	= Vector3d( 0,0,1 );
	fSlideCounter		= 0.0f;
	fWallrunCooldown	= 0.0f;

	// Other states
	vVaultHeight		= 0.0f;
	bVaultObject		= false;
	vVaultTime			= 0.0f;

}
void CCharacterMotion::SetMovetypeConstants ( void )
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

	// Ledge vaulting
	fVaultTargetMaxHeight = 4.1f;
	fVaultTargetMaxDistance = 3.0f;

	// Friction and bounce properties
	fRigidbodyNormalFriction = 0.02f;
	fRigidbodySlideFriction = 0.01f;

	fRigidbodyNormalRestitution = 0.01f;
	fRigidbodyMovementRestitution = 0.00f;
}


CCharacterMotion::~CCharacterMotion ( void )
{
	FreeMovement();
}
// Cleanup for the movement.
//  Called in the destructor of CPlayer
void	CCharacterMotion::FreeMovement ( void )
{
	if ( m_rigidbody ) {
		m_rigidbody->RemoveReference();
		delete m_rigidbody;
		m_rigidbody = NULL;
	}
	delete m_collider;
	m_collider = NULL;
}


// Sets bOnGround and returns value of bOnGround.
bool	CCharacterMotion::OnGround ( Real& outGroundDistance )
{
	bool onGround = false;
	bool hasHit = false;
	int hitCount = 0;
	const ftype maxStepHeight = 1.8f;
	ftype footOffset = 0;
	if ( m_collider ) {
		footOffset = m_collider->GetFootOffset();
	}
	outGroundDistance = -10;
	RaycastHit hitResult;
	//const static ftype playerWidth = 1.5f;
	// First check for the ground
	Ray ray;
	ray.dir = Vector3d(0,0,-1);
	for ( short i = 0; i < 5; i += 1 )
	{
		switch ( i )
		{
		case 0:	ray.pos = m_character->transform.position+Vector3d(0,0,maxStepHeight+footOffset);
			break;
		case 1: ray.pos = m_character->transform.position+Vector3d(m_stats->fPlayerRadius*0.95f,0,maxStepHeight+footOffset);
			break;
		case 2: ray.pos = m_character->transform.position+Vector3d(-m_stats->fPlayerRadius*0.95f,0,maxStepHeight+footOffset);
			break;
		case 3: ray.pos = m_character->transform.position+Vector3d(0,m_stats->fPlayerRadius*0.95f,maxStepHeight+footOffset);
			break;
		case 4: ray.pos = m_character->transform.position+Vector3d(0,-m_stats->fPlayerRadius*0.95f,maxStepHeight+footOffset);
			break;
		}

		// if hit
		if ( Raycaster.Raycast( ray, 4.0f+footOffset, &hitResult, Physics::GetCollisionFilter(Layers::PHYS_WORLD_TRACE), m_character ) )
		{
			// then we hit
			hitCount += 1;
			hasHit = true;
			// if the hit distance is valid, and is smaller if outGroundDistance is smaller than 5
			if (( hitResult.distance >= 0 )&&(( hitResult.distance < outGroundDistance )||(outGroundDistance < -5)))
			{
				outGroundDistance = hitResult.distance;
				vMoveLastGroundNormal += hitResult.hitNormal;
			}
		}
		else
		{
			if ( outGroundDistance < 0 ) { // Only set too far if is a straight up miss
				outGroundDistance = 8.0f;
			}
		}
	}
	outGroundDistance -= maxStepHeight;
	if ( outGroundDistance < 0.15f )
	{
		if ( hasHit )
		{
			onGround = true;
			if ( outGroundDistance < -0.1f )
			{
				//transform.position.z -= outGroundDistance;
				//pMainRigidBody->SetPosition( pMainRigidBody->GetPosition() + Vector3d( 0,0,-outGroundDistance ) );
				outGroundDistance = 0;
			}
		}
	}
	// Override the ground check with the contact points
	if ( hitCount < 2 ) { // Todo: This gets skipped if character is sure-footed
		onGround = false;
	}
	bOnGround = onGround;
	// Normalize ground normal
	vMoveLastGroundNormal.normalize();
	return onGround;
}

// ==Common Moving Routines==
// Repeated crouching code. Changes player height and also updates the collision shape of the player.
void	CCharacterMotion::MvtCommonCrouching ( void )
{
	if ( fPlayerHeight < 1.0f )
		fPlayerHeight = m_stats->fStandingHeight;
	if ( m_input->axes.crouch ) //if ( fInput[iCrouch] )
		bWantsCrouch = true;
	else
		bWantsCrouch = false;

	if ( m_input->axes.prone.pressed() ) {
		bWantsProne = !bWantsProne;
		m_input->axes.prone.Skip();
	}
	if ( bWantsCrouch )
		bWantsProne = false;
	/*float groundDistance = 0.0f;
	bool onGround = OnGround( groundDistance );
	if ( !onGround )
		bWantsProne = false;*/

	// Raytrace upwards to check if can uncrouch
	RaycastHit upHitResult;
	Ray ray;
	ray.pos = m_character->transform.position + Vector3d( 0,0,1 );
	ray.dir = Vector3d(0,0,1);
	Raycaster.Raycast( ray, 6.0f, &upHitResult, Physics::GetCollisionFilter(Layers::PHYS_WORLD_TRACE) ); 
	if ( upHitResult.hit )
	{
		if ( upHitResult.distance < (m_stats->fCrouchingHeight-1) )
		{
			bCanUncrouch = false;
			bCanUnprone = false;
		}
		else if ( upHitResult.distance < (m_stats->fStandingHeight-1) )
		{
			bCanUncrouch = false;
			bCanUnprone = true;
		}
		else
		{
			bCanUncrouch = true;
			bCanUnprone = true;
		}
	}
	else
	{
		bCanUncrouch = true;
		bCanUnprone = true;
	}

	/*if (( bWantsCrouch )||( !bCanUncrouch ))
	{
		bIsCrouching = true;
		float deltaHeight = (fCrouchPlayerHeight-fPlayerHeight)*Time::TrainerFactor( 0.4f );
		fPlayerHeight += deltaHeight;
		//transform.position.z += deltaHeight;
		//transform.SetDirty();
		//pMainRigidBody->
		pMainRigidBody->AddToPosition( Vector3d( 0,0,-deltaHeight ) );
	}
	else
	{
		bIsCrouching = false;
		float deltaHeight = (fNormalPlayerHeight-fPlayerHeight)*Time::TrainerFactor( 0.4f );
		fPlayerHeight += deltaHeight;
		//transform.position.z += deltaHeight;
		//transform.SetDirty();
		//pMainRigidBody->AddToPosition( Vector3d( 0,0,deltaHeight ) );
	}
	// Set the height
	pPlayerCollision->SetHeight( fPlayerHeight );*/
	if (( bWantsProne )||( !bCanUnprone ))
	{
		bIsProne = true;
		bIsCrouching = false;
		MvtSetPlayerHeightStick( m_stats->fProneHeight );
	} 
	else if (( bWantsCrouch )||( !bCanUncrouch ))
	{
		bIsProne = false;
		bIsCrouching = true;
		//if ( bOnGround )
			MvtSetPlayerHeightStick( m_stats->fCrouchingHeight );
		//else
		//	MvtSetPlayerHeight( fCrouchPlayerHeight );
	}
	else
	{
		bIsProne = false;
		bIsCrouching = false;
		MvtSetPlayerHeight( m_stats->fStandingHeight );
	}

	// Check if just started crouching
	bStartedCrouching = false;
	if ( bIsCrouching )
		if ( m_input->axes.crouch.PreviousValue < 0.5f )//	if ( fInputPrev[iCrouch] < 0.5f )
			bStartedCrouching = true;
	//

}
// Set player's height. This will do the up and down duck-jump things automatically
void	CCharacterMotion::MvtSetPlayerHeight ( float fnPlayerHeight )
{
	// Check for a default player size value
	if ( fnPlayerHeight <= 0 )
		fnPlayerHeight = m_stats->fStandingHeight;

	// Change the player size
	float deltaHeight;
	//deltaHeight = (fnPlayerHeight-fPlayerHeight)*Time::TrainerFactor( 0.4f );
	deltaHeight = (((fnPlayerHeight-fPlayerHeight)<0) ? -12.0f : 12.0f ) * Time::deltaTime;
	if ( fabs(fnPlayerHeight-fPlayerHeight) < fabs(deltaHeight) )
		deltaHeight = fnPlayerHeight-fPlayerHeight;

	fPlayerHeight += deltaHeight;
	if ( fnPlayerHeight > fPlayerHeight )
	{
		//deltaHeight = (fnPlayerHeight-fPlayerHeight)*Time::TrainerFactor( 0.4f );
		//fPlayerHeight += deltaHeight;
	}
	else if ( fnPlayerHeight < fPlayerHeight )
	{
		//deltaHeight = (fnPlayerHeight-fPlayerHeight)*Time::TrainerFactor( 0.4f );
		//fPlayerHeight += deltaHeight;
		m_rigidbody->AddToPosition( Vector3d( 0,0,-deltaHeight ) );
	}

	// Zero out the foot offset
	float footOffset = m_collider->GetFootOffset();
	deltaHeight = -12.0f*Time::deltaTime;
	if ( fabs(footOffset) < fabs(deltaHeight) ) {
		deltaHeight = -footOffset;
	}
	footOffset += deltaHeight;
	if ( bOnGround ) {
		m_rigidbody->AddToPosition( Vector3d( 0,0,-deltaHeight ) );
	}
	m_collider->SetFootOffset( footOffset );

	// Set the height
	m_collider->SetHeight( fPlayerHeight );
}
// Set player's height. This doesn't do any duck-jump functionality
void	CCharacterMotion::MvtSetPlayerHeightStick ( float fnPlayerHeight )
{
	// Check for a default player size value
	if ( fnPlayerHeight <= 0 ) {
		fnPlayerHeight = m_stats->fStandingHeight;
	}

	// Calculate delta amount
	//float deltaHeight = (fnPlayerHeight-fPlayerHeight)*Time::TrainerFactor( 0.4f );
	float deltaHeight;
	deltaHeight = (((fnPlayerHeight-fPlayerHeight)<0) ? -12.0f : 12.0f ) * Time::deltaTime;

	// Check the ceiling first
	if ( fabs(fnPlayerHeight-fPlayerHeight) > fabs(deltaHeight) )
	{
		if ( deltaHeight > 0 ) {
			RaycastHit upHitResult;
			Ray ray;
			ray.pos = m_character->transform.position + Vector3d( 0,0,fPlayerHeight );
			ray.dir = Vector3d( 0,0,1 );
			if ( Raycaster.Raycast( ray, deltaHeight*2.0f, &upHitResult, Physics::GetCollisionFilter(Layers::PHYS_WORLD_TRACE) ) ) {
				m_rigidbody->AddToPosition( Vector3d( 0,0,-deltaHeight ) );
			}
		}
	}

	// Change the player size
	if ( fabs(fnPlayerHeight-fPlayerHeight) < fabs(deltaHeight) ) {
		deltaHeight = fnPlayerHeight-fPlayerHeight;
	}
	fPlayerHeight += deltaHeight;

	// Zero out the foot offset
	float footOffset = m_collider->GetFootOffset();
	deltaHeight = -12.0f*Time::deltaTime;
	if ( fabs(footOffset) < fabs(deltaHeight) ) {
		deltaHeight = -footOffset;
	}
	footOffset += deltaHeight;
	if ( bOnGround ) {
		m_rigidbody->AddToPosition( Vector3d( 0,0,-deltaHeight ) );
	}
	m_collider->SetFootOffset( footOffset );

	// Set the height
	m_collider->SetHeight( fPlayerHeight );
}

// Get effective hull size
Real CCharacterMotion::MvtGetEffectiveHullRadius ( void )
{
	CCharacterModel* model = m_character->model;
	if ( model != NULL )
	{
		XTransform lookatPos;
		model->GetEyecamTransform( lookatPos );
		Vector2d cameraOffset = Vector2d( model->transform.position - lookatPos.position );
		return cameraOffset.magnitude();
	}
	return 0.0F;
}


void CCharacterMotion::FixedUpdate ( Vector3d* io_turnInput, Vector3d* io_charRotation, Vector3d* io_headRotation )
{
	if ( io_turnInput )		vTurnInput		= *io_turnInput;
	if ( io_charRotation )	vCharRotation	= *io_charRotation;
	if ( io_headRotation )	vHeadRotation	= *io_headRotation;

	// Don't continue if no input
	if ( !m_input ) {
		return;
	}
	// Check if owner is player
	if ( m_charIsPlayer ) {
		m_charIsPlayer = (m_character->GetTypeName() == "CPlayer");
	}

	// Apply queued move type
	if ( m_moveTypeQueued ) {
		m_moveType = m_moveTypeQueued;
		m_moveTypeQueued = NULL;
	}
	// Call the movement type
	(this->*m_moveType)();

	if ( m_rigidbody )
		m_rigidbody->Wake();	// Keep the body awake!

	// Animation motion extrapolation
	{
		// Get model offset as base
		Vector3d modelOffset = m_model->GetAnimationState()->GetExtrapolatedMotion();
		m_model->GetAnimationState()->ResetExtrapolatedMotion();

		// Add linear motions
		if ( m_character->IsAlive() )
		{
			for ( auto it = vLinearMotions.begin(); it != vLinearMotions.end(); )
			{
				it->w -= Time::deltaTime;
				modelOffset += Vector3d( it->x, it->y, it->z ) * Time::deltaTime;
				if ( it->w <= 0 ) {
					it = vLinearMotions.erase(it);
				}
				else {
					++it;
				}
			}
		}

		if ( m_rigidbody ) {
			// Add linear motions to the current position
			m_rigidbody->AddToPosition( m_model->GetModelRotation() * modelOffset );
		}
		else if ( m_character ) {
			m_character->transform.position += m_model->GetModelRotation() * modelOffset;
		}
	}

	if ( io_turnInput )		*io_turnInput	= vTurnInput;
	if ( io_charRotation )	*io_charRotation= vCharRotation;
	if ( io_headRotation )	*io_headRotation= vHeadRotation;
}

void CCharacterMotion::Update ( void )
{
	// Don't continue if no input
	if ( !m_input ) {
		return;
	}

	if ( bMoveUnobstructed ) {
		MoveUnstuck();
	}

	// TODO moveto movetype
	if (( m_canSprint )&&( m_input->axes.sprint )&&( !bIsCrouching || m_stats->bCanCrouchSprint )&&( m_input->vDirInput.magnitude() > 0.5f )) {
		bIsSprinting = true;
	}
	else {
		bIsSprinting = false;
	}

	// TODO
	// clip some of the turning inputs (by turning them to zero)
	vTurnInput.x = std::max<Real>( 0, fabs(vTurnInput.x)-Time::deltaTime*15 ) * Math.sgn<Real>( vTurnInput.x );
	vTurnInput.y = std::max<Real>( 0, fabs(vTurnInput.y)-Time::deltaTime*10 ) * Math.sgn<Real>( vTurnInput.y );
}

// Stops motion and resets the falling damage counter
void CCharacterMotion::StopMotion ( void )
{
	if ( m_rigidbody )
	{
		m_rigidbody->SetVelocity( Vector3d(0,0,0) );
		vFallingVelocity = Vector3d(0,0,0);
	}
}


// Move Unstuck
#include "after/terrain/Zones.h"
#include "after/types/terrain/BlockType.h"
void CCharacterMotion::MoveUnstuck ( void )
{
	return;

	// Only check with OnGround and Falling
	/*if ( (m_moveType != (stateFunc_t)&CMccPlayer::mvt_OnGround) && (m_moveType != (stateFunc_t)&CMccPlayer::mvt_Falling) )
	{
		return;
	}*/

	// Raytrace downward for the check
	Ray			downCheckRayLower ( m_character->transform.position + Vector3d(0,0,0.05f), -Vector3d::up );
	RaycastHit	downCheckHitInfoLower;
	Raycaster.Raycast( downCheckRayLower, 2.0f, &downCheckHitInfoLower,  Physics::GetCollisionFilter(Layers::PHYS_WORLD_TRACE) );
	Ray			downCheckRayUpper ( m_character->transform.position + Vector3d(0,0,2.05f), -Vector3d::up );
	RaycastHit	downCheckHitInfoUpper;
	Raycaster.Raycast( downCheckRayUpper, 4.0f, &downCheckHitInfoUpper,  Physics::GetCollisionFilter(Layers::PHYS_WORLD_TRACE) );

	Vector3d samplePositionUpper = m_character->transform.position + Vector3d(0,0,0.05f);
	Terrain::terra_b blockUpper = Zones.GetBlockAtPosition(samplePositionUpper);
	Vector3d samplePositionLower = m_character->transform.position + Vector3d(0,0,-1.95f);
	Terrain::terra_b blockLower = Zones.GetBlockAtPosition(samplePositionLower);

	if (( blockLower.block == Terrain::EB_NONE )||( blockLower.block == Terrain::EB_WATER ))
	{

	}
	else
	{
		// Calculate deltaZ to the ground.
		float deltaZ = -0.1f;
		if ( !downCheckHitInfoLower.hit && downCheckHitInfoUpper.hit )
		{
			if (( blockUpper.block == Terrain::EB_NONE )||( blockUpper.block == Terrain::EB_WATER ))
			{
				// Guess the current Z position
				float deltaZ = (floor(samplePositionLower.z/2)*2.0f) + (2*Terrain::_depth_bias(blockLower.normal_z_w)) + 0.05f;
				// Target delta = Target - Current
				deltaZ = deltaZ - m_character->transform.position.z;
			}
			else
			{	// In solid block, move up
				deltaZ = 2;
			}
		}
		else if ( !downCheckHitInfoLower.hit && !downCheckHitInfoUpper.hit )
		{
			// Both traces missed
			if (( blockUpper.block == Terrain::EB_NONE )||( blockUpper.block == Terrain::EB_WATER ))
			{
				// Nothing
			}
			{	// In solid block, move up
				deltaZ = 2;
			}
		}
		// If positive movement, move the player upwards
		if ( deltaZ > 0.0 )
		{
			m_character->transform.position.z += deltaZ;

			if ( m_rigidbody ) { // Set dirty body and slow movement
				m_character->transform.SetDirty();
				m_rigidbody->SetVelocity( m_rigidbody->GetVelocity() * 0.5f );
			}
		}
	}
}


#undef quickReturn
#undef func
