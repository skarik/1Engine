//===============================================================================================//
//		client.h
// Client-side code
//===============================================================================================//
#ifndef _ENGINE_CLIENT_H_
#define _ENGINE_CLIENT_H_

#include "core/types.h"
#include "engine/network/types.h"

//		IsClient()
// Returns if the current system is a client.
// Result true for both default singleplayer mode and multiplayer clients
ENGINE_API	bool IsClient ( void );

namespace Network
{
	//		Join ()
	// Joins a server
	ENGINE_API	int	Join ( const char* address, uint16_t port=PORT_DEFAULT );

	//		GetServerInfo ()
	// Pings a server for info
	// Is a blocking call, so potentially should be put into another thread and have its result waited on.
	ENGINE_API	Network::netgameinfo_t GetServerInfo ( const char* address );
};

#endif//_ENGINE_CLIENT_H_