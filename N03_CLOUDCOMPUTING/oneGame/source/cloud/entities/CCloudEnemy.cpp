#include "CCloudEnemy.h"
// Includes
#include "core/math/random/Random.h"

#include "core-ext/input/CInputControl.h"

#include "engine/audio/CAudioInterface.h"

#include "engine/physics/motion/CRigidbody.h"
#include "engine/physics/collider/types/CCapsuleCollider.h"

//#include "renderer/camera/CCamera.h"
#include "renderer/logic/model/CModel.h"
//#include "renderer/logic/model/CInstancedModel.h"
#include "renderer/object/mesh/CInstancedMesh.h"

#include "renderer/material/glMaterial.h"
#include "renderer/texture/CTexture.h"

#include "engine-common/entities/CParticleSystem.h"
#include "renderer/object/particle/CParticleRenderer_Animated.h"

#include "cloud/entities/projectiles/ProjectileBullet.h"



CModel*				CCloudEnemy::model	= NULL;
CInstancedMesh*		CCloudEnemy::mesh	= NULL;
std::vector<CCloudEnemy*>	CCloudEnemy::manifest;

CCloudEnemy::CCloudEnemy(void)
	: CActor(), health(100), guncooldown(3.0)
{
	// Create collision sphere
	const Real t_sphereRadius = 2.5F;
	collider = new CCapsuleCollider(t_sphereRadius * 2, t_sphereRadius, true);

	// Create motion rigidbody
	rigidbody = new CRigidBody(collider, this, 100.0F);
	rigidbody->SetGravity(false);
	rigidbody->SetAngularDamping( 0.1F );
	rigidbody->SetLinearDamping( 0.1F );
	rigidbody->SetCollisionLayer( Layers::PHYS_CHARACTER );

	// Create model for the bot.
	//model = new CModel( "models/geosphere.fbx" );
	if ( model == NULL )
	{
		model = new CModel( "models/enemy/bot_zero.fbx" );
		model->transform.scale = Vector3d(1,1,1) * t_sphereRadius * 2.54F;// / 120.0F;
		model->SetVisibility(false);
		model->GetMesh(uint(0))->m_glMesh->RecalculateNormals();
	}
	if ( mesh == NULL )
	{
		mesh = new CInstancedMesh( model->GetMesh(uint(0))->m_glMesh );
		mesh->transform.scale = model->transform.scale;

		glMaterial* material = new glMaterial;
		material->setTexture( 0, new CTexture("textures/white.jpg") );
		material->m_diffuse = Color(1,0.5F,0.0F,1.0F);
		material->m_isInstancedShader = true;
		material->passinfo.push_back( glPass() );
		material->passinfo[0].shader = new glShader( ".res/shaders/particles/colorBlended.instanced.glsl" );
		/*material->passinfo[0].m_blend_mode = Renderer::BM_NORMAL;
		material->passinfo[0].b_depthmask = true;
		material->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_ALPHATEST;
		material->passinfo[0].m_hint = RL_WORLD | RL_FOG;*/
		material->deferredinfo.push_back( glPass_Deferred() );
		mesh->SetMaterial( material );
		material->removeReference();

	}
	manifest.push_back(this);
	manifest_id = manifest.size() - 1;
}

CCloudEnemy::~CCloudEnemy(void)
{
	delete_safe_decrement(rigidbody);
	delete_safe(collider);

	//delete_safe(model);

	// Remove this from the manifest
	auto manifest_spot = std::find( manifest.begin(), manifest.end(), this );
	if ( manifest_spot != manifest.end() )
	{
		manifest.erase(manifest_spot);
		// Update the manifest ID's so the instancing can still work
		for ( uint i = 0; i < manifest.size(); ++i )
		{
			manifest[i]->manifest_id = i;
		}
	}
}

// Game step

void CCloudEnemy::Update(void)
{
	model->transform.position = Vector3d::zero;
	model->transform.localPosition = Vector3d::zero;
	mesh->transform.position = Vector3d::zero;
	mesh->transform.localPosition = Vector3d::zero;

	// Update base class code first
	CActor::Update();
	guncooldown -= Time::deltaTime;
}

