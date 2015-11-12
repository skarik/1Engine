
#ifndef _GAME_MESSAGES_H_
#define _GAME_MESSAGES_H_

#include "core/types/types.h"

namespace Game
{
	enum eGameMessages : uint64_t
	{
		MSG_NONE,
		MSG_KILL,
		MSG_WAYPOINTIN,
		MSG_WAYPOINTOUT,

		MSG_GAME_SKILLTREE_RELOAD
	};
}

#endif//_GAME_MESSAGES_H_