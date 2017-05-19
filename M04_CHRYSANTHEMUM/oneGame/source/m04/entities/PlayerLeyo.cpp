
#include "PlayerLeyo.h"

#include "core/math/Math.h"
#include "core/math/Easing.h"
#include "core-ext/input/CInputControl.h"
#include "renderer/light/CLight.h"
#include "render2d/camera/COrthoCamera.h"
#include "render2d/object/sprite/CEditableRenderable2D.h"

#include "engine/physics/collider/types/CBoxCollider.h"
#include "engine/physics/motion/CRigidbody.h"

#include "m04/entities/UILuvPpl.h"

using namespace M04;

DECLARE_OBJECT_REGISTRAR(player_leyo,M04::PlayerLeyo);

PlayerLeyo* PlayerLeyo::active = NULL;

PlayerLeyo::PlayerLeyo ( void )
	: CGameBehavior(), Engine2D::SpriteContainer(&position, NULL, &flipstate)
	//: CGameBehavior(), Engine2D::AnimationContainer(&position, NULL, &flipstate)
{
	active = this;

	input = new CInputControl(this);
	input->Capture();

	camera = new COrthoCamera();
	// Set camera options
	camera->pixel_scale_mode = orthographicScaleMode_t::ORTHOSCALE_MODE_SIMPLE;
	camera->viewport_target.size = Vector2d( 1280,720 ) * 0.5f;
	camera->render_scale = 0.5F;
	camera->SetActive(); // Mark it as the main camera to use IMMEDIATELY
	// Start camera in follow mode
	camera_mode = 1;
	camera_lerp_mode = 1.0F;

	SetupDepthOffset( -1.0F, 0.0F );
	m_sprite->SpriteGenParams().normal_default = Vector3d(0, 2.0F, 1.0F).normal();
	m_sprite->SetSpriteFile("sprites/leo.gal");
	m_spriteOrigin = Vector2i( m_sprite->GetSpriteInfo().fullsize.x / 2, m_sprite->GetSpriteInfo().fullsize.y - 8 );
	
	//this->AddFromFile(Animation::TYPE_IDLE, 0, "sprites/tests/demon-run.gal");
	//this->AddFromFile(Animation::TYPE_IDLE, 0, "sprites/leo.gal");
	//m_spriteOrigin = Vector2i( m_spriteSize.x / 2, m_spriteSize.y );

	light = new CLight;
	light->diffuseColor = Color(0.4,0.4,0.4) * 0.0F;
	light->range = 128;

	bod = NULL;

	flipstate = Vector3d(1,1,1);

	ui = new UILuvPpl();
}

PlayerLeyo::~PlayerLeyo ( void )
{
	delete_safe(input);

	delete_safe(camera);
	delete_safe(light);

	delete_safe_decrement(bod);

	delete_safe_decrement(ui);
}

void PlayerLeyo::Update ( void )
{
	if ( bod == NULL )
	{
		auto box = new CBoxCollider( Vector2d( 24,16 ) );
		bod = new CRigidbody(box, this);
		bod->target_position = &position;
		bod->SetPosition(position);
	}

	input->Update(this, Time::deltaTime);

	Vector3d motion_input (-input->vDirInput.y, -input->vDirInput.x, 0);

	const float acceleration = 512.0F;
	const float maxSpeed = 128.0F;

	// Do motion
	for ( int i = 0; i < 2; ++i )
	{
		float target = motion_input[i] * maxSpeed;
		float distance = target - velocity[i];
		float delta = math::sgn(distance) * acceleration * Time::deltaTime;

		if ( fabs(delta) > fabs(distance) )
		{
			velocity[i] = target;
		}
		else
		{
			velocity[i] += delta;
		}
	}

	bod->SetVelocity( velocity );

	// Move around
	//position += velocity * Time::deltaTime;
	position.z = -4.5F;
}

void PlayerLeyo::PostFixedUpdate ( void )
{
	CameraUpdate();

	// Update sprite
	if ( math::sgn( velocity.x ) != 0 )
	{
		flipstate.x = (Real)math::sgn( velocity.x );
	}

	// Update camera position
	camera->transform.position.x = (Real)math::round(camera_position.x);
	camera->transform.position.y = (Real)math::round(camera_position.y);

	light->position.x = (Real)math::round(position.x);
	light->position.y = (Real)math::round(position.y + 32);

	// Update UI
	ui->transform.world.position = camera->transform.position;
	ui->transform.world.position.z = -101;
}

void PlayerLeyo::CameraUpdate ( void )
{
	// Create limited camera position
	Vector3d cam_pos_limited = position + Vector3d(0, -16, 0);
	cam_pos_limited.x = math::clamp<Real>(cam_pos_limited.x, camera->ortho_size.x * 0.5F, 100000);
	cam_pos_limited.y = math::clamp<Real>(cam_pos_limited.y, camera->ortho_size.y * 0.5F, 100000);

	Real camera_delta = (cam_pos_limited - camera_position).magnitude();
	
	// Do mode check
	if ( camera_mode == 0 )
	{
		// Camera is stuck! Check if it wants to move...
		if ( camera_delta > 64.0F )
		{	// Unstuck it.
			camera_mode = 1;
		}
	}
	else
	{
		// Check camera stuck time
		if ( camera_delta < 1.2F )
		{
			camera_stuck_time += Time::deltaTime;
		}
		else
		{
			camera_stuck_time = 0;
		}
		// Has it been stuck for a bit?
		if ( camera_stuck_time > 1.4F )
		{	// Camera hasn't moved too much for a few seconds, so go to stuck mode.
			camera_mode = 0;
			camera_lockposition = camera_position;
		}
	}

	// Blend camera modes in and out
	if ( camera_mode == 0 )
	{
		camera_lerp_mode -= Time::deltaTime * 1.0F;
	}
	else
	{
		camera_lerp_mode += Time::deltaTime * 1.0F;
	}
	camera_lerp_mode = math::saturate(camera_lerp_mode);

	// Lerp for final camera position
	camera_position = camera_lockposition.lerp(cam_pos_limited, easing::cubic_inout(camera_lerp_mode));
}