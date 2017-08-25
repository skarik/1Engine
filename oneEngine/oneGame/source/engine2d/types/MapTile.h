
#ifndef _ENGINE2D_TYPES_MAP_TILE_
#define _ENGINE2D_TYPES_MAP_TILE_

#include "core/types/types.h"

#define MAPTILE_AUTOTILE_SIZE 32

struct mapTile_t
{
	uint32_t	type;				// tile ID in the tileset. In terms of autotiles, number may not be contiguous
	uint16_t	collision_override;
	int16_t		depth;
	int32_t		x;
	int32_t		y;
};
static_assert( sizeof(mapTile_t) == sizeof(uint32_t)*4, "mapTile_t: Invalid bit sizing!" );

enum tilesetTiletype_t
{
	TILE_AUTOTILE,
	TILE_DEFAULT,
	TILE_PROP,
	TILE_AUTOWALL,
	TILE_RANDOMIZED
};
struct tilesetEntry_t
{
	tilesetTiletype_t	type;
	uint16_t			atlas_x;
	uint16_t			atlas_w;
	uint16_t			atlas_y;
	uint16_t			atlas_h;

	uint8_t		autotile_0 [MAPTILE_AUTOTILE_SIZE];
	uint8_t		autotile_1 [MAPTILE_AUTOTILE_SIZE];
};

// autotile looks around but for now, let's focus on tile editing

#endif//_ENGINE2D_TYPES_MAP_TILE_