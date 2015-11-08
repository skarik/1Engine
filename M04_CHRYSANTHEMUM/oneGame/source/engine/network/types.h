//===============================================================================================//
//		network/types.h
// Definition of structs, constants, and types specific to the networking system
//===============================================================================================//
#ifndef _ENGINE_NETWORK_TYPE_H_
#define _ENGINE_NETWORK_TYPE_H_

#include "core/types.h"
#include <string>

using std::string;

namespace Network
{
	//===============================================================================================//
	// CONSTANTS
	//===============================================================================================//

	enum eGameIdInfo
	{
		NET_GAMEID		= 01,
	};

	enum ePortConstants 
	{
		PORT_DEFAULT	= 27015,
		PORT_GAMEINFO	= 64322,
	};

	//===============================================================================================//
	// STRUCT DEFINITIONS
	//===============================================================================================//

	//		struct netgameinfo_t
	// Stores gameinfo. Is what is returned when a possible client requests game information.
	struct netgameinfo_t
	{
		uint32_t	size;
		uint16_t	gameid;
		uint16_t	mainport;
		uint16_t	maxplayers;
		uint16_t	players;
		char		ip [24];
	};

	//		struct netplayer_t
	// Stores the player net information.
	struct netplayer_t
	{
		uint16_t	id;
		uint16_t	ping;
		string		address;
		uint16_t	port;
	};
	
};

#endif//_ENGINE_NETWORK_TYPE_H_