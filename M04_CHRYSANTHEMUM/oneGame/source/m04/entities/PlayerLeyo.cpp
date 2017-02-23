
#include "PlayerLeyo.h"

#include "core/math/Math.h"
#include "core-ext/input/CInputControl.h"
#include "renderer/light/CLight.h"
#include "render2d/camera/COrthoCamera.h"
#include "render2d/object/sprite/CStreamedRenderable2D.h"

using namespace M04;

DECLARE_OBJECT_REGISTRAR(player_leyo,M04::PlayerLeyo);

PlayerLeyo::PlayerLeyo ( void )
	: CGameBehavior(), Engine2D::SpriteContainer(&position)
{
	input = new CInputControl(this);
	input->Capture();

	camera = new COrthoCamera();
	// Set camera options
	camera->pixel_scale_mode = orthographicScaleMode_t::ORTHOSCALE_MODE_SIMPLE;
	camera->viewport_target.size = Vector2d( 1280,720 ) * 0.5f;
	camera->SetActive(); // Mark it as the main camera to use IMMEDIATELY

	m_sprite->SpriteGenParams().normal_default = Vector3d(0, 1.0F, 1.0F).normal();
	m_sprite->SetSpriteFile("sprites/leo.gal");
	m_spriteOrigin = m_sprite->GetSpriteInfo().fullsize / 2;

	light = new CLight;
	light->diffuseColor = Color(0.4,0.4,0.4);
	light->range = 128;
}

PlayerLeyo::~PlayerLeyo ( void )
{
	delete_safe(input);
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
		float delta = Math.sgn(distance) * acceleration * Time::deltaTime;

		if ( fabs(delta) > fabs(distance) )
		{
			velocity[i] = target;
		}
		else
		{
			velocity[i] += delta;
		}
	}

	// Move around
	position += velocity * Time::deltaTime;

	// Update camera position
	camera->transform.position.x = (Real)Math.Round(position.x);
	camera->transform.position.y = (Real)Math.Round(position.y - 16);

	light->transform.position.x = (Real)Math.Round(position.x);
	light->transform.position.y = (Real)Math.Round(position.y + 32);
}