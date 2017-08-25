
#ifndef _C_LUA_BEHAVIOR_
#define _C_LUA_BEHAVIOR_

#include "core/containers/arstring.h"
#include "engine/behavior/CGameBehavior.h"
#include "clua.h"

class CLuaBehavior : public CGameBehavior
{

public:
	explicit		CLuaBehavior ( const char* entityName, const char* packageName="" );
					~CLuaBehavior ( void );
					
	// Simple Callbacks
	void			OnCreate ( void ) override;
	void			OnDestroy ( void ) override;

	// Game step
	void			Update ( void ) override;
	void			LateUpdate ( void ) override;
	void			PostUpdate ( void ) override;

	// Physics step
	void			FixedUpdate ( void ) override;

	// Messenger system
	void			OnReceiveSignal ( const uint64_t signal ) override;

protected:
	// Load up Lua object
	void			Initialize ( void );
	void			Cleanup ( void );

protected:
	arstring<128>	m_entityName;	// Lua entity folder
	arstring<96>	m_packageName;	// Mod directory or otherwise
	arstring<30>	m_environment;	// Object environment
	
	enum eEnableKey_t
	{
		EK_OnCreate			= 1,
		EK_OnDestroy		= 2,
		EK_Update			= 4,
		EK_LateUpdate		= 8,
		EK_PostUpdate		= 16,
		EK_FixedUpdate		= 32,
		EK_OnReceiveSignal	= 64
	};
	uint16_t		m_enableKey;
};

#endif