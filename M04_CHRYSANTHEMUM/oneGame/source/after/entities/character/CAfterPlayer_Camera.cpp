
// Includes
#include "core/time/time.h"
#include "core/math/Math.h"
#include "core/input/CInput.h"
#include "core-ext/input/CInputControl.h"
#include "after/entities/character/CAfterPlayer.h"
#include "after/entities/CCharacterModel.h"
#include "after/interfaces/CCharacterMotion.h"

// Quick return macro
#ifndef camQuickReturn
#define camQuickReturn(a) (updateCameraState( (stateFunc_t)&CAfterPlayer::a ))
#endif
// Change the camera state
void*	CAfterPlayer::updateCameraState ( stateFunc_t pNewCameraState )
{
	m_cameraUpdateType = pNewCameraState;
	return NULL;
}

//=========================================//
//	State Sets
//=========================================//
void CAfterPlayer::SetCameraMode ( const NPC::eCameratypeEnumeration mode )
{
	stateFunc_t oldCamUpdate = m_cameraUpdateType;
	switch ( mode ) {
		case NPC::CAMERA_SETTING_ANIMLOCK_OFF:	bUseAnimCamMode = false;
			break;
		case NPC::CAMERA_SETTING_ANIMLOCK_ON:	bUseAnimCamMode = true;
			break;

		case NPC::CAMERA_DEFAULT: m_cameraUpdateType	= (stateFunc_t)&CAfterPlayer::cam_Default;
			break;
		case NPC::CAMERA_OVERVIEW: m_cameraUpdateType	= (stateFunc_t)&CAfterPlayer::cam_Inventory;
			break;
		case NPC::CAMERA_THIRDPERSON: m_cameraUpdateType	= (stateFunc_t)&CAfterPlayer::cam_ThirdPerson;
			break;
		case NPC::CAMERA_MONTAGE: m_cameraUpdateType	= (stateFunc_t)&CAfterPlayer::cam_BeautyMontage;
			break;
		case NPC::CAMERA_MATCH:
		default:
			if ( bUseThirdPersonMode ) {
				m_cameraUpdateType	= (stateFunc_t)&CAfterPlayer::cam_ThirdPerson;
			}
			else if ( bUseBeautyCamMode ) {
				m_cameraUpdateType	= (stateFunc_t)&CAfterPlayer::cam_Beauty;
			}
			else if ( bUseBeautyMontage ) {
				m_cameraUpdateType	= (stateFunc_t)&CAfterPlayer::cam_BeautyMontage;
			}
			else {
				m_cameraUpdateType	= (stateFunc_t)&CAfterPlayer::cam_Default;
			}
			break;
	};
	if ( oldCamUpdate != m_cameraUpdateType ) {
		fCamSwapBlend = 0;
		bCamSwapFirstframe = true;
		vCameraRotationPrev = vCameraRotation;
		vCameraPositionPrev = pCamera->transform.position;
	}
}

//=========================================//
// External State Modifiers
//=========================================//

// Sets the view angle. Normally, this is something like the FOV (field of vision) of a character.
void CAfterPlayer::SetViewAngle ( float fnAngle )
{
	fViewAngle = fnAngle;
}

// Sets the offset for the view angle. This is the function you call when you want to temporarily change the view angle of a character
// as when, for example, you want to zoom in with a Sniper Rifle.
void CAfterPlayer::SetViewAngleOffset ( float fnAngle )
{
	fViewAngleOffset = fnAngle;
}

// Sets the turn rate sensitivity. You don't need to do this when zooming in. That turn rate decrease is taken care of automatically.
void CAfterPlayer::SetTurnSensitivity ( float fnSensitivity )
{
	fTurnSensitivity = fnSensitivity;
}

// Punches the view
void CAfterPlayer::PunchView ( Vector3d const&	vPunchValue )
{
	vViewPunch += vPunchValue;
	vViewPunch.x = Math.Clamp( vViewPunch.x, -73, 73 );
	vViewPunch.y = Math.Clamp( vViewPunch.y, -52, 52 );
	vViewPunch.z = Math.Clamp( vViewPunch.z, -52, 52 );
}

//=========================================//
// Internal State Modifiers
//=========================================//

