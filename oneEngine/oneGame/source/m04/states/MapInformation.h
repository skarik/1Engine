
#ifndef _M04_MAP_INFORMATION_H_
#define _M04_MAP_INFORMATION_H_

#include <cstdlib>

#include "core/types/types.h"
#include "core/containers/arstring.h"

namespace M04
{
	class MapInformation
	{
	public:
		arstring<256>		map_name;
		arstring<256>		area_name;

		int32_t				tilesize_x;		// Width of the map in tiles
		int32_t				tilesize_y;		// Height of the map in tiles

		uint32_t			env_weathertype;
		uint32_t			env_ambientcolor;
		uint32_t			env_unused2;
		uint32_t			env_unused3;
		uint32_t			env_unused4;
		uint32_t			env_unused5;
		uint32_t			env_unused6;
		uint32_t			env_unused7;

		uint32_t			prop_unused0;
		uint32_t			prop_unused1;
		uint32_t			prop_unused2;
		uint32_t			prop_unused3;
		uint32_t			prop_unused4;
		uint32_t			prop_unused5;
		uint32_t			prop_unused6;
		uint32_t			prop_unused7;

	public:
		MapInformation ( void )
		{
			// Initalize everything to zero
			memset( this, 0, sizeof(MapInformation) );
		}
	};
}

#endif//_M04_MAP_INFORMATION_H_