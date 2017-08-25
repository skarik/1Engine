
#include "gametypeMenu.h"
#include "after/states/CWorldState.h"
#include "after/entities/world/environment/CEnvironmentEffects.h"
#include "after/entities/character/npc/zoned/CZonedCharacterController.h"
#include "after/entities/world/CNpcSpawner.h"

gametypeMenu::gametypeMenu ( void )
	: CGameType()
{
	m_charactercontroller->active = false;
	m_characterspawner->active = false;

	m_worldeffects->active = false;
}

gametypeMenu::gametypeMenu ( CGameType* n_gametype )
	: CGameType( n_gametype )
{
	m_charactercontroller->active = false;
	m_characterspawner->active = false;

	m_worldeffects->active = false;
}

gametypeMenu::~gametypeMenu ( void )
{
	;
}

void gametypeMenu::Update ( void )
{
	/*if ( bTickWorld ) {
		m_worldstate->
		m_worldstate->Tick();
	}*/
	/*
	m_charactercontroller->active = false;
	m_characterspawner->active = false;

	m_worldeffects->active = false;*/
}

void gametypeMenu::InitWorld ( void )
{
	m_worldstate->Load();

	m_characterspawner->active = true;
	m_worldeffects->active = true;

	m_charactercontroller->active = true;
	m_charactercontroller->ReadyUp();
}
void gametypeMenu::StopWorld ( void )
{
	m_charactercontroller->active = false;
	m_characterspawner->active = false;

	m_worldeffects->active = false;
}