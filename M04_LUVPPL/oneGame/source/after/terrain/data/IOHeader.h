

#ifndef _TERRAIN_IO_HEADER_H_
#define _TERRAIN_IO_HEADER_H_

#include "core/types/types.h"

namespace Terrain
{
	//	struct Terrain::filestruct_t
	// Used as a header for area information lookups.
	struct filestruct_t
	{
		uint16_t	version;
		uint8_t		terra_type;
		uint8_t		biome_type;

		Real_32		elevation;

		uint32_t	foliage_number;
		uint32_t	component_number;
		uint32_t	grass_number;
		uint32_t	npc_number;

		uint8_t padding [8];
	};
};

#endif//_TERRAIN_IO_HEADER_H_