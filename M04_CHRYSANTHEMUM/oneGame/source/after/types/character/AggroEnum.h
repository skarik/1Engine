#ifndef _AFTER_TYPES_AGGRO_ENUM_H_
#define _AFTER_TYPES_AGGRO_ENUM_H_

#include "core/types/types.h"

namespace NPC
{
	// Aggro type
	//  Determines how NPCs will aggro to the player
	enum eAggroAIType : uint8_t
	{
		// type 0: default, mostly peaceful feral animals
		//		enemies will mostly ignore the target unless the target is too close for too long
		//		or the target attacks the NPC
		AGGRO_ANIMAL = 0,
		// type 1: enemy default, general enemies
		//		enemy will actively seek out and destroy any non-allied target it can find.
		AGGRO_MONSTER = 1,
		// type 2: character NPCs
		//		will try not to aggro to targets in general unless safety is deemed hazardous, or
		//		the target has type 0 or 1 and is aggroed to self
		AGGRO_CHARACTER = 2
	};
}

#endif//_AFTER_TYPES_AGGRO_ENUM_H_