// Setters of the view bob. Separates the movement and view bob just enough.
void CAfterPlayer::camViewBobSetSpeed ( float fnSpeed )
{
	fViewBobSpeed = fabs(fnSpeed) / float(PI);
}
void CAfterPlayer::camViewBobSetTargetSize ( float fnTarget )
{
	fViewBobTarget = fnTarget*0.5f;
}
// View angle setters
void CAfterPlayer::camViewAngleSetPOffset ( float fnTarget )
{
	fViewAnglePTarget = fnTarget;
}
// View roll
void CAfterPlayer::camRollSetOffset ( float fnTarget )
{
	fViewRollTarget = fnTarget;
}

//=========================================//
//  MCC Player Cameratypes
//=========================================//

// Default camera update state
void*	CAfterPlayer::cam_Default ( void )
{
	// Set the position of the view
	Vector3d defCamRot (0,0,0);
	if ( model )
	{
		XTransform camtrans;
		model->GetEyecamTransform( camtrans );
		pCamera->transform.position = camtrans.position;
		//pCamera->transform.position.z = ( (pCamera->transform.position.z)*0.6f + (transform.position.z + fPlayerHeight-0.6f)*0.4f ); // Stabilize vertical height of camera a little bit more
	
		defCamRot = (camtrans.rotation*Quaternion( Vector3d( 90,0,90 ) )).GetEulerAngles() ;
	}

	// Do mouse look
	//vCameraRotation.z = vPlayerRotation.z;
	//vCameraRotation.y -= vTurnInput.y;
	//vTurnInput.y = 0;

	// Limit horizontal head angle
	if ( vCameraRotation.z > 95 )
		vCameraRotation.z = 95;
	if ( vCameraRotation.z < -95 )
		vCameraRotation.z = -95;
	// Limit vertical head angle
	if ( vCameraRotation.y > 95 )
		vCameraRotation.y = 95;
	if ( vCameraRotation.y < -95 )
		vCameraRotation.y = -95;

	// Perform view bob calculations
	fViewBobTimer += Time::deltaTime*fViewBobSpeed;
	if ( fViewBobTimer > PI*2 )
		fViewBobTimer -= (float)(PI)*2;
	fViewBobAmount += (fViewBobTarget-fViewBobAmount)*Time::TrainerFactor( 50 );
	vViewBob.x = (float)cosf( fViewBobTimer )*fViewBobAmount;	 //Roll
	vViewBob.z = (float)sinf( fViewBobTimer )*-fViewBobAmount;	 //Yaw
	vViewBob.y = (float)cosf( fViewBobTimer*2 )*-fViewBobAmount; //Pitch
	vViewBob = Vector3d(0,0,0);
	// view bob is now view shake
	vViewBob = Vector3d( Random.Range(-1.0f,1.0f), Random.Range(-1.0f,1.0f), Random.Range(-1.0f,1.0f)*0.15f )*fViewBobAmount;

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
	//pCamera->transform.rotation.Euler( vCameraRotation+vViewBob+vViewPunch+vCameraAnimOffsetRotation );
	Rotator normalRot ( (vPlayerRotation+vCameraRotation)+vViewBob+vViewPunch+vCameraAnimOffsetRotation );
	Rotator animRot ( defCamRot+vViewBob+vViewPunch );

	//pCamera->transform.rotation.Euler();
	if ( bUseAnimCamMode ) {
		fCamAnimBlend = std::min<ftype>( 1, fCamAnimBlend + Time::smoothDeltaTime*6.0f );
		//fCamAnimBlend = 1;
	}
	else {
		fCamAnimBlend = std::max<ftype>( 0, fCamAnimBlend - Time::smoothDeltaTime*6.0f );
		//fCamAnimBlend = 0;
	}
	pCamera->transform.rotation = normalRot.LerpTo( animRot, fCamAnimBlend );
	//pCamera->transform.rotation = animRot;

	pCamera->fov = fViewAngle + fViewAngleOffset + fViewAnglePOffset;
	if ( pCamera->fov > 150 )
		pCamera->fov = 150;

	return camQuickReturn( cam_Default );
}