void CCloudEnemy::LateUpdate(void)
{
	//GrabInput();

	//camShipFirstPerson();

	//model->transform.position = transform.position;
	//model->transform.rotation = transform.rotation;
	if ( mesh != NULL )
	{
		mesh->SetInstanceCount( manifest.size() );
		mesh->SetInstancePosition( manifest_id, transform.position );
		mesh->SetInstanceRotation( manifest_id, transform.rotation );
	}

	rigidbody->Wake();
}

void CCloudEnemy::OnDamaged ( Damage const& damage, DamageFeedback* )
{
	//rigidbody->ApplyForce( ((transform.position - damage.source).normal()*2 + Random.PointInUnitSphere()) * damage.amount * 1000.0F );
	Vector3d hitforce = ((transform.position - damage.source).normal()*2 + Random.PointInUnitSphere()) * damage.amount;
	rigidbody->SetVelocity( rigidbody->GetVelocity() + hitforce * 0.3F );

	health -= damage.amount;
	if ( health <= 0 )
	{
		OnDeath( damage );
	}
}
void CCloudEnemy::OnDeath ( Damage const& )
{
	CParticleSystem* ps = new CParticleSystem("particlesystems/sparkexplo.pcf");

	CParticleRenderer_Animated* renderer = (CParticleRenderer_Animated*)ps->GetRenderable();
	if ( renderer )
	{
		renderer->GetMaterial()->loadFromFile("particle/sparks");
		renderer->GetMaterial()->passinfo[0].m_hint = RL_WORLD | RL_FOG;
		renderer->GetMaterial()->passinfo[0].b_depthmask = false;

		// following is a hack. currently these values are not serialized properly (corrupted) so set them here to fix
		renderer->fFramesPerSecond = 30.0F;
		renderer->bStretchAnimationToLifetime = true;
		renderer->bClampToFrameCount = true;
		renderer->iFrameCount = 16;
		renderer->iHorizontalDivs = 4;
		renderer->iVerticalDivs = 4;
	}
	ps->transform.position = transform.position;
	ps->bAutoDestroy = true;
	ps->PostUpdate();
	ps->RemoveReference();

	// play sound
	CSoundBehavior* behavior = Audio.playSound( "Char.Wep.Explo" );
	if ( behavior )
	{
		behavior->position = Vector3d(0,0,0);
		behavior->parent = &transform;
		behavior->Update();
		behavior->RemoveReference();
	}

	DeleteObject(this);
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
	/*enemyRotation *= Rotator(Vector3d(0, -vTurnInput.y, vTurnInput.x));
	vTurnInput.x = 0;
	vTurnInput.y = 0;*/

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

	// Rotate the move vector to match the enemy
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

//Take a 3D vector and use it for turning
void CCloudEnemy::SetTurnInput(Vector3d turn)
{
	vTurnInput.x = turn.x;
	vTurnInput.y = turn.y;
	vTurnInput.z = turn.z;
}

//Take a 3D vector and use it for movement
void CCloudEnemy::SetDirInput(Vector3d dir)
{
	vDirInput.x = dir.x;
	vDirInput.y = dir.y;
	vDirInput.z = dir.z;
}

//Set the flags for crouch, jump, menuToggle, and sprint, to mirror the player
void CCloudEnemy::SetVAxes(int flags)
{
	(flags & 1) ? input.crouch = true		: input.crouch = false;
	
	(flags & 2)	? input.jump = true			: input.jump = false;
	
	(flags & 4)	? input.menuToggle = true	: input.menuToggle = false;

	(flags & 8)	? input.sprint = true		: input.sprint = false;
}

void CCloudEnemy::SetRotation(Rotator rot)
{
	transform.rotation = rot * transform.rotation;
	enemyRotation = transform.rotation;
}

Vector3d CCloudEnemy::GetVelocity()
{
	if (rigidbody != NULL)
		return rigidbody->GetVelocity();
	else
		return Vector3d(0,0,0);
}

void CCloudEnemy::SetVelocity(Vector3d velocity)
{
	if (rigidbody != NULL)
		rigidbody->SetVelocity(velocity);
}

bool CCloudEnemy::FireGun(void)
{
	if (guncooldown <= 0.0)
	{
		CProjectile* projectile;
		projectile = new ProjectileBullet(
			Ray(transform.position + enemyRotation * Vector3d(0, 1, 0), enemyRotation * Vector3d(1, 0, 0)), 1000.0F);
		projectile->SetOwner(this);
		projectile->RemoveReference();
		guncooldown = 3.0;
		return true;
	}
	return false;
}
