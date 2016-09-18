
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
#include "renderer/object/mesh/CStreamedMesh.h"

#include "CCloudPlayer.h"

#include "cloud/entities/projectiles/ProjectileBullet.h"
#include "cloud/entities/projectiles/ProjectilePlasma.h"
#include "cloud/entities/projectiles/ProjectileMissile.h"

#include "engine-common/utils/CDeveloperConsoleUI.h"

#include "renderer/object/screen/CScreenFade.h"
#include "core-ext/system/io/Resources.h"

#include "engine/audio/CAudioInterface.h"
#include "audio/CAudioSource.h"
#include "engine/audio/CSoundBehavior.h"

CCloudPlayer::CCloudPlayer ( void )
	: CPlayer(),
	drive_afterburner(false), drive_gravitydrive(false),
	fuel_afterburner(0), fuel_gravitydrive(0),
	weapons({0})
{
	// Create collision sphere
	const Real t_sphereRadius = 2.5F;
	collider = new CCapsuleCollider( t_sphereRadius*2, t_sphereRadius, true );

	// Create motion rigidbody
	rigidbody = new CRigidBody( collider, this, 100.0F );
	rigidbody->SetGravity(false);
	rigidbody->SetAngularDamping( 0.1F );
	rigidbody->SetLinearDamping( 0.1F );
	rigidbody->SetCollisionLayer( Layers::PHYS_CHARACTER );

	// Zoom particle shits
	particlesystem = new CParticleSystem("particlesystems/zoom_shit.pcf");
	particlesystem->transform.SetParent(&transform);
	particlesystem->transform.localPosition = Vector3d(0,0,0);
	{	// Set up the additional details
		particlesystem->GetEmitter()->vEmitterSize = Vector3d( 20,20,20 );
		particlesystem->GetEmitter()->rfStartSize.SetRange( 0.5F, 0.5F );
		particlesystem->GetEmitter()->rfEndSize.SetRange( 0.3F, 0.3F );
		// Set up the material
		glMaterial* particle_material = new glMaterial;
		particle_material->setTexture( 0, new CTexture("textures/white.jpg") );
		particle_material->m_diffuse = Color(1,1,1,0.5F);
		particle_material->passinfo.push_back( glPass() );
		particle_material->passinfo[0].shader = new glShader( ".res/shaders/particles/colorBlended.glsl" );
		particle_material->passinfo[0].m_blend_mode = Renderer::BM_ADD;
		particle_material->passinfo[0].b_depthmask = false;
		particle_material->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
		particle_material->passinfo[0].m_hint = RL_WORLD | RL_FOG;
		particlesystem->GetRenderable()->SetMaterial( particle_material );
		particle_material->removeReference();
	}

	ActiveCursor->SetVisible(false);

	weapons.autocannon_active = true;

	CScreenFade* fade = new CScreenFade ( true, 1.0F, 2.0F, Color(1,1,1,1) );
	fade->GetMaterial()->setTexture( 0, new CTexture("textures/system/cc.png") );
	// TODO: Delete the screeenfade later
}

