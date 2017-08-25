//===============================================================================================//
//		server.h
// Server-side code
//===============================================================================================//
#ifndef _ENGINE_SERVER_H_
#define _ENGINE_SERVER_H_

#include "core/types.h"
#include "engine/network/types.h"
#include <vector>

//		IsServer()
// Returns if the current system is a server.
// Result true for both default singleplayer mode and multiplayer servers.
ENGINE_API	bool IsServer ( void );

namespace Network
{
	//		Host ()
	// Hosts server. Does not assume a player to connect.
	ENGINE_API	int Host ( uint16_t	mainPort=PORT_DEFAULT );
	
	//		GetPlayers()
	// Returns list of active players in the game. Should never be empty
	ENGINE_API	std::vector<Network::netplayer_t> GetPlayers ( void );
};

#endif//_ENGINE_SERVER_H_