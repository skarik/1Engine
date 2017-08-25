/*
// Includes
#include "CPlayer.h"

// Needed misc includes
#include "Time.h"
#include "Math.h"

#define camQuickReturn(a) (updateCameraState( (stateFunc_t)&CPlayer::a ))

// ==Common Setters (Camera Related)==
// Sets the view angle. Normally, this is something like the FOV (field of vision) of a character.
void CPlayer::SetViewAngle ( float fnAngle )
{
	fViewAngle = fnAngle;
}
// Sets the offset for the view angle. This is the function you call when you want to temporarily change the view angle of a character
// as when, for example, you want to zoom in with a Sniper Rifle.
void CPlayer::SetViewAngleOffset ( float fnAngle )
{
	fViewAngleOffset = fnAngle;
}

// Sets the turn rate sensitivity. You don't need to do this when zooming in. That turn rate decrease is taken care of automatically.
void CPlayer::SetTurnSensitivity ( float fnSensitivity )
{
	fTurnSensitivity = fnSensitivity;
}

// ==View Punching==
void	CPlayer::PunchView ( Vector3d const&	vPunchValue )
{
	vViewPunch += vPunchValue;
	vViewPunch.x = Math.Clamp( vViewPunch.x, -73, 73 );
	vViewPunch.y = Math.Clamp( vViewPunch.y, -52, 52 );
	vViewPunch.z = Math.Clamp( vViewPunch.z, -52, 52 );
}

// ==Camera Updating==
// Change the camera state
void*	CPlayer::updateCameraState ( stateFunc_t pNewCameraState )
{
	m_cameraUpdateType = pNewCameraState;
	return NULL;
}

// ==View Bob Setters==
// Setters of the view bob. Separates the movement and view bob just enough.
void	CPlayer::camViewBobSetSpeed ( float fnSpeed )
{
	fViewBobSpeed = fabs(fnSpeed) / float(PI);
}
void	CPlayer::camViewBobSetTargetSize ( float fnTarget )
{
	fViewBobTarget = fnTarget*0.5f;
}

// View angle setters
void	CPlayer::camViewAngleSetPOffset ( float fnTarget )
{
	fViewAnglePTarget = fnTarget;
}
// View roll
void	CPlayer::camRollSetOffset ( float fnTarget )
{
	fViewRollTarget = fnTarget;
}

#include "CCharacterModel.h"

// ==Camera States==
// Default camera update state
void*	CPlayer::camDefault ( void )
{
	float fPlayerHeight = 5.5f;
	// Set the position of the view
	if ( model )
	{
		XTransform camtrans;
		model->GetEyecamTransform( camtrans );
		pCamera->transform.position = camtrans.position;
		pCamera->transform.position.z = ( (pCamera->transform.position.z)*0.6f + (transform.position.z + fPlayerHeight-0.6f)*0.4f ); // Stabilize vertical height of camera a little bit more
	}
	else
	{
		pCamera->transform.position = transform.position + Vector3d( 0,0,fPlayerHeight-0.6f );
	}
	// Do mouse look
	vCameraRotation.z = vPlayerRotation.z;
	vCameraRotation.y -= vTurnInput.y;
	vTurnInput.y = 0;

	// Limit vertical mouselook angle
	if ( vCameraRotation.y > 95 )
		vCameraRotation.y = 95;
	if ( vCameraRotation.y < -95 )
		vCameraRotation.y = -95;

	// Perform view bob calculations
	fViewBobTimer += Time::deltaTime*fViewBobSpeed;
	if ( fViewBobTimer > PI*2 )
		fViewBobTimer -= (float)(PI)*2;
	fViewBobAmount += (fViewBobTarget-fViewBobAmount)*Time::TrainerFactor( 50 );
	vViewBob.x = (float)cosf( fViewBobTimer )*fViewBobAmount;	//Roll
	vViewBob.z = (float)sinf( fViewBobTimer )*-fViewBobAmount;	//Yaw
	vViewBob.y = (float)cosf( fViewBobTimer*2 )*-fViewBobAmount; //Pitch

	// Perform view punch calculations
	// The following gives an interesting underdamped situation. This may be useful for vehicle collision.
	//vViewPunchVelocity += ( -vViewPunch*0.2f - vViewPunchVelocity*0.04f )*Time::TrainerFactor( 50 );
	//vViewPunch += ( vViewPunchVelocity - vViewPunch*0.02f )*Time::TrainerFactor( 50 );

	vViewPunchVelocity += ( -vViewPunch*22.5f - vViewPunchVelocity*0.6f )*Time::deltaTime;
	vViewPunch += ( vViewPunchVelocity - vViewPunch*3.6f )*Time::deltaTime;

	// Perform view angle calculations
	fViewAnglePOffset += ( fViewAnglePTarget-fViewAnglePOffset )*Time::TrainerFactor( 1 );
	// Perform view roll calculations
	fViewRollOffset += ( fViewRollTarget-fViewRollOffset )*Time::TrainerFactor( 0.3f );
	vCameraRotation.x = fViewRollOffset;

	// Set the final camera rotation and view angle
	//Vector3d fin = vCameraRotation+vViewBob+vViewPunch+vCameraAnimOffsetRotation;
	pCamera->transform.rotation.Euler( vCameraRotation+vViewBob+vViewPunch+vCameraAnimOffsetRotation );
	//pCamera->transform.rotation = !pCamera->transform.rotation;
	//pCamera->SetRotation( vCameraRotation+vViewBob+vViewPunch+vCameraAnimOffsetRotation );
	pCamera->fov = fViewAngle + fViewAngleOffset + fViewAnglePOffset;
	if ( pCamera->fov > 150 )
		pCamera->fov = 150;

	return camQuickReturn( camDefault );
}*/