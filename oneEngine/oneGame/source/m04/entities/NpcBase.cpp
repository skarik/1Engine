
#include "NpcBase.h"

#include "core/math/Math.h"
#include "core-ext/input/emulated/CEmulatedInputControl.h"
#include "render2d/object/sprite/CEditableRenderable2D.h"

using namespace M04;

NpcBase::NpcBase ( void )
	: CGameBehavior(), Engine2D::SpriteContainer(&position)
{
	input = new CEmulatedInputControl();
	SetupDepthOffset( -1.0F, 0.0F );
}

NpcBase::~NpcBase ( void )
{
	delete_safe(input);
}

void NpcBase::Update ( void )
{
	input->Update(Time::deltaTime);
	position.z = -4.5F;
}