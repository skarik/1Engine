#ifndef ENGINE2D_TYPES_MAP_TILE_
#define ENGINE2D_TYPES_MAP_TILE_

#include "core/types/types.h"

#define MAPTILE_AUTOTILE_SIZE 32	// Max number of autotile entries allowed to be defined.

enum ETileCollisionOverride : uint16_t
{
	kTileCollisionOverrideDefault,
	kTileCollisionOverrideNoCollision,
	kTileCollisionOverrideCollideBlock,
};
enum ETileCollisionType : uint16_t
{
	kTileCollisionTypeNone,
	kTileCollisionTypeImpassable,
};

struct mapTile_t
{
	// tile ID in the tileset. In terms of autotiles, number may not be contiguous.
	// Indexes into the array of tilesetEntry_t.
	uint32_t	type;	
	// Collision override. Defaults to 0.
	uint16_t	collision_override;
	// layer of the tile
	int16_t		depth;
	// tile world position
	int32_t		x, y;
};
static_assert( sizeof(mapTile_t) == sizeof(uint32_t)*4, "mapTile_t: Invalid bit sizing!" );

enum tilesetTiletype_t : uint16_t
{
	TILE_AUTOTILE,
	TILE_DEFAULT,
	TILE_PROP,
	TILE_AUTOWALL,
	TILE_RANDOMIZED,
};
struct tilesetEntry_t
{
	tilesetTiletype_t	type;
	ETileCollisionType	collision;
	uint16_t			atlas_x;
	uint16_t			atlas_w;
	uint16_t			atlas_y;
	uint16_t			atlas_h;

	uint8_t		autotile_0 [MAPTILE_AUTOTILE_SIZE];
	uint8_t		autotile_1 [MAPTILE_AUTOTILE_SIZE];
};

// autotile looks around but for now, let's focus on tile editing

#endif//ENGINE2D_TYPES_MAP_TILE_