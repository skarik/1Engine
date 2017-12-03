#include "PlayerPlatformerBase.h"

#include "core/math/Math.h"
#include "core/math/Easing.h"
#include "core-ext/input/CInputControl.h"
#include "renderer/light/CLight.h"
#include "render2d/camera/COrthoCamera.h"
#include "render2d/object/sprite/CEditableRenderable2D.h"

#include "physical/interface/tobt.h"
#include "physical/physics/shapes/PrShapeBox.h"
#include "engine/physics/motion/CRigidbody.h"

#include "ld40/interfaces/CameraControllerPlatformer.h"
#include "ld40/interfaces/CharacterControllerPlatformer.h"

using namespace M04;

DECLARE_OBJECT_REGISTRAR(player_platformer_base,M04::PlayerPlatformerBase);

PlayerPlatformerBase::PlayerPlatformerBase ( void )
	: CGameBehavior()//, Engine2D::SpriteContainer(&position, NULL, &flipstate)
	//: CGameBehavior(), Engine2D::AnimationContainer(&position, NULL, &flipstate)
{
	// Create the input
	{
		input = new CInputControl(this);
		input->Capture();
	}

	// Set up the camera
	{
		COrthoCamera* cam = new COrthoCamera();
		// Set camera options
		cam->pixel_scale_mode = orthographicScaleMode_t::ORTHOSCALE_MODE_SIMPLE;
		cam->viewport_target.size = Vector2d( 1280,720 ) * 0.5f;
		cam->render_scale = 0.5F;

		// Create camera controller:
		camera = new CameraControllerPlatformer(this);
		camera->m_camera = cam; // Camera now belongs to the controller
		camera->m_tracked_position = &position;
		camera->m_tracked_velocity = &velocity;
	}
}

void PlayerPlatformerBase::OnCreate ( void )
{
	// hack fix of editor issue
	position.z = 0;

	// Set the character controller
	{
		motion = new CharacterControllerPlatformer(this);
		motion->m_tracked_position = &position;
		motion->m_tracked_velocity = &velocity;
		motion->m_input = input;
		motion->InitCollision( Vector2f(32,64) );

		// Set up Withstand's motion parameters as a baseline.
		grPlatformControllerOptions opt;
		opt.gravity = 720;
		opt.runSpeed = 120 * 1.5F;
		opt.runAcceleration = 720 * 1.3F;
		opt.runDecceleration = 1440 * 1.3F;
		opt.airAcceleration = 720;
		opt.airDecceleration = 180;
		opt.jumpSpeed = 278;
		motion->m_opt = opt;
	}
}

PlayerPlatformerBase::~PlayerPlatformerBase ( void )
{
	delete_safe(input);
	delete_safe(camera);
	delete_safe(motion);
}

void PlayerPlatformerBase::Update ( void )
{	
	motion->Step();
}

void PlayerPlatformerBase::FixedUpdate ( void )
{
	motion->PhysicsStep();
}

void PlayerPlatformerBase::PostFixedUpdate ( void )
{
	camera->Step();
}