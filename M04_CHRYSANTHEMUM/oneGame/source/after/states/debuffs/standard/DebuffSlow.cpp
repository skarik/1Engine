#include "DebuffSlow.h"

#include "after/entities/character/CCharacter.h"

#include "renderer/state/Settings.h"
#include "after/states/CWorldState.h"

#include "after/entities/world/environment/DayAndNightCycle.h"
#include "engine-common/entities/CRendererHolder.h"
#include "renderer/object/screenshader/effects/CBloomShader.h"

#include "core/math/Math.h"

DebuffSlow::DebuffSlow(CCharacter* inTarget) 
	: CCharacterBuff( TYPE_ACTIVE, inTarget)
{
		positive = 0;
}

DebuffSlow::~DebuffSlow()
{
	;
}

void DebuffSlow::Update()
{
	//Update the remaining duration
	//Check if the slow needs to be updated.


	
}