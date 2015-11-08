#include "CWaypoint.h"
#include "CPlayer.h"

#include "engine/state/CGameState.h"
#include "engine-common/network/playerlist.h"

//DON'T LOSE YOUR WAY!

std::vector<CWaypoint*> CWaypoint::WaypointList;

CWaypoint::CWaypoint (void) : CPointBase(),
	entity (NULL), type (PLAIN)
{
	WaypointList.push_back (this);
	distance = 0.0;
	in_range = false;
}

CWaypoint::~CWaypoint (void)
{
	entity = NULL;

	auto findresult = std::find(WaypointList.begin(), WaypointList.end(), this);
	if (findresult != WaypointList.end())
	{
		WaypointList.erase(findresult);
	}
	//We lost our way. : |
}

void CWaypoint::Update (void)
{
	if ( IsServer() )
	{
		auto playerList = Network::GetPlayerActors();
		for ( uint i = 0; i < playerList.size(); ++i )
		{
			if ( playerList[i].actor != NULL ) // If the actor is valid....
			{	// ....then work with that actor!
				distance = (m_position - playerList[i].actor->transform.position).magnitude(); //TODO: Check if the actor is indeed a player.

				if (distance < 3.0f && in_range == false)
				{
					//GameState->messenger.Send("CPlayer", Game::eGameMessages::MSG_WAYPOINTIN);
					CGameState::Active()->messenger.SendGlobal( Game::eGameMessages::MSG_WAYPOINTIN );
					in_range = true;
				}
				if (distance > 3.0f && in_range == true)
				{
					CGameState::Active()->messenger.SendGlobal( Game::eGameMessages::MSG_WAYPOINTOUT );
					in_range = false;
				}
				//go tell the responsible system it's zero
			}
		}
	}
}