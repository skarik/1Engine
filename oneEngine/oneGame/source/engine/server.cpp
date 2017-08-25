
#include "server.h"

//		IsServer()
// Returns if the current system is a server.
// Result true for both default singleplayer mode and multiplayer servers.
bool IsServer ( void )
{
	// Currently, we assume singleplay. Therefore, return true.
	return true;
}


namespace Network
{
	//		Host ()
	// Hosts server. Does not assume a player to connect.
	int Host ( uint16_t	mainPort )
	{
		return 0;
	}

}