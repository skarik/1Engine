
// Includes
#include "core-ext/input/CInputControl.h"

#include "engine/audio/CAudioInterface.h"
#include "engine-common/entities/CPlayer.h"

#include "engine/physics/motion/CRigidbody.h"
#include "engine/physics/collider/types/CCapsuleCollider.h"

#include "renderer/camera/CCamera.h"

#include "CCloudPlayer.h"

CCloudPlayer::CCloudPlayer ( void )
	: CPlayer()
{
	// Create collision sphere
	const Real t_sphereRadius = 2.5F;
	collider = new CCapsuleCollider( t_sphereRadius*2, t_sphereRadius, true );

	// Create motion rigidbody
	rigidbody = new CRigidBody( collider, this, 100.0F );
	rigidbody->SetGravity(false);
}

CCloudPlayer::~CCloudPlayer ( void )
{
	delete_safe_decrement( rigidbody );
	delete_safe( collider );
}


// Game step

void CCloudPlayer::Update ( void )
{
	// Update base class code first
	CActor::Update();
}

void CCloudPlayer::LateUpdate ( void )
{
	GrabInput();

	camShipFirstPerson();

	// Update listener position
	pListener->velocity = pCamera->transform.position - pListener->position;
	pListener->position = pCamera->transform.position;
	pListener->orient_forward = pCamera->transform.Forward();
	pListener->orient_up = pCamera->transform.Up();
}

// Physics Step

void CCloudPlayer::FixedUpdate ( void )
{
	mvtNormalShip();
}


// ==Camera States==
// Default camera update state
void*	CCloudPlayer::camShipFirstPerson ( void )
{
	// Set the position of the view
	pCamera->transform.position = transform.position;

	// Do mouse look
	//vCameraRotation.z = vPlayerRotation.z;
	//vCameraRotation.y -= vTurnInput.y;
	//vTurnInput.y = 0;
	//vCameraRotation = playerRotation.getEulerAngles();

	// Limit vertical mouselook angle
	/*if ( vCameraRotation.y > 95 )
		vCameraRotation.y = 95;
	if ( vCameraRotation.y < -95 )
		vCameraRotation.y = -95;*/

	// Set the final camera rotation and view angle
	//pCamera->transform.rotation.Euler( vCameraRotation );
	pCamera->transform.rotation = playerRotation;
	pCamera->fov = 90.0f;

	return NULL;
}


// == Phase Movetype ==
// Phase flying. For the win.
void*	CCloudPlayer::mvtNormalShip ( void )
{
	// Turn first
	playerRotation *= Rotator( Vector3d( 0, -vTurnInput.y, vTurnInput.x ) );
	vTurnInput.x = 0;
	vTurnInput.y = 0;

	Vector3d moveVector(0,0,0);
	moveVector.x = vDirInput.x;
	moveVector.y = vDirInput.y;
	moveVector.z = 0;
	if ( input->axes.jump ) {
		moveVector.z = 1;
	}
	if ( input->axes.crouch ) {
		moveVector.z = -1;
	}

	// Rotate the move vector to match the player
	moveVector = playerRotation*moveVector;

	

	Vector3d t_velocity = rigidbody->GetVelocity();


	// Shift for gravity drive. Instantly rotates velocity but prevent acceleration
	if ( input->axes.sprint )
	{ 
		//t_velocity += moveVector * Time::deltaTime * 30.0f;
		if ( moveVector.sqrMagnitude() > 0.1F )
		{
			t_velocity = (moveVector + t_velocity.normal()).normal() * t_velocity.magnitude();
		}
	}
	else
	{
		// Tab for boost shit
		if ( input->axes.menuToggle )
		{
			//if ( t_velocity.sqrMagnitude() < sqr(100) )
			{
				t_velocity += playerRotation * Vector3d(1,0,0) * Time::deltaTime * 60.0F;
			}
		}
		else
		{
			t_velocity += moveVector * Time::deltaTime * 10.0F;
		}
	}


	rigidbody->SetVelocity(t_velocity);

	return NULL;
}