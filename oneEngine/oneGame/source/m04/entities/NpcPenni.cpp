
#include "NpcPenni.h"

#include "core/math/Math.h"
#include "core-ext/input/emulated/CEmulatedInputControl.h"
#include "render2d/object/sprite/CEditableRenderable2D.h"
#include "m04/entities/TalkerBox.h"

#include "m04/entities/PlayerLeyo.h"

#include "physical/interface/tobt.h"
#include "physical/physics/shapes/PrShapeBox.h"
#include "engine/physics/motion/CRigidbody.h"

using namespace M04;

DECLARE_OBJECT_REGISTRAR(npc_rex,M04::NpcPenni);

NpcPenni::NpcPenni ( void )
	: NpcBase()
{
	SetupDepthOffset( -1.0F, 0.0F );
	m_sprite->SpriteGenParams().normal_default = Vector3d(0, 2.0F, 1.0F).normal();
	m_sprite->SetSpriteFile("sprites/penni.gal");
	m_spriteOrigin = Vector2i( m_sprite->GetSpriteInfo().fullsize.x / 2, m_sprite->GetSpriteInfo().fullsize.y - 8 );

	talker = NULL;

	bod = NULL;
}

NpcPenni::~NpcPenni ( void )
{
	delete_safe_decrement(talker);

	delete_safe_decrement(bod);
}

void NpcPenni::Update ( void )
{
	// Draw 4 pixels above the ground
	position.z = -4.5F;

	if ( bod == NULL )
	{
		PrShape* box = new PrShapeBox( Vector2f(24, 16) ); 

		prRigidbodyCreateParams params = {0};
		params.shape = box;
		params.owner = this;
		params.ownerType = core::kBasetypeGameBehavior;
		params.group = physical::layer::PHYS_CHARACTER;
		params.mass = 30.0F;

		bod = new CRigidbody(params);
		bod->target_position = &position;

		bod->world_transform.position = position;
		bod->PushTransform();
		bod->ApiBody()->setActivationState(DISABLE_DEACTIVATION);
	}

	if (talker != NULL)
	{
		talker->position = position;
		talker->position.y -= 128.0F;
		talker->position.x -= 64.0F;
		talker->text = "It's scientifically proven that larger textboxes are less likely to be read!";

		if ( (PlayerLeyo::active->position - position).magnitude() > 150 )
		{
			talker->Hide();
			if (talker->IsHidden())
			{
				delete_safe_decrement(talker);
			}
		}
	}
	else
	{
		if ( (PlayerLeyo::active->position - position).magnitude() < 100 )
		{
			talker = new TalkerBox();
		}
	}

	input->Update(Time::deltaTime);

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

	//bod->ApiBody()->setLinearVelocity( physical::bt(velocity) );

	// Move around
	//position += velocity * Time::deltaTime;
	position.z = -4.5F;
}