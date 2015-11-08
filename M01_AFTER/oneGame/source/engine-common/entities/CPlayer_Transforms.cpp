/*
// Includes
#include "CPlayer.h"


// ===Transform Getters===
XTransform	CPlayer::GetHoldTransform ( char i_handIndex )
{


	XTransform result;
	result.rotation = -pCamera->transform.rotation.getEulerAngles();

	Matrix4x4 rotMatrix, rotMatrix2;
	rotMatrix.setRotation( pCamera->transform.rotation );
	rotMatrix2.setRotation( Vector3d( 0,-27,29 ) );
	result.position = ( pCamera->transform.position + (rotMatrix*(rotMatrix2*Vector3d(1.0f,0,0))) );

	return result;
}
XTransform	CPlayer::GetBeltTransform ( char i_beltIndex )
{

	float fPlayerHeight = 5.5f;

	XTransform result;
	result.rotation = (!transform.rotation).getQuaternion();
	result.position = transform.position + transform.Forward() + Vector3d( 0,0,fPlayerHeight*0.5f );
	return result;
}
*/