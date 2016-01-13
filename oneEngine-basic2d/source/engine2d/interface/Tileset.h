
#ifndef _ENGINE2D_INTERFACE_TILESET_
#define _ENGINE2D_INTERFACE_TILESET_

#include "core/types/types.h"
#include "engine2d/types/MapTile.h"

#include <vector>

namespace Engine2D
{
	class Tileset
	{
	public:


	public:
		// Size of the smallest individual tiles in pixels
		int tilesize_x;
		int tilesize_y;

		// Size of the tileset source image in pixels
		int tilecount_x;
		int tilecount_y;

		// Actual size of the tileset source image in tiles
		uint32_t atlassize_x;
		uint32_t atlassize_y;

		// Types of tiles this tileset works with
		std::vector<tilesetEntry_t> tiles;
	};
}

#endif//_ENGINE2D_INTERFACE_TILESET_