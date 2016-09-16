#include "DebuffDPS.h"

#include "after/entities/character/CCharacter.h"

#include "renderer/state/Settings.h"
#include "after/states/CWorldState.h"

#include "after/entities/world/environment/DayAndNightCycle.h"
#include "engine-common/entities/CRendererHolder.h"
#include "renderer/object/screenshader/effects/CBloomShader.h"

#include "core/math/Math.h"

DebuffDPS::DebuffDPS(CCharacter* inTarget) 
	: CCharacterBuff( TYPE_ACTIVE, inTarget)
{
	
}

DebuffDPS::~DebuffDPS(void)
{
	;
}

void DebuffDPS::Update(void)
{
	//Calculate remaining time
}
