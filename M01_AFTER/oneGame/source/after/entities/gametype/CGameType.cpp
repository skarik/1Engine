
#include "CGameType.h"

#include "after/states/CWorldState.h"
#include "after/entities/world/environment/CEnvironmentEffects.h"
#include "after/entities/character/npc/zoned/CZonedCharacterController.h"
#include "after/entities/world/CNpcSpawner.h"

CGameType::~CGameType ( void )
{
	delete_safe(m_worldstate);
	delete_safe_decrement(m_worldeffects);
	delete_safe_decrement(m_charactercontroller);
	delete_safe_decrement(m_characterspawner);
}