
#include "NpcPenni.h"

#include "core/math/Math.h"
#include "core-ext/input/emulated/CEmulatedInputControl.h"
#include "render2d/object/sprite/CStreamedRenderable2D.h"
#include "render2d/object/CTextMesh.h"

using namespace M04;

DECLARE_OBJECT_REGISTRAR(npc_rex,M04::NpcPenni);

NpcPenni::NpcPenni ( void )
	: NpcBase()
{
	m_sprite->SpriteGenParams().normal_default = Vector3d(0, 1.0F, 1.0F).normal();
	m_sprite->SetSpriteFile("sprites/penni.gal");
	m_spriteOrigin = m_sprite->GetSpriteInfo().fullsize / 2;

	test_text = new CTextMesh();
	test_text->SetFont("ComicNeue-Angular-Bold.ttf", 12, FW_BOLD);
	test_text->m_text = "Hella!";
	test_text->UpdateText();
}

NpcPenni::~NpcPenni ( void )
{

}

void NpcPenni::Update ( void )
{
	test_text->transform.position = position;
	test_text->transform.position.y -= 36.0F;
	test_text->transform.position.x -= 20.0F;

	input->Update(Time::deltaTime);

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
}