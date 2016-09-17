
// Includes
#include "core/math/random/Random.h"
#include "core-ext/input/CInputControl.h"

#include "engine/audio/CAudioInterface.h"
#include "engine-common/entities/CPlayer.h"

#include "engine/physics/motion/CRigidbody.h"
#include "engine/physics/collider/types/CCapsuleCollider.h"

#include "engine-common/entities/CParticleSystem.h"
#include "renderer/logic/particle/CParticleEmitter.h"
#include "renderer/material/glMaterial.h"
#include "renderer/texture/CTexture.h"

#include "renderer/camera/CCamera.h"

#include "CCloudPlayer.h"

CCloudPlayer::CCloudPlayer ( void )
	: CPlayer(),
	drive_afterburner(false), drive_gravitydrive(false),
	fuel_afterburner(0), fuel_gravitydrive(0)
{
	// Create collision sphere
	const Real t_sphereRadius = 2.5F;
	collider = new CCapsuleCollider( t_sphereRadius*2, t_sphereRadius, true );

	// Create motion rigidbody
	rigidbody = new CRigidBody( collider, this, 100.0F );
	rigidbody->SetGravity(false);
	rigidbody->SetAngularDamping( 0.1F );
	rigidbody->SetLinearDamping( 0.1F );


	// Zoom particle shits
	particlesystem = new CParticleSystem("particlesystems/zoom_shit.pcf");
	particlesystem->transform.SetParent(&transform);
	particlesystem->transform.localPosition = Vector3d(0,0,0);
	{
		particlesystem->GetEmitter()->vEmitterSize = Vector3d( 20,20,20 );
		particlesystem->GetEmitter()->rfStartSize.SetRange( 0.3F, 0.3F );
		particlesystem->GetEmitter()->rfEndSize.SetRange( 0.3F, 0.3F );

		glMaterial* particle_material = new glMaterial;
		particle_material->setTexture( 0, new CTexture("textures/white.jpg") );
		particle_material->passinfo.push_back( glPass() );
		particle_material->passinfo[0].shader = new glShader( ".res/shaders/particles/colorBlended.glsl" );
		particle_material->passinfo[0].m_blend_mode = Renderer::BM_ADD;
		particle_material->passinfo[0].b_depthmask = false;
		particle_material->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
		particle_material->m_diffuse = Color(1,1,1,0.5F);
		particlesystem->GetRenderable()->SetMaterial( particle_material );
		particle_material->removeReference();
	}
}

CCloudPlayer::~CCloudPlayer ( void )
{
	delete_safe_decrement( rigidbody );
	delete_safe( collider );

	delete_safe_decrement( particlesystem );
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

	// Update control logic
	controlNormalShip();

	// Update camera
	camShipFirstPerson();

	// Update listener position
	pListener->velocity = pCamera->transform.position - pListener->position;
	pListener->position = pCamera->transform.position;
	pListener->orient_forward = pCamera->transform.Forward();
	pListener->orient_up = pCamera->transform.Up();

	// Update physics broadphase
	/*Vector3d world_center;
	Physics::GetWorldCenter(world_center);
	if ( (world_center - transform.position).magnitude() > 100 )
	{
		Physics::ShiftWorld( transform.position - world_center );
	}
	std::cout << world_center << " vs " << transform.position << std::endl;*/

	// Center the world around the player
	if ( transform.position.magnitude() > 200 )
	{
		transform.root.position -= transform.position;
	}

	rigidbody->Wake();
}

void CCloudPlayer::controlNormalShip ( void )
{
	// Degrade shake amount over time
	shakeAmount = std::max( 0.0F, shakeAmount - Time::deltaTime * 5.0F );
	// Degrade widen over time
	fovWiden = std::max( rigidbody->GetVelocity().magnitude() * 0.1F, fovWiden - Time::deltaTime * 25.0F );

	// Gravity drive behavior
	if ( input->axes.sprint )
	{
		if ( fuel_gravitydrive > 0 )
		{
			fuel_gravitydrive -= Time::deltaTime * 0.1F;
			drive_gravitydrive = true;
			
			shakeAmount = std::max( shakeAmount, 1.0F );
		}
		else
		{
			drive_gravitydrive = false;
		}
	}
	else
	{
		fuel_gravitydrive = std::min( fuel_gravitydrive + Time::deltaTime * 0.1F, 1.0F );
		drive_gravitydrive = false;
	}

	// Afterburner behavior
	if ( input->axes.menuToggle )
	{
		if ( fuel_afterburner > 0 )
		{
			fuel_afterburner -= Time::deltaTime * 0.5F;
			drive_afterburner = true;

			shakeAmount = std::max( shakeAmount, 5.0F );
			fovWiden = std::min( fovWiden + Time::deltaTime * 90.0F, std::max( fovWiden, 35.0F ) );
		}
		else
		{
			drive_afterburner = false;
		}
	}
	else
	{
		fuel_afterburner = std::min( fuel_afterburner + Time::deltaTime * 0.125F, 1.0F );
		drive_afterburner = false;
	}

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
	Vector3d randomRotation = Random.PointInUnitSphere() * shakeAmount;
	randomRotation.x *= 0.15F;

	pCamera->transform.rotation = playerRotation * Rotator(randomRotation);
	pCamera->fov = 75.0F + fovWiden;

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
	if ( drive_gravitydrive )
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
		if ( drive_afterburner )
		{
			t_velocity += playerRotation * Vector3d(1,0,0) * Time::deltaTime * 60.0F;
		}
		else
		{
			t_velocity += moveVector * Time::deltaTime * 10.0F;
		}
	}


	rigidbody->SetVelocity(t_velocity);

	return NULL;
}