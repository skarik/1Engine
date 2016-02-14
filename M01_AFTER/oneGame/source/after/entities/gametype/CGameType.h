
#ifndef _C_GAME_TYPE_H_
#define _C_GAME_TYPE_H_

#include "engine/behavior/CGameBehavior.h"

class CWorldState;
class CEnvironmentEffects;
namespace NPC {
	class CZonedCharacterController;
	class CNpcSpawner;
}

class CGameType : public CGameBehavior
{
	ClassName( "CGameType" );
public:
	CGameType ( void )
		: CGameBehavior ()
	{
		;
	}
	CGameType ( CGameType* n_gametype )
		: CGameBehavior ()
	{
		if ( n_gametype )
		{
			m_worldstate			= n_gametype->m_worldstate;
			m_worldeffects			= n_gametype->m_worldeffects;
			m_charactercontroller	= n_gametype->m_charactercontroller;
			m_characterspawner		= n_gametype->m_characterspawner;
			delete n_gametype;
		}
	}
	~CGameType();

	void Update ( void )
	{
		;
	}

	bool bSinglePlayer;

	CWorldState*			m_worldstate;
	CEnvironmentEffects*	m_worldeffects;

	NPC::CZonedCharacterController*	m_charactercontroller;
	NPC::CNpcSpawner*				m_characterspawner;

};

#endif