// Third person camera update state
void*	CAfterPlayer::cam_ThirdPerson ( void )
{
	// Do mouse look
	//vCameraRotation.z = vPlayerRotation.z;
	//vCameraRotation.y -= vTurnInput.y;
	//vTurnInput.y = 0;

	// Limit horizontal head angle
	if ( vCameraRotation.z > 95 )
		vCameraRotation.z = 95;
	if ( vCameraRotation.z < -95 )
		vCameraRotation.z = -95;
	// Limit vertical head angle
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
	pCamera->transform.rotation.Euler( (vPlayerRotation+vCameraRotation)+vViewBob+vViewPunch+vCameraAnimOffsetRotation );
	pCamera->fov = fViewAngle + fViewAngleOffset + fViewAnglePOffset;
	if ( pCamera->fov > 150 )
		pCamera->fov = 150;

	// Set the position of the view
	Real t_currentHeight;
	if ( m_motion ) {
		t_currentHeight = m_motion->fPlayerHeight;
	}
	else {
		t_currentHeight = pl_race_stats->fStandingHeight;
	}
	if ( model )
	{
		XTransform camtrans;
		model->GetEyecamTransform( camtrans );
		pCamera->transform.position = model->transform.position;
		pCamera->transform.position.z = camtrans.position.z;
		pCamera->transform.position.z = ( (pCamera->transform.position.z)*0.2f + (transform.position.z + t_currentHeight-0.6f)*0.8f ); // Stabilize vertical height of camera a LOT
		pCamera->transform.position.z = ( (pCamera->transform.position.z)*0.2f + (transform.position.z + t_currentHeight*0.6f)*0.8f ); // And bring it down even farther
	}
	else
	{
		pCamera->transform.position = transform.position;
	}
	// Pull camera back
	pCamera->transform.position -= pCamera->transform.rotation * Vector3d( 5.4f*0.7f+t_currentHeight*0.3f,0,0 );

	return camQuickReturn( cam_ThirdPerson );
}


// Third person camera update state
void*	CAfterPlayer::cam_Inventory ( void )
{
	static Vector3d l_startPosition;
	static Rotator  l_startRotation;
	static Real		l_startFoV;
	// If first frame, grab source position
	const Real t_startOffset = 0.2f;
	if ( bCamSwapFirstframe ) 
	{
		l_startPosition = pCamera->transform.position;
		l_startRotation = pCamera->transform.rotation;
		l_startFoV		= pCamera->fov;
		fCamSwapBlend = t_startOffset;
		bCamSwapFirstframe = false;
	}
	// Blend to target position
	fCamSwapBlend += Time::deltaTime;
	if ( fCamSwapBlend >= 1 ) {
		fCamSwapBlend = 1;
	}
	Real t_blendValue = Math.Smoothlerp( fCamSwapBlend,0,1 );
	Real t_blendValue_S0 = Math.Smoothlerp( ((fCamSwapBlend-t_startOffset)/(1-t_startOffset))*2, 0,1 );
	Real t_blendValue_S1 = Math.Smoothlerp( fCamSwapBlend*2-1, 0,1 );

	// Set the position of the view
	if ( model )
	{
		XTransform camtrans;
		model->GetEyecamTransform( camtrans );
		pCamera->transform.position = (camtrans.position + transform.position)*0.5f;
	}

	// Limit horizontal head angle
	if ( vCameraRotation.z > 95 )
		vCameraRotation.z = 95;
	if ( vCameraRotation.z < -95 )
		vCameraRotation.z = -95;
	// Limit vertical head angle
	if ( vCameraRotation.y > 95 )
		vCameraRotation.y = 95;
	if ( vCameraRotation.y < -95 )
		vCameraRotation.y = -95;

	// Pull camera back
	pCamera->transform.position -= pCamera->transform.rotation * Vector3d( 3.0f+(3.0f*(1-t_blendValue_S0)),-1.0f*t_blendValue_S1,-0.6f*(t_blendValue_S0*(1-t_blendValue_S1))-0.8f*t_blendValue ) * 1.4f;

	// Perform view bob calculations
	fViewBobTimer += Time::deltaTime*fViewBobSpeed;
	if ( fViewBobTimer > PI*2 )
		fViewBobTimer -= (float)(PI)*2;
	fViewBobAmount += (fViewBobTarget-fViewBobAmount)*Time::TrainerFactor( 50 );
	vViewBob.x = (float)cosf( fViewBobTimer )*fViewBobAmount;	//Roll
	vViewBob.z = (float)sinf( fViewBobTimer )*-fViewBobAmount;	//Yaw
	vViewBob.y = (float)cosf( fViewBobTimer*2 )*-fViewBobAmount; //Pitch

	// Perform view punch calculations
	vViewPunchVelocity += ( -vViewPunch*22.5f - vViewPunchVelocity*0.6f )*Time::deltaTime;
	vViewPunch += ( vViewPunchVelocity - vViewPunch*3.6f )*Time::deltaTime;
	// Perform view angle calculations
	fViewAnglePOffset += ( fViewAnglePTarget-fViewAnglePOffset )*Time::TrainerFactor( 1 );
	// Perform view roll calculations
	fViewRollOffset += ( fViewRollTarget-fViewRollOffset )*Time::TrainerFactor( 0.3f );
	vCameraRotation.x = fViewRollOffset;

	// Set the final camera rotation and view angle
	//pCamera->transform.rotation.Euler( vCameraRotation+vViewBob+vViewPunch+vCameraAnimOffsetRotation+Vector3d(0,-15,160) );
	Vector3d t_angleOffset (0,-15,160);
	t_angleOffset = t_angleOffset*t_blendValue_S1 + (Vector3d(0,-40,0)*t_blendValue_S0*(1-t_blendValue_S1));

	pCamera->transform.rotation.Euler( vPlayerRotation+Vector3d(0,0,vCameraRotation.z)+vViewBob+vViewPunch+vCameraAnimOffsetRotation+t_angleOffset );
	pCamera->fov = fViewAngle + fViewAngleOffset + fViewAnglePOffset;
	if ( pCamera->fov > 150 )
		pCamera->fov = 150;

	// Now perform the interpolation
	pCamera->transform.position = l_startPosition.lerp( pCamera->transform.position, t_blendValue );
	//pCamera->transform.rotation = l_startRotation.LerpTo( pCamera->transform.rotation, t_blendValue );
	pCamera->transform.rotation = l_startRotation.getQuaternion().Slerp( pCamera->transform.rotation, t_blendValue );
	pCamera->fov = Math.Lerp( t_blendValue, l_startFoV, pCamera->fov );

	return camQuickReturn( cam_Inventory );
}

