/*
// Includes
#include "CPlayer.h"
#include "CInputControl.h"

// Needed misc includes
#include "Time.h"
#include "Raytracer.h"
#include "Water.h"

#include "CRigidbodyCharacter.h"

// Define
#ifndef mvtQuickReturn
#define mvtQuickReturn(a) (updateMoveType( (stateFunc_t)&CPlayer::a ))
#endif

// Movement initialization. Initializes the rigidbody, collider, and floor triggers.
//  Called in the constructor of CPlayer.
void	CPlayer::InitMovement ( void )
{
	//pPlayerCollision = new CCylinderCollider ( fPlayerHeight, fPlayerRadius );
	pPlayerCollision = new CCapsuleCollider ( fPlayerHeight, fPlayerRadius );
	//pMainRigidBody = new CRigidbody ( pPlayerCollision, this, 30 ); //weight should be 30
	pMainRigidBody = new CRigidBodyCharacter ( pPlayerCollision, this, 30 ); //weight should be 30
	pMainRigidBody->SetRotationEnabled( false );
	pMainRigidBody->SetQualityType( HK_COLLIDABLE_QUALITY_CHARACTER );
	//pMainRigidBody->SetQualityType( HK_COLLIDABLE_QUALITY_CRITICAL ); //causes weird bouncing effect
	pMainRigidBody->SetRestitution( 0.01f );
	pMainRigidBody->SetCollisionLayer( Layers::PHYS_SWEPTCOLLISION, 2 );
}
// Cleanup for the movement.
//  Called in the destructor of CPlayer
void	CPlayer::FreeMovement ( void )
{
	delete pMainRigidBody;
	delete pPlayerCollision;
}
*/
//// ==Common Moving Routines==
//// Repeated crouching code. Changes player height and also updates the collision shape of the player.
//void	CPlayer::MvtCommonCrouching ( void )
//{
//	if ( fPlayerHeight < 1.0f )
//		fPlayerHeight = fNormalPlayerHeight;
//	if ( input->axes.crouch ) //if ( fInput[iCrouch] )
//		bWantsCrouch = true;
//	else
//		bWantsCrouch = false;
//
//	if ( input->axes.prone.pressed() ) {
//		bWantsProne = !bWantsProne;
//		input->axes.prone.Skip();
//	}
//	if ( bWantsCrouch )
//		bWantsProne = false;
//	/*float groundDistance = 0.0f;
//	bool onGround = OnGround( groundDistance );
//	if ( !onGround )
//		bWantsProne = false;*/
//
//	// Raytrace upwards to check if can uncrouch
//	RaycastHit upHitResult;
//	Ray ray;
//	ray.pos = transform.position + Vector3d( 0,0,1 );
//	ray.dir = Vector3d(0,0,1);
//	Raytracer.Raycast( ray, 6.0f, &upHitResult, Physics::GetCollisionFilter(Layers::PHYS_WORLD_TRACE) ); 
//	if ( upHitResult.hit )
//	{
//		if ( upHitResult.distance < (fCrouchPlayerHeight-1) )
//		{
//			bCanUncrouch = false;
//			bCanUnprone = false;
//		}
//		else if ( upHitResult.distance < (fNormalPlayerHeight-1) )
//		{
//			bCanUncrouch = false;
//			bCanUnprone = true;
//		}
//		else
//		{
//			bCanUncrouch = true;
//			bCanUnprone = true;
//		}
//	}
//	else
//	{
//		bCanUncrouch = true;
//		bCanUnprone = true;
//	}
//
//	/*if (( bWantsCrouch )||( !bCanUncrouch ))
//	{
//		bIsCrouching = true;
//		float deltaHeight = (fCrouchPlayerHeight-fPlayerHeight)*Time::TrainerFactor( 0.4f );
//		fPlayerHeight += deltaHeight;
//		//transform.position.z += deltaHeight;
//		//transform.SetDirty();
//		//pMainRigidBody->
//		pMainRigidBody->AddToPosition( Vector3d( 0,0,-deltaHeight ) );
//	}
//	else
//	{
//		bIsCrouching = false;
//		float deltaHeight = (fNormalPlayerHeight-fPlayerHeight)*Time::TrainerFactor( 0.4f );
//		fPlayerHeight += deltaHeight;
//		//transform.position.z += deltaHeight;
//		//transform.SetDirty();
//		//pMainRigidBody->AddToPosition( Vector3d( 0,0,deltaHeight ) );
//	}
//	// Set the height
//	pPlayerCollision->SetHeight( fPlayerHeight );*/
//	if (( bWantsProne )||( !bCanUnprone ))
//	{
//		bIsProne = true;
//		bIsCrouching = false;
//		MvtSetPlayerHeightStick( fPronePlayerHeight );
//	} 
//	else if (( bWantsCrouch )||( !bCanUncrouch ))
//	{
//		bIsProne = false;
//		bIsCrouching = true;
//		//if ( bOnGround )
//			MvtSetPlayerHeightStick( fCrouchPlayerHeight );
//		//else
//		//	MvtSetPlayerHeight( fCrouchPlayerHeight );
//	}
//	else
//	{
//		bIsProne = false;
//		bIsCrouching = false;
//		MvtSetPlayerHeight( fNormalPlayerHeight );
//	}
//
//	// Check if just started crouching
//	bStartedCrouching = false;
//	if ( bIsCrouching )
//		if ( input->axes.crouch.PreviousValue < 0.5f )//	if ( fInputPrev[iCrouch] < 0.5f )
//			bStartedCrouching = true;
//	//
//
//}
//// Set player's height. This will do the up and down duck-jump things automatically
//void	CPlayer::MvtSetPlayerHeight ( float fnPlayerHeight )
//{
//	// Check for a default player size value
//	if ( fnPlayerHeight <= 0 )
//		fnPlayerHeight = fNormalPlayerHeight;
//
//	// Change the player size
//	float deltaHeight;
//	//deltaHeight = (fnPlayerHeight-fPlayerHeight)*Time::TrainerFactor( 0.4f );
//	deltaHeight = (((fnPlayerHeight-fPlayerHeight)<0) ? -12.0f : 12.0f ) * Time::deltaTime;
//	if ( fabs(fnPlayerHeight-fPlayerHeight) < fabs(deltaHeight) )
//		deltaHeight = fnPlayerHeight-fPlayerHeight;
//
//	fPlayerHeight += deltaHeight;
//	if ( fnPlayerHeight > fPlayerHeight )
//	{
//		//deltaHeight = (fnPlayerHeight-fPlayerHeight)*Time::TrainerFactor( 0.4f );
//		//fPlayerHeight += deltaHeight;
//	}
//	else if ( fnPlayerHeight < fPlayerHeight )
//	{
//		//deltaHeight = (fnPlayerHeight-fPlayerHeight)*Time::TrainerFactor( 0.4f );
//		//fPlayerHeight += deltaHeight;
//		pMainRigidBody->AddToPosition( Vector3d( 0,0,-deltaHeight ) );
//	}
//	// Set the height
//	pPlayerCollision->SetHeight( fPlayerHeight );
//}
//// Set player's height. This doesn't do any duck-jump functionality
//void	CPlayer::MvtSetPlayerHeightStick ( float fnPlayerHeight )
//{
//	// Check for a default player size value
//	if ( fnPlayerHeight <= 0 ) {
//		fnPlayerHeight = fNormalPlayerHeight;
//	}
//
//	// Calculate delta amount
//	//float deltaHeight = (fnPlayerHeight-fPlayerHeight)*Time::TrainerFactor( 0.4f );
//	float deltaHeight;
//	deltaHeight = (((fnPlayerHeight-fPlayerHeight)<0) ? -12.0f : 12.0f ) * Time::deltaTime;
//
//	// Check the ceiling first
//	if ( fabs(fnPlayerHeight-fPlayerHeight) > fabs(deltaHeight) )
//	{
//		if ( deltaHeight > 0 ) {
//			RaycastHit upHitResult;
//			Ray ray;
//			ray.pos = transform.position + Vector3d( 0,0,fPlayerHeight );
//			ray.dir = Vector3d( 0,0,1 );
//			if ( Raytracer.Raycast( ray, deltaHeight*2.0f, &upHitResult, Physics::GetCollisionFilter(Layers::PHYS_WORLD_TRACE) ) ) {
//				pMainRigidBody->AddToPosition( Vector3d( 0,0,-deltaHeight ) );
//			}
//		}
//	}
//
//	// Change the player size
//	if ( fabs(fnPlayerHeight-fPlayerHeight) < fabs(deltaHeight) ) {
//		deltaHeight = fnPlayerHeight-fPlayerHeight;
//	}
//	fPlayerHeight += deltaHeight;
//
//	// Set the height
//	pPlayerCollision->SetHeight( fPlayerHeight );
//}

