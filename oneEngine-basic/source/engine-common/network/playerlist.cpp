
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