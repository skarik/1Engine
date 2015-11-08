
#ifndef _C_EVENT_BASE_H_
#define _C_EVENT_BASE_H_

#include "engine/behavior/CGameBehavior.h"

class CActor;

class CEventBase : public CGameBehavior
{
public:
	//=========================================//
	// Public event properties for spawning

	// The following keep track of who an event pertains to.
	// In a multiplayer session, events may focus around a certain player.

	Vector3d	m_focus_position;
	uint		m_focus_player;
	CActor*		m_focus_player_actor;

};

#endif//_C_EVENT_BASE_H_