// ==MOVING==
// Change the move state
/*void*	CPlayer::updateMoveType ( stateFunc_t pNewMoveType )
{
	m_moveType = pNewMoveType;
	return NULL;
}*/
// Check if there's ground below
/*bool	CPlayer::OnGround ( Real& outGroundDistance )
{
	bool onGround = false;
	bool hasHit = false;
	const Real maxStepHeight = 1.8f;
	outGroundDistance = -10;
	RaycastHit hitResult;
	//const static Real playerWidth = 1.5f;
	// First check for the ground
	Ray ray;
	ray.dir = Vector3d(0,0,-1);
	for ( short i = 0; i < 5; i += 1 )
	{
		switch ( i )
		{
		case 0:	ray.pos = transform.position+Vector3d(0,0,maxStepHeight);
			break;
		case 1: ray.pos = transform.position+Vector3d(fPlayerRadius*0.95f,0,maxStepHeight);
			break;
		case 2: ray.pos = transform.position+Vector3d(-fPlayerRadius*0.95f,0,maxStepHeight);
			break;
		case 3: ray.pos = transform.position+Vector3d(0,fPlayerRadius*0.95f,maxStepHeight);
			break;
		case 4: ray.pos = transform.position+Vector3d(0,-fPlayerRadius*0.95f,maxStepHeight);
			break;
		}

		// if hit
		if ( Raytracer.Raycast( ray, 4.0f, &hitResult, Physics::GetCollisionFilter(Layers::PHYS_WORLD_TRACE) ) )
		{
			// then we hit
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
	bOnGround = onGround;
	// Normalize ground normal
	vMoveLastGroundNormal.normalize();
	//
	return onGround;
}*/
// Attempt to move to a position
/*bool CPlayer::AttemptMoveToPosition ( Vector3d targetPos )
{
	const static Real playerWidth = 1.5f;
	const static Real halfPlayerWidth = 0.75f;

	Vector3d vDist = targetPos-transform.position;
	Real maxDist = vDist.magnitude();
	if ( maxDist <= 0 )
		return true;
	
	Ray moveRay;
	moveRay.dir = vDist.normal();
	Vector3d offsetDir ( moveRay.dir.y, moveRay.dir.x, 0 );
	offsetDir = offsetDir.normal();

	Real currentDist = maxDist;

	RaycastHit hitResult;

	for ( short i = 0; i < (int)(fPlayerHeight-0.1f)*3; i += 1 )
	{
		moveRay.pos = transform.position+Vector3d( 0,0,0.1f+Real(i/3) )-(offsetDir*Real(1-i%3)*halfPlayerWidth);
		if ( Raytracer.Raycast( moveRay, maxDist, &hitResult,NULL, 1|2|4 ) )
		{
			if (( hitResult.distance > 0 )&&( hitResult.distance < currentDist ))
			{
				currentDist = hitResult.distance;
			}
		}
	}

	//transform.position += moveRay.dir * (currentDist*0.99f);
	transform.position += moveRay.dir * (currentDist - 0.00001f );

	if ( currentDist > 0.00001f )
		return true;
	return false;
}*/
// ===MOVEMENT STATES===