CCloudPlayer::~CCloudPlayer ( void )
{
	delete_safe_decrement( rigidbody );
	delete_safe( collider );

	delete_safe_decrement( particlesystem );
	delete_safe( hudmesh );
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

	// Update the hud after the camera has moved
	hudUpdate();

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

	// Update weapons now
	weaponsNormalShip();
}
void CCloudPlayer::weaponsNormalShip ( void )
{
	weapons.autocannon_cd	-= Time::deltaTime;
	weapons.plasma_cd		-= Time::deltaTime;
	weapons.missile_cd		-= Time::deltaTime;

	weapons.autocannon_overheat	= std::max(0.0F, weapons.autocannon_overheat - Time::deltaTime * 5.0F );
	weapons.plasma_overheat		= std::max(0.0F, weapons.plasma_overheat - Time::deltaTime * 2.0F );
	if ( weapons.autocannon_overheat < 1.0F ) weapons.autocannon_overheated = false;
	else if ( weapons.autocannon_overheat > 50.0F )
	{
		if ( !weapons.autocannon_overheated )
		{
			CSoundBehavior* behavior = Audio.playSound( "Char.Wep.Overheat" );
			if ( behavior )
			{
				behavior->position = Vector3d(0,0,0);
				behavior->parent = &transform;
				behavior->Update();
				behavior->RemoveReference();
			}
			weapons.autocannon_overheated = true;
		}
	}
	if ( weapons.plasma_overheat < 1.0F ) weapons.plasma_overheated = false;
	else if ( weapons.plasma_overheat > 50.0F )
	{
		if ( !weapons.plasma_overheated )
		{
			CSoundBehavior* behavior = Audio.playSound( "Char.Wep.Overheat" );
			if ( behavior )
			{
				behavior->position = Vector3d(0,0,0);
				behavior->parent = &transform;
				behavior->Update();
				behavior->RemoveReference();
			}
			weapons.plasma_overheated = true;
		}
	}

	// Enable autocannon!
	if ( input->axes.secondary.pressed() || input->axes.tertiary.pressed() || input->axes.prone.pressed() )
	{
		if ( weapons.autocannon_active )
		{
			weapons.autocannon_active = false;
			weapons.plasma_active = true;
		}
		else if ( weapons.plasma_active )
		{
			weapons.plasma_active = false;
			weapons.missile_active = true;
		}
		else if ( weapons.missile_active )
		{
			weapons.missile_active = false;
			weapons.autocannon_active = true;
		}
	}

	// Fire ze weaponz!
	if ( input->axes.primary )
	{
		if ( weapons.autocannon_active )
		{
			if ( weapons.autocannon_cd <= 0.0F )
			{
				// 10 rounds/sec
				weapons.autocannon_cd = weapons.autocannon_overheated ? 0.7F : 0.1F;
				weapons.autocannon_overheat += 0.5F;
				// Fire autocannon!

				CProjectile* projectile;
				projectile = new ProjectileBullet( 
					Ray( transform.position + playerRotation * Vector3d(0,1,0), playerRotation * Vector3d(1,0,0) ), 1000.0F );
				projectile->SetOwner(this);
				projectile->RemoveReference();
				projectile = new ProjectileBullet( 
					Ray( transform.position - playerRotation * Vector3d(0,1,0), playerRotation * Vector3d(1,0,0) ), 1000.0F );
				projectile->SetOwner(this);
				projectile->RemoveReference();

				// Load in a sound, set lipsync with the feedback sound file name
				CSoundBehavior* behavior = Audio.playSound( "Char.Wep.Autocannon" );
				if ( behavior )
				{
					behavior->position = Vector3d(0,0,0);
					behavior->parent = &transform;
					behavior->Update();
					behavior->Play();
					behavior->RemoveReference();
				}

				shakeAmount += 0.2F;
			}
		}
		if ( weapons.plasma_active )
		{
			if ( weapons.plasma_cd <= 0.0F )
			{
				// 2 rounds/sec
				weapons.plasma_cd = weapons.plasma_overheated ? 2.0F : 0.5F; 
				weapons.plasma_overheat += 5.0F;
				// Fire plasma!

				shakeAmount += 1.0F;

				weapons.plasma_state = (weapons.plasma_state + 1) % 2;
				CProjectile* projectile;
				if ( weapons.plasma_state == 0 )
				{
					projectile = new ProjectilePlasma( 
						Ray( transform.position + playerRotation * Vector3d(0,1,0), playerRotation * Vector3d(1,0,0) ), 400.0F );
				}
				else
				{
					projectile = new ProjectilePlasma( 
						Ray( transform.position - playerRotation * Vector3d(0,1,0), playerRotation * Vector3d(1,0,0) ), 400.0F );
				}
				projectile->SetOwner(this);
				projectile->RemoveReference();
				
				// Load in a sound, set lipsync with the feedback sound file name
				CSoundBehavior* behavior = Audio.playSound( "Char.Wep.Plasma" );
				if ( behavior )
				{
					behavior->position = Vector3d(0,0,0);
					behavior->parent = &transform;
					behavior->Update();
					behavior->Play();
					behavior->RemoveReference();
				}
			}
		}
		if ( weapons.missile_active )
		{
			if ( weapons.missile_cd <= 0.0F )
			{
				// 5 rounds/sec
				weapons.missile_cd = 1 / 5.0F;

				// Fire missiles!
				shakeAmount += 0.1F;

				weapons.missile_state = (weapons.missile_state + 1) % 2;
				CProjectile* projectile;
				if ( weapons.missile_state == 0 )
				{
					projectile = new ProjectileMissile( 
						Ray( transform.position + playerRotation * Vector3d(0,1,0), playerRotation * Vector3d(1,0,0) ), 100.0F );
				}
				else
				{
					projectile = new ProjectileMissile( 
						Ray( transform.position - playerRotation * Vector3d(0,1,0), playerRotation * Vector3d(1,0,0) ), 100.0F );
				}
				projectile->SetOwner(this);
				projectile->RemoveReference();

				// Load in a sound, set lipsync with the feedback sound file name
				CSoundBehavior* behavior = Audio.playSound( "Char.Wep.Missile" );
				if ( behavior )
				{
					behavior->position = Vector3d(0,0,0);
					behavior->parent = &transform;
					behavior->Update();
					behavior->Play();
					behavior->RemoveReference();
				}
			}
		}
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

//	hudUpdate
// Updates hud shit
void CCloudPlayer::hudUpdate ( void )
{	// TODO: Move this to a CLogicObject renderer derivative so can push the mesh at a faster time
	return;

	// Create the hudmesh
	if ( hudmesh == NULL )
	{
		hudmesh = new CStreamedMesh();
		hudmesh->SetRenderType( Renderer::Foreground );
		{
			glMaterial* material = new glMaterial;
			material->setTexture( 0, new CTexture("textures/white.jpg") );
			material->m_diffuse = Color(1,0.5F,0.0F,1.0F);
			material->passinfo.push_back( glPass() );
			material->passinfo[0].shader = new glShader( ".res/shaders/particles/colorBlended.glsl" );
			material->passinfo[0].m_blend_mode = Renderer::BM_NORMAL;
			material->passinfo[0].b_depthmask = false;
			material->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
			material->passinfo[0].m_hint = RL_WORLD | RL_FOG;
			hudmesh->SetMaterial( material );
			material->removeReference();
		}

		CModelData* modeldata = hudmesh->GetModelData();
		modeldata->triangles = new CModelTriangle [2048];
		modeldata->vertices = new CModelVertex [2048];
	}

	CModelData* modeldata = hudmesh->GetModelData();
	// let's start simple
	for ( int i = 0; i < 10; ++i )
	{
		Vector3d points [3];

		points[0] = Vector3d( cos(i*2), 0, 0 );
		points[1] = Vector3d( 0, i, 0 );
		points[2] = Vector3d( 0, 0, cos(i*2) );

		// Push new triangle to the fuckin mesh
		for ( int v = 0; v < 3; ++v )
		{
			modeldata->vertices[i*3 + v] = {0};

			modeldata->vertices[i*3 + v].x = points[v].x;
			modeldata->vertices[i*3 + v].y = points[v].y;
			modeldata->vertices[i*3 + v].z = points[v].z;

			modeldata->vertices[i*3 + v].r = 1.0F;
			modeldata->vertices[i*3 + v].g = 1.0F;
			modeldata->vertices[i*3 + v].b = 1.0F;
			modeldata->vertices[i*3 + v].a = 1.0F;

			modeldata->triangles[i].vert[v] = i*3 + v;
		}
	}
	modeldata->vertexNum = 30;
	modeldata->triangleNum = 10;

	// Push the modified data to the GPU now
	hudmesh->StreamLockModelData();

	// Move the hudmesh in front of the camera
	hudmesh->transform.position = pCamera->transform.position + pCamera->transform.Forward() * 3.0F;
}