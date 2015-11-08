
// Includes
#include "CAfterPlayer.h"
#include "after/entities/CCharacterModel.h"
#include "after/interfaces/CCharacterMotion.h" // Getting player height to scale against (eventually remove!)


// ===Common Getters===
// Get eye ray. Commonly used for raycasts.
Ray		CAfterPlayer::GetEyeRay ( void )
{
	if ( m_cameraUpdateType	== (stateFunc_t)&CAfterPlayer::cam_Default )
	{
		return Ray( pCamera->transform.position, pCamera->transform.Forward() );
	}
	else
	{
		XTransform transCamPos;
		model->GetEyecamTransform( transCamPos );
		return Ray( transCamPos.position, transCamPos.rotation * Vector3d::left );
	}
}
// Get aiming direction. Used for melee weapons.
Rotator	CAfterPlayer::GetAimRotator ( void )
{
	/*XTransform transCamPos;
	model->GetEyecamTransform( transCamPos );
	return Quaternion::CreateRotationTo( Vector3d::forward, transCamPos.rotation * Vector3d::left );*/
	return pCamera->transform.rotation;
}

// ===Transform Getters===
XTransform	CAfterPlayer::GetHoldTransform ( char i_handIndex )
{
	/*Matrix4x4 resultT, resultR;
	Matrix4x4 rotMatrix, rotMatrix2;

	resultR.setRotation( -pCamera->transform.rotation.getEulerAngles() );

	rotMatrix.setRotation( pCamera->transform.rotation );
	rotMatrix2.setRotation( Vector3d( 0,-27,29 ) );
	resultT.setTranslation( pCamera->transform.position + (rotMatrix*(rotMatrix2*Vector3d(1.0f,0,0))) );

	return resultT*resultR;*/

	/*Matrix4x4 resultT, resultR;
	XTransform prop01Transform;

	model->GetProp01Transform( prop01Transform );

	resultT.setTranslation( prop01Transform.position );
	resultR.setRotation( prop01Transform.rotation );

	return resultT*resultR;*/

	XTransform prop01Transform;

	if ( i_handIndex == 0 ) {
		model->GetProp01Transform( prop01Transform );
	}
	else if ( i_handIndex == 1 ) {
		model->GetProp02Transform( prop01Transform );
	}
	else {
		model->GetEyecamTransform( prop01Transform );
	}

	return prop01Transform;
}
XTransform	CAfterPlayer::GetBeltTransform ( char i_beltIndex )
{
	/*Matrix4x4 resultT, resultR;
	resultR.setRotation( !transform.rotation );
	resultT.setTranslation( transform.position + transform.Forward() + Vector3d( 0,0,fPlayerHeight*0.5f ) );
	return resultT*resultR;*/
	
	// Get player height to scale from
	Real t_currentHeight;
	if ( m_motion ) {
		t_currentHeight = m_motion->fPlayerHeight;
	}
	else {
		t_currentHeight = pl_race_stats->fStandingHeight;
	}

	XTransform result;
	result.rotation = transform.rotation.getQuaternion();
	result.position = transform.position + transform.Forward() + Vector3d( 0,0,t_currentHeight*0.5f );
	return result;
}
