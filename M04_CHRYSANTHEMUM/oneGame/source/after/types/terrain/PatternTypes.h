
#ifndef _TERRAIN_PATTERN_TYPES_H_
#define _TERRAIN_PATTERN_TYPES_H_

#include "core/types/types.h"

namespace Terrain
{
	// Pattern type - used to check if a town should be edited over time
	enum EPatternType : int16_t
	{
		PTN_INVALID = -1,
		PTN_UPDATE_TOWN = 1,
		PTN_UPDATE_CITY = 2,
		PTN_NO_UPDATE = 4
	};
}

#endif//_TERRAIN_PATTERN_TYPES_H_