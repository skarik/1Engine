
#ifndef _C_GAME_MESSENGER_H_
#define _C_GAME_MESSENGER_H_

#include "GameMessages.h"
#include <string>

class CGameState;

class CGameMessenger
{
private:
	friend CGameState;
	explicit CGameMessenger ( CGameState* targetGamestate );

public:
	// Define signale
	typedef uint64_t signal_t;

	// Sends signal to all objects with matching classname
	ENGINE_API void Send ( const std::string& target, signal_t signal );
	// Sends signal to all objects
	ENGINE_API void SendGlobal ( signal_t signal );
	// Stops on first object with matched classname
	ENGINE_API void SendFirstUnique ( const std::string& target, signal_t signal );
	// Stops on first object with matched name
	ENGINE_API void SendTo ( const std::string& name, signal_t signal );


private:
	CGameState* m_gamestate;
};

#endif//_C_GAME_MESSENGER_H_