// Beauty camera
void*	CAfterPlayer::cam_Beauty ( void )
{
	static Vector3d l_cameraRotation ( 0,0,0 );
	static Vector3d l_previousCameraRot ( 0,0,0 );
	static Vector3d l_previousPlayerRot ( 0,0,0 );
	static float	l_cameraDist = 0;

	// Change camera rotations when Shift is pressed and mouse moved
	if ( Input::Key( Keys.Shift ) ) {
		l_cameraRotation += vCameraRotation-l_previousCameraRot;
		l_cameraRotation += vPlayerRotation-l_previousPlayerRot;
		vCameraRotation = l_previousCameraRot;
		vPlayerRotation = l_previousPlayerRot;
		if ( Input::Mouse(Input::MBLeft) ) {
			l_cameraDist -= 1.4f * Time::deltaTime;
		}
		if ( Input::Mouse(Input::MBRight) ) {
			l_cameraDist += 1.4f * Time::deltaTime;
		}
	}
	// Remove roll, though
	Real t_subAmount = 20 * Time::deltaTime;
	if ( fabs(l_cameraRotation.x) > t_subAmount ) {
		l_cameraRotation.x -= Math.sgn( l_cameraRotation.x ) * t_subAmount;
	}
	else {
		l_cameraRotation.x = 0;
	}

	// Save previous rotations
	l_previousCameraRot = vCameraRotation;
	l_previousPlayerRot = vPlayerRotation;

	// Limit horizontal head angle
	if ( vCameraRotation.z > 95 )
		vCameraRotation.z = 95;
	if ( vCameraRotation.z < -95 )
		vCameraRotation.z = -95;
	// Limit vertical head angle
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
	vViewPunchVelocity += ( -vViewPunch*22.5f - vViewPunchVelocity*0.6f )*Time::deltaTime;
	vViewPunch += ( vViewPunchVelocity - vViewPunch*3.6f )*Time::deltaTime;

	// Perform view angle calculations
	fViewAnglePOffset += ( fViewAnglePTarget-fViewAnglePOffset )*Time::TrainerFactor( 1 );
	// Perform view roll calculations
	fViewRollOffset += ( fViewRollTarget-fViewRollOffset )*Time::TrainerFactor( 0.3f );
	vCameraRotation.x = fViewRollOffset;

	// Set the final camera rotation and view angle
	pCamera->transform.rotation.Euler( (vPlayerRotation+vCameraRotation+l_cameraRotation)+vViewBob+vViewPunch+vCameraAnimOffsetRotation );
	pCamera->fov = fViewAngle + fViewAngleOffset + fViewAnglePOffset;
	if ( pCamera->fov > 150 )
		pCamera->fov = 150;

	// Set the position of the view
	Real t_currentHeight;
	if ( m_motion ) {
		t_currentHeight = m_motion->fPlayerHeight;
	}
	else {
		t_currentHeight = pl_race_stats->fStandingHeight;
	}
	if ( model )
	{
		XTransform camtrans;
		model->GetEyecamTransform( camtrans );
		pCamera->transform.position = camtrans.position;
		pCamera->transform.position.z = ( (pCamera->transform.position.z)*0.2f + (transform.position.z + t_currentHeight-0.6f)*0.8f ); // Stabilize vertical height of camera a LOT
		pCamera->transform.position.z = ( (pCamera->transform.position.z)*0.2f + (transform.position.z + t_currentHeight*0.6f)*0.8f ); // And bring it down even farther

		pCamera->transform.position = pCamera->transform.position.lerp(
			Vector3d(
				camtrans.position.x,
				camtrans.position.y,
				Math.lerp(0.22f,camtrans.position.z,transform.position.z+t_currentHeight-0.3f) ),
			-l_cameraDist*0.35f );
	}
	// Pull camera back
	pCamera->transform.position -= pCamera->transform.rotation * Vector3d( 5.4f*0.7f+t_currentHeight*0.3f+l_cameraDist,0,0 );

	return camQuickReturn( cam_Beauty );
}

