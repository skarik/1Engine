
#include "PlayerLeyo.h"

#include "core/math/Math.h"
#include "core-ext/input/CInputControl.h"
#include "renderer/light/CLight.h"
#include "render2d/camera/COrthoCamera.h"
#include "render2d/object/sprite/CStreamedRenderable2D.h"

#include "engine/physics/collider/types/CBoxCollider.h"
#include "engine/physics/motion/CRigidbody.h"

using namespace M04;

DECLARE_OBJECT_REGISTRAR(player_leyo,M04::PlayerLeyo);

PlayerLeyo* PlayerLeyo::active = NULL;

PlayerLeyo::PlayerLeyo ( void )
	: CGameBehavior(), Engine2D::SpriteContainer(&position)
{
	active = this;

	input = new CInputControl(this);
	input->Capture();

	camera = new COrthoCamera();
	// Set camera options
	camera->pixel_scale_mode = orthographicScaleMode_t::ORTHOSCALE_MODE_SIMPLE;
	camera->viewport_target.size = Vector2d( 1280,720 ) * 0.5f;
	camera->SetActive(); // Mark it as the main camera to use IMMEDIATELY

	m_sprite->SpriteGenParams().normal_default = Vector3d(0, 2.0F, 1.0F).normal();
	m_sprite->SetSpriteFile("sprites/leo.gal");
	m_spriteOrigin = Vector2i( m_sprite->GetSpriteInfo().fullsize.x / 2, m_sprite->GetSpriteInfo().fullsize.y - 8 );

	light = new CLight;
	light->diffuseColor = Color(0.4,0.4,0.4) * 0.0F;
	light->range = 128;

	auto box = new CBoxCollider( Vector2d( 24,16 ) );
	bod = new CRigidbody(box, this);
	bod->target_position = &position;
}

PlayerLeyo::~PlayerLeyo ( void )
{
	delete_safe(input);

	delete_safe(camera);
	delete_safe(light);

	delete_safe_decrement(bod);
}

void PlayerLeyo::Update ( void )
{
	input->Update(this, Time::deltaTime);

	Vector3d motion_input (-input->vDirInput.y, -input->vDirInput.x, 0);

	const float acceleration = 512.0F;
	const float maxSpeed = 128.0F;

	// Do motion
	for ( int i = 0; i < 2; ++i )
	{
		float target = motion_input[i] * maxSpeed;
		float distance = target - velocity[i];
		float delta = Math::sgn(distance) * acceleration * Time::deltaTime;

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
	// Update camera position
	camera->transform.position.x = (Real)Math::round(position.x);
	camera->transform.position.y = (Real)Math::round(position.y - 16);

	light->transform.position.x = (Real)Math::round(position.x);
	light->transform.position.y = (Real)Math::round(position.y + 32);
}