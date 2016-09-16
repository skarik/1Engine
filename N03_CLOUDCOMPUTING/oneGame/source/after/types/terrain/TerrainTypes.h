
#ifndef _TERRAIN_AND_BLOCK_TYPES_H_
#define _TERRAIN_AND_BLOCK_TYPES_H_

#include "core/types/types.h"

namespace Terrain
{
	enum ETerrainType : uint8_t
	{
		TER_DEFAULT		= 0,
		TER_OUTLANDS	= 1,
		TER_FLATLANDS	= 2,
		TER_HILLLANDS	= 3,
		TER_OCEAN		= 4,
		TER_ISLANDS		= 5,
		TER_DESERT		= 6,
		TER_MOUNTAINS	= 7,
		TER_SPIRES		= 8,
		TER_BADLANDS	= 9,
		TER_GLACIER		= 10,
		TER_THE_EDGE	= 11
	};
}

#endif