// == Player Base Movement ==
// == this is falling, walking, sliding, rolling, climbing ==
// Player falling
/*void*	CPlayer::mvtFalling ( void )
{
	MvtCommonCrouching();

	// Turn first
	vPlayerRotation.z += vTurnInput.x;
	vTurnInput.x = 0.0f;

	// In Water
	if ( Water.PositionInside( transform.position+Vector3d( 0,0,2.5f ) ) )
	{
		return mvtQuickReturn( mvtSwimmingInWater );
	}

	float groundDistance = 0.0f;
	bool onGround = OnGround( groundDistance );
	if ( onGround )
	{
		// Punch the camera based on the vertical speed
		//PunchView( Vector3d( 0,vMoveVelocity.sqrMagnitude()*0.002f*8.0f,0 ) );
		float punchAmt = sqr(vMoveVelocity.z);
		PunchView( Vector3d( random_range(-0.0015f,0.0015f)*punchAmt,-punchAmt*0.0030f,random_range(-0.0010f,0.0010f)*punchAmt ) );

		// Check for fall damage and all that
		if ( punchAmt*0.0060f > 3.4f )
		{
			Damage hurt;
			hurt.type |= DamageType::Fall;
			hurt.amount = sqr(punchAmt*0.0035f);
			OnDamaged( hurt );
		}

		// Return ground speed
		return mvtQuickReturn( mvtWalkingOnGround );
	}

	Vector3d moveVector(0,0,0);
	moveVector = vDirInput;
	moveVector.z = 0;

	// Rotate the move vector to match the camera
	Matrix4x4 rotMatrix;
	rotMatrix.setRotation( vPlayerRotation );
	moveVector = rotMatrix*moveVector;

	vMoveVelocity = pMainRigidBody->GetVelocity();

	// Limit the horizontal acceleration
	Vector3d testCase = Vector3d( vMoveVelocity.x, vMoveVelocity.y, 0 );
	if ( testCase.magnitude() <= fRunSpeed*1.5f )
	{
		vMoveVelocity += moveVector * Time::deltaTime * fAirAccelBase;
	}

	// Autojump
	if (( bAutojumpEnabled )&&( !bIsCrouching )&&( bIsSprinting )&&( vMoveVelocity.z < 0 ))
	{
		// Raytrace forward
		RaycastHit hitResult;
		Ray ray;
		ray.pos = transform.position + Vector3d( 0,0,1 );
		ray.dir = testCase.normal();
		if ( Raytracer.Raycast( ray, 3.0f, &hitResult, 1|2|4 ) )
		{
			ray.pos.z += 2;
			if ( !Raytracer.Raycast( ray, 5.0f, &hitResult, 1|2|4 ) )
			{
				vMoveVelocity.z = 10.4f;
			}
		}
	}

	// Set new velocity
	pMainRigidBody->SetVelocity( vMoveVelocity );

	// View shake!
	camViewBobSetSpeed( vMoveVelocity.sqrMagnitude() * 0.05f * ( (vMoveVelocity.z<0) ? 2.0f : 1.0f )  );
	camViewBobSetTargetSize( vMoveVelocity.sqrMagnitude()*0.002f );

	// FOV widen at high speed!
	camViewAngleSetPOffset( vMoveVelocity.sqrMagnitude()*0.004f );


	return mvtQuickReturn( mvtFalling );
}
// Player walkin
void*	CPlayer::mvtWalkingOnGround ( void )
{
	MvtCommonCrouching();
	camViewAngleSetPOffset( 0 );
	camRollSetOffset( 0 );

	float groundDistance = 0.0f;
	bool onGround = OnGround( groundDistance );
	//bool onGround = true;
	if ( !onGround )
	{
		//transform.position.z += groundDistance;
		return mvtQuickReturn( mvtFalling );
	}
	else
	{
		// Turn first
		vPlayerRotation.z += vTurnInput.x;
		vTurnInput.x = 0.0f;

		// In Water
		if ( Water.PositionInside( transform.position+Vector3d( 0,0,2.5f ) ) )
		{
			return mvtQuickReturn( mvtSwimmingInWater );
		}

		Vector3d moveVector(0,0,0);
		moveVector = vDirInput;
		moveVector.z = 0;

		// Rotate the move vector to match the camera
		Matrix4x4 rotMatrix;
		rotMatrix.setRotation( vPlayerRotation );
		moveVector = rotMatrix*moveVector;

		// Slow down movement
		if ( moveVector.sqrMagnitude() < 0.1f )
			vMoveVelocity += (Vector3d(0,0,0) - vMoveVelocity) * Time::TrainerFactor( 0.5f );
		// Add to move velocity
		else
			vMoveVelocity += moveVector * Time::deltaTime * fGroundAccelBase;

		// Get the move speed limit
		Real limit;
		if ( bIsCrouching )
			limit = fCrouchSpeed;
		else
			limit = fRunSpeed;
		if ( bIsSprinting )
			limit *= fSprintSpeed/fRunSpeed;
		
		limit *= 0.7f;
		// Limit the xy movement while moving
		Vector3d testCase = Vector3d( vMoveVelocity.x, vMoveVelocity.y, 0 );
		if ( testCase.magnitude() > limit )
		{
			testCase = testCase.normal() * limit;
			vMoveVelocity.x = testCase.x;
			vMoveVelocity.y = testCase.y;
		}

		// Vertical movement
		vMoveVelocity.z = pMainRigidBody->GetVelocity().z;
		if ( input->axes.jump ) //if ( fInput[iJump] )
			vMoveVelocity.z = 12.0f;

		// Autojump
		if (( bAutojumpEnabled )&&( !bIsCrouching )&&( bIsSprinting ))
		{
			// Raytrace forward
			RaycastHit hitResult;
			Ray ray;
			ray.pos = transform.position + Vector3d( 0,0,1 );
			ray.dir = testCase.normal();
			if ( Raytracer.Raycast( ray, 3.0f, &hitResult, 1|2|4 ) )
			{
				ray.pos.z += 2;
				if ( !Raytracer.Raycast( ray, 5.0f, &hitResult, 1|2|4 ) )
				{
					vMoveVelocity.z = 11.5f;
				}
			}
		}

		// Combat slide
		if ( fSlideCounter > 0.01f )
		{
			fSlideCounter -= Time::deltaTime;
		}
		else
		{
			fSlideCounter = 0.0f;
			if ( bIsSprinting && bStartedCrouching )
			{
				//if ( testCase.magnitude() > fSprintSpeed*0.6f )
				if ( pMainRigidBody->GetVelocity().magnitude() > fSprintSpeed*0.67f )
					return mvtQuickReturn( mvtCombatSlide );
			}
		}

		// Set rigidbody's velocity
		pMainRigidBody->SetVelocity( vMoveVelocity );

		// View bob!
		camViewBobSetSpeed( 22.0f + testCase.magnitude()/fSprintSpeed + ( bIsSprinting ? 6.0f : 0.0f )  );
		camViewBobSetTargetSize( testCase.sqrMagnitude()*0.006f );
	}

	return mvtQuickReturn( mvtWalkingOnGround );
}
// Player Swimming
void*	CPlayer::mvtSwimmingInWater ( void )
{
	
	camViewAngleSetPOffset( 0 );
	camRollSetOffset( 0 );

	// In Water
	if ( !Water.PositionInside( transform.position+Vector3d( 0,0,2.5f ) ) )
	{
		// Set the gravity to true
		pMainRigidBody->SetGravity( true );

		return mvtQuickReturn( mvtWalkingOnGround );
	}
	else
	{
		// Set the gravity to nil
		pMainRigidBody->SetGravity( false );

		// Set player height to crouch height on default, and prone if ducking
		if ( input->axes.crouch ) //if ( fInput[iCrouch] )
			MvtSetPlayerHeightStick( fPronePlayerHeight );
		else
			MvtSetPlayerHeightStick( fCrouchPlayerHeight );

		// Turn first
		vPlayerRotation.z += vTurnInput.x;
		vTurnInput.x = 0.0f;

		Vector3d moveVector(0,0,0);
		moveVector = vDirInput;
		moveVector.z = 0;

		// Rotate the move vector to match the camera
		Matrix4x4 rotMatrix;
		//rotMatrix.setRotation( transform.rotation );
		//rotMatrix.setRotation( pCamera->transform.rotation );
		rotMatrix.setRotation( Vector3d( 0,vCameraRotation.y,vPlayerRotation.z ) );
		moveVector = rotMatrix*moveVector;

		// Slow down movement
		if ( moveVector.sqrMagnitude() < 0.1f )
			vMoveVelocity += (Vector3d(0,0,-1.0f) - vMoveVelocity) * Time::TrainerFactor( 0.2f );
		// Add to move velocity
		else
			vMoveVelocity += moveVector * Time::deltaTime * fWaterAccelBase;

		// Vertical movement
		//vMoveVelocity.z += pMainRigidBody->GetVelocity().z;
		if ( input->axes.jump ) //if ( fInput[iJump] )
			vMoveVelocity.z = 12.0f;

		// Get the move speed limit
		Real limit;
		//if ( bIsCrouching )
			//limit = fRunSpeed * fCrouchSpeed/fRunSpeed;
		//else
			//limit = fRunSpeed;
		limit = fSwimSpeed;
		if ( bIsSprinting )
			limit *= fSprintSpeed/fRunSpeed;
		
		limit *= 0.7f;
		// Limit the xyz movement while moving
		Vector3d testCase = Vector3d( vMoveVelocity.x, vMoveVelocity.y, vMoveVelocity.z );
		if ( testCase.magnitude() > limit )
		{
			testCase = testCase.normal() * limit;
			vMoveVelocity.x = testCase.x;
			vMoveVelocity.y = testCase.y;
			vMoveVelocity.z = testCase.z;
		}

		// Set rigidbody's velocity
		pMainRigidBody->SetVelocity( vMoveVelocity );

		// View bob!
		//camViewBobSetSpeed( 22.0f + testCase.magnitude()/fSprintSpeed + ( bIsSprinting ? 6.0f : 0.0f )  );
		camViewBobSetSpeed( (22.0f + testCase.magnitude()/fSprintSpeed + ( bIsSprinting ? 6.0f : 0.0f ))*0.6f  );
		camViewBobSetTargetSize( testCase.sqrMagnitude()*0.006f );

	}

	return mvtQuickReturn( mvtSwimmingInWater );
}


// Player Sliding
void*	CPlayer::mvtCombatSlide ( void )
{
	
	// Get the move vector from the current direction
	Vector3d moveVector(0,0,0);
	moveVector = pMainRigidBody->GetVelocity();

	Vector3d testCase = moveVector;
	testCase.z = 0;

	// Check for if we hit a wall in front of us
	// Todo: move to a separate function
	bool hitWall = false;
	Ray ray;
	ray.dir = testCase.normal();
	ray.pos = transform.position + Vector3d( 0,0,1 );
	RaycastHit result;
	if ( Raytracer.Raycast( ray, 4.0f, &result, 1|2|4 ) )
	{
		if ( result.distance < 2.0f )
		{
			hitWall = true;
		}
	}

	// Increment slide counter
	fSlideCounter += Time::deltaTime;
	// If we hit a wall or the end of the slide time
	if (( fSlideCounter >= fSlideTime )||( hitWall ))
	{
		//fSlideCounter = 0.0f;
		return mvtQuickReturn( mvtWalkingOnGround );
	}
	// Do actual slide movement

	// Override crouch input
	//fInput[iCrouch] = 1.0f;
	//MvtCommonCrouching();
	MvtSetPlayerHeightStick( (fPronePlayerHeight+fCrouchPlayerHeight)*0.5f ); // Set player height rather low

	// Turn first
	vPlayerRotation.z += vTurnInput.x*0.1f; //but at a slower rate
	vTurnInput.x = 0.0f;

	// Offset the turning to the current facing direction
	{
		moveVector = vDirInput;
		moveVector.z = 0;
		// Rotate the move vector to match the camera
		Matrix4x4 rotMatrix;
		rotMatrix.setRotation( vPlayerRotation );
		moveVector = rotMatrix*moveVector;
		// Add the move vector to the test case
		testCase.x += moveVector.x;
		testCase.y += moveVector.y;
	}
	// Set the movespeed to constant sprintspeed
	testCase = testCase.normal() * fSprintSpeed * 0.9f;

	// Set the move vector at a constant value
	moveVector = testCase;

	// Set the horizontal velocity
	vMoveVelocity = moveVector;
	// Set vertical velocity
	vMoveVelocity.z = pMainRigidBody->GetVelocity().z;

	// Set rigidbody's velocity
	pMainRigidBody->SetVelocity( vMoveVelocity );
	// Reduce view bob to mostly nil, but very fast
	//camViewBobSetSpeed( 34.0f + testCase.magnitude()/fSprintSpeed );
	camViewBobSetSpeed( 76.0f + testCase.magnitude()/fSprintSpeed*10.0f );
	camViewBobSetTargetSize( testCase.sqrMagnitude()*0.002f );
	camViewAngleSetPOffset( 7 );
	camRollSetOffset( -7 );

	return mvtQuickReturn( mvtCombatSlide );
}*/
	/*
// == Phase Movetype ==
// Phase flying. For the win.
void*	CPlayer::mvtPhaseFlying ( void )
{
	//pMainRigidBody->SetMotionType( physMotion::MOTION_KEYFRAMED );
	//pMainRigidBody->SetGravity( false );

	// Turn first
	vPlayerRotation.z += vTurnInput.x;
	vTurnInput.x = 0.0f;

	Vector3d moveVector(0,0,0);
	moveVector = vDirInput;
	moveVector.z = 0;

	// Rotate the move vector to match the camera
	Matrix4x4 rotMatrix;
	//cout << pCamera->transform.rotation.getEulerAngles() << endl;
	rotMatrix.setRotation( pCamera->transform.rotation );
	moveVector = rotMatrix*moveVector;

	if ( input->axes.jump ) {
		moveVector.z += 1;
	}
	if ( input->axes.crouch ) {
		moveVector.z -= 1;
	}

	if ( input->axes.sprint ) { //if ( fInput[iSprint] )
		transform.position += moveVector * Time::deltaTime * 30.0f;
	}
	else {
		transform.position += moveVector * Time::deltaTime * 6.0f;
	}

	//pMainRigidBody->SetPosition( transform.position );

	//pMainRigidBody->SetVelocity( Vector3d::zero );

	//cout << transform.position << endl;

	//return mvtQuickReturn( mvtPhaseFlying );
	return NULL;
}*/