// Beauty camera
void*	CAfterPlayer::cam_BeautyMontage ( void )
{
	static Real	l_montageCamera = 0;
	static int	l_montageMode = 0;
	static Real	l_montageTimer = 0.5f;
	static bool l_montageFirstFrame = true;

	// Limit horizontal head angle
	if ( vCameraRotation.z > 95 )
		vCameraRotation.z = 95;
	if ( vCameraRotation.z < -95 )
		vCameraRotation.z = -95;
	// Limit vertical head angle
	if ( vCameraRotation.y > 95 )
		vCameraRotation.y = 95;
	if ( vCameraRotation.y < -95 )
		vCameraRotation.y = -95;
	// Limit the FoV
	if ( pCamera->fov > 150 )
		pCamera->fov = 150;

	// Perform the different camera modes
	l_montageTimer -= Time::deltaTime;

	// Swap between camera states here and there
	if ( l_montageTimer <= 0 )
	{
		l_montageTimer = Random.Range( 1.7f,5 )*1.2f; // Make this number smaller for more of a montage
		int nextMode;
		do {
			nextMode = Random.Next() % 4;
		}
		while ( l_montageMode == nextMode );
		l_montageMode = nextMode;
		l_montageFirstFrame = true;
		if ( l_montageMode == 0 ) {
			l_montageTimer *= 0.15f;
		}
		else if ( l_montageMode == 2 ) {
			l_montageTimer *= 0.35f;
		}
	}

	// Get player height to scale from
	Real t_currentHeight;
	if ( m_motion ) {
		t_currentHeight = m_motion->fPlayerHeight;
	}
	else {
		t_currentHeight = pl_race_stats->fStandingHeight;
	}

	// 
	switch ( l_montageMode )
	{
	case 0: // Spin around character
		{
			static Real l_rotation;
			static Real l_roll;
			static Real l_xheight;
			static Real l_direction;
			if ( l_montageFirstFrame )
			{
				l_rotation = Random.Range( -180,180 );
				l_roll = Random.Range( -15,15 );
				l_xheight = Random.Range( -15,15 );
				if ( Random.Next()%2 ) {
					l_direction = 1;
				}
				else {
					l_direction = -1;
				}
				l_direction *= Random.Range( 0.8f,1.2f );
			}
			l_rotation += Time::deltaTime * 100.0f * l_direction;

			pCamera->transform.rotation.Euler( Vector3d( l_roll, l_xheight, l_rotation ) );
			pCamera->fov = 110;
			pCamera->transform.position = transform.position + Vector3d( 0,0,t_currentHeight*0.5f );

			// Pull camera back
			pCamera->transform.position -= pCamera->transform.rotation * Vector3d( 4.2f,0,0 );
		}
		break;
	case 1: // Choose a slowly rotating view
		{
			static Real l_rotation = 0;
			if ( l_montageFirstFrame )
			{
				l_rotation += Random.Range( -45, 45 );
			}
			l_rotation -= Time::deltaTime * 10.0f;

			pCamera->fov = 80;
			pCamera->transform.rotation.Euler( Vector3d(0,0,l_rotation)+Vector3d(0,-15,160) );

			XTransform camtrans;
			model->GetEyecamTransform( camtrans );
			pCamera->transform.position = (camtrans.position + transform.position)*0.5f;
			pCamera->transform.position -= pCamera->transform.rotation * Vector3d( 3.0f,-1.0f,-0.8f ) * 1.4f;
		}
		break;
	case 2: // Zoom to the side of the character
		{
			static Real l_rotation = 0;
			if ( l_montageFirstFrame )
			{
				switch ( Random.Next()%4 ) {
				case 0: l_rotation = 90;
					break;
				case 1: l_rotation = 45+90;
					break;
				case 2: l_rotation = 180;
					break;
				case 3: l_rotation = 90;
					break;
				}
				if ( Random.Next()%2 ) {
					l_rotation *= -1;
				}
			}

			pCamera->fov = 35;
			pCamera->transform.rotation.Euler( vPlayerRotation + Vector3d( 0,0,l_rotation ) );

			XTransform camtrans;
			model->GetEyecamTransform( camtrans );
			pCamera->transform.position = camtrans.position;
			pCamera->transform.position -= pCamera->transform.rotation * Vector3d(3.5f,0,0.8f );
		}
		break;
	case 3: // Select a faster rotating view, closer to player feet
		{
			static Real l_rotation = 0;
			if ( l_montageFirstFrame )
			{
				l_rotation += Random.Range( -45, 45 );
			}
			l_rotation += Time::deltaTime * 17.0f;

			pCamera->fov = 50;
			pCamera->transform.rotation.Euler( Vector3d(0,0,l_rotation)+Vector3d(0,5,160) );

			XTransform camtrans;
			model->GetEyecamTransform( camtrans );
			pCamera->transform.position = camtrans.position.lerp(transform.position,0.7f);
			pCamera->transform.position -= pCamera->transform.rotation * Vector3d( 3.5f+t_currentHeight*0.5f,0.0f,-0.2f ) * 1.4f;
		}
		break;
	}


	// No longer first frame
	l_montageFirstFrame = false;

	// Perform view bob calculations
	/*fViewBobTimer += Time::deltaTime*fViewBobSpeed;
	if ( fViewBobTimer > PI*2 )
		fViewBobTimer -= (float)(PI)*2;
	fViewBobAmount += (fViewBobTarget-fViewBobAmount)*Time::TrainerFactor( 50 );
	vViewBob.x = (float)cosf( fViewBobTimer )*fViewBobAmount;	//Roll
	vViewBob.z = (float)sinf( fViewBobTimer )*-fViewBobAmount;	//Yaw
	vViewBob.y = (float)cosf( fViewBobTimer*2 )*-fViewBobAmount; //Pitch

	// Perform view punch calculations
	vViewPunchVelocity += ( -vViewPunch*22.5f - vViewPunchVelocity*0.6f )*Time::deltaTime;
	vViewPunch += ( vViewPunchVelocity - vViewPunch*3.6f )*Time::deltaTime;

	// Perform view angle calculations
	fViewAnglePOffset += ( fViewAnglePTarget-fViewAnglePOffset )*Time::TrainerFactor( 1 );
	// Perform view roll calculations
	fViewRollOffset += ( fViewRollTarget-fViewRollOffset )*Time::TrainerFactor( 0.3f );
	vCameraRotation.x = fViewRollOffset;

	// Set the final camera rotation and view angle
	pCamera->transform.rotation.Euler( (vPlayerRotation+vCameraRotation)+vViewBob+vViewPunch+vCameraAnimOffsetRotation );
	pCamera->fov = fViewAngle + fViewAngleOffset + fViewAnglePOffset;

	// Set the position of the view
	if ( model )
	{
		XTransform camtrans;
		model->GetEyecamTransform( camtrans );
		pCamera->transform.position = camtrans.position;
		pCamera->transform.position.z = ( (pCamera->transform.position.z)*0.2f + (transform.position.z + fPlayerHeight-0.6f)*0.8f ); // Stabilize vertical height of camera a LOT
		pCamera->transform.position.z = ( (pCamera->transform.position.z)*0.2f + (transform.position.z + fPlayerHeight*0.6f)*0.8f ); // And bring it down even farther
	}
	// Pull camera back
	pCamera->transform.position -= pCamera->transform.rotation * Vector3d( 5.4f*0.7f+fPlayerHeight*0.3f,0,0 );*/

	return camQuickReturn( cam_BeautyMontage );
}

// Remove definition to not pollute the compiler
#ifdef camQuickReturn
#undef camQuickReturn
#endif