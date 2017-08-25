
#ifndef _NPC_AIFOCUS_H_
#define _NPC_AIFOCUS_H_

#include "core/types/types.h"

namespace NPC
{
	// enum eBasicAIFocus; prefix AIFOCUS_
	// Listing of all types of 'jobs' AI's may hold
	enum eBasicAIFocus : uint16_t
	{
		AIFOCUS_Wanderer = 0,
		AIFOCUS_Companion,
		AIFOCUS_TownPerson,
		AIFOCUS_Farmer,
		AIFOCUS_LuaGeneral,

		AIFOCUS_None = 65535
	};
}


#endif//_NPC_AIFOCUS_H_