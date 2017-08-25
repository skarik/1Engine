//===============================================================================================//
//		playerlist.h
// Provides method to get list of player characters
//===============================================================================================//
#ifndef _ENGINE_COMMON_NETWORK_PLAYERLISTING_H_
#define _ENGINE_COMMON_NETWORK_PLAYERLISTING_H_

#include "engine/types.h"
#include "engine/network/GameNetworking.h"
//#include "engine/entities/CActor.h"
class CActor;

namespace Network
{
	//===============================================================================================//
	// TYPES
	//===============================================================================================//

	//	struct netplayeractor_t
	// Stores actor for a player
	struct netplayeractor_t {
		netid_t			netid;
		uint16_t		player;
		CActor*			actor;
	};

	//===============================================================================================//
	// METHODS
	//===============================================================================================//

	//		GetPlayerActors()
	// Returns a list of player actors in the game, in addition to their player id and their entity netid.
	// entity netid may not always be up-to-date.
	ENGCOM_API const std::vector<netplayeractor_t>& GetPlayerActors ( void );

	//		AddPlayerActor()
	// Adds an actor to the list of player actors, using the netlist to link up the netid with the player.
	ENGCOM_API void AddPlayerActor ( CActor* actor, const uint16_t playerid );

};

#endif//_ENGINE_COMMON_NETWORK_PLAYERLISTING_H_