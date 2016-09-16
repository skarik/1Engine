#include "DebuffDisable.h"

#include "after/entities/character/CCharacter.h"

#include "renderer/state/Settings.h"
#include "after/states/CWorldState.h"

#include "after/entities/world/environment/DayAndNightCycle.h"
#include "engine-common/entities/CRendererHolder.h"
#include "renderer/object/screenshader/effects/CBloomShader.h"

#include "core/math/Math.h"

DebuffDisable::DebuffDisable ( CCharacter* inTarget ) 
	: CCharacterBuff(TYPE_ACTIVE, inTarget)
{
	
}

DebuffDisable::~DebuffDisable(void)
{
	;
}

void DebuffDisable::Update ( void )
{
	//Update duration
}
