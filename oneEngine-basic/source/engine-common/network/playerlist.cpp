
#include "playerlist.h"

std::vector<Network::netplayeractor_t> m_playerList;

//		GetPlayerActors()
// Returns a list of player actors in the game, in addition to their player id and their entity netid.
// entity netid may not always be up-to-date.
ENGCOM_API const std::vector<Network::netplayeractor_t>& Network::GetPlayerActors ( void )
{
	// return empty list for now
	return m_playerList;
}


//		AddPlayerActor()
// Adds an actor to the list of player actors, using the netlist to link up the netid with the player.
ENGCOM_API void Network::AddPlayerActor ( CActor* actor, const uint16_t playerid )
{
	netplayeractor_t newActor;
	newActor.actor = actor;
	newActor.netid = -1; // TODO
	newActor.player = playerid;
	m_playerList.push_back(newActor);
}