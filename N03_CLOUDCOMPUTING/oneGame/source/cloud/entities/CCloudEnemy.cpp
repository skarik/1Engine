#include "CCloudEnemy.h"
// Includes
#include "core-ext/input/CInputControl.h"

#include "engine/audio/CAudioInterface.h"

#include "engine/physics/motion/CRigidbody.h"
#include "engine/physics/collider/types/CCapsuleCollider.h"

#include "renderer/camera/CCamera.h"

CCloudEnemy::CCloudEnemy(void)
	: CActor()
{
	// Create collision sphere
	const Real t_sphereRadius = 2.5F;
	collider = new CCapsuleCollider(t_sphereRadius * 2, t_sphereRadius, true);

	// Create motion rigidbody
	rigidbody = new CRigidBody(collider, this, 100.0F);
	rigidbody->SetGravity(false);
}

CCloudEnemy::~CCloudEnemy(void)
{
	delete_safe_decrement(rigidbody);
	delete_safe(collider);
}


// Game step

void CCloudEnemy::Update(void)
{
	// Update base class code first
	CActor::Update();
}

void CCloudEnemy::LateUpdate(void)
{
	//GrabInput();

	//camShipFirstPerson();

	// Update listener position
	//Not sure if enemies need this stuff - Chiefmasamune
	/*
	pListener->velocity = pCamera->transform.position - pListener->position;
	pListener->position = pCamera->transform.position;
	pListener->orient_forward = pCamera->transform.Forward();
	pListener->orient_up = pCamera->transform.Up();*/
}

// Physics Step

void CCloudEnemy::FixedUpdate(void)
{
	mvtNormalShip();
}

// == Phase Movetype ==
// Phase flying. For the win.
//Need to get movement instructions from AI
void*	CCloudEnemy::mvtNormalShip(void)
{
	// Turn first
	enemyRotation *= Rotator(Vector3d(0, -vTurnInput.y, vTurnInput.x));
	vTurnInput.x = 0;
	vTurnInput.y = 0;

	//Set the direction the enemy will move in
	Vector3d moveVector(0, 0, 0);
	moveVector.x = vDirInput.x;
	moveVector.y = vDirInput.y;
	moveVector.z = 0;

	//Jump to go up, crouch 
	if (input.jump) {
		moveVector.z = 1;
	}
	if (input.crouch) {
		moveVector.z = -1;
	}

	// Rotate the move vector to match the player
	moveVector = enemyRotation*moveVector;

	Vector3d t_velocity = rigidbody->GetVelocity();

	// Shift for gravity drive. Instantly rotates velocity but prevent acceleration
	if (input.sprint)
	{
		//t_velocity += moveVector * Time::deltaTime * 30.0f;
		if (moveVector.sqrMagnitude() > 0.1F)
		{
			t_velocity = (moveVector + t_velocity.normal()).normal() * t_velocity.magnitude();
		}
	}
	else
	{
		// Tab for boost shit
		if (input.menuToggle)
		{
			//if ( t_velocity.sqrMagnitude() < sqr(100) )
			{
				t_velocity += enemyRotation * Vector3d(1, 0, 0) * Time::deltaTime * 60.0F;
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

void CCloudEnemy::SetTurnInput(Vector3d turn)
{
	vTurnInput.x = turn.x;
	vTurnInput.y = turn.y;
	vTurnInput.z = turn.z;
}

void CCloudEnemy::SetDirInput(Vector3d dir)
{
	vDirInput.x = dir.x;
	vDirInput.y = dir.y;
	vDirInput.z = dir.z;
}

void CCloudEnemy::SetVAxes(int flags)
{
	if (flags & 1)
		input.crouch = true;
	
	if (flags & 2)
		input.jump = true;

	if (flags & 4)
		input.menuToggle = true;

	if (flags & 8)
		input.sprint = true;
}