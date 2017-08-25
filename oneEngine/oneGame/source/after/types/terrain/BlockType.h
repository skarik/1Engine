
#ifndef _TERRAIN_BLOCKTYPES_H_
#define _TERRAIN_BLOCKTYPES_H_

#include "core/types/types.h"

class physMaterial;

namespace Terrain
{
	enum EBlockType : uint8_t
	{
		EB_NONE = 0,
		EB_DIRT = 1,
		EB_GRASS,
		EB_STONE,
		EB_SAND,
		EB_WATER,
		EB_WOOD,
		EB_CLAY,
		EB_MUD,
		EB_WIRE,
		EB_GRAVEL,
		EB_STONEBRICK,
		EB_WATERSPRING,
		EB_STEAMPIPE,
		EB_ANC_ELECTRIC,
		EB_ANC_WIRE,
		EB_ANC_UNOBTANIUM,
		EB_ICE,
		EB_ANC_GRAVITY_JOKE,
		EB_CRYSTAL,
		EB_XPLO_CRYSTAL,
		EB_SANDSTONE,
		EB_ROAD_GRAVEL,
		EB_ASH,
		EB_DEADSTONE,
		EB_CURSED_DEADSTONE,

		EB_SNOW,
		EB_TOP_SNOW,

		EB_RIGDESTONE,

		EB_ROOF_0,
		EB_ROOF_1,

		EB_HEMATITE,
		EB_MAGNETITE,
		EB_GOETHITE,

		EB_IRONBRICK,

		EB_TILLED_DIRT,

		// System block types.
		// The following blocks actually don't have a presence and are used for "system" things
		EB_SYS_ORE_SPAWN		=249, // Is replaced with a proper ore on area's first load

		EB_SYS_COMPONENT_0		=250,
		EB_SYS_COMPONENT_1		=251,
		EB_SYS_COMPONENT_2		=252,
		EB_SYS_COMPONENT_3		=253,
	
		EB_SYS_COLLISION		=254,
		EB_IGNORE				=255
	};


	// Block material properties
	extern char cDefaultBlockHardness [1024];
	extern char blockName [1024][64];
	//extern const char blockName [1024][64];

	// Returns material of block type
	const physMaterial& MaterialOf ( const short iBlockType );

	// Checker class, for block-type checks (used in meshbuilding)
	class Checker
	{
	public:
		FORCE_INLINE static bool BlocktypeOpaque ( const ushort blockType )
		{
			if ( blockType != EB_NONE && blockType != EB_WATER ) {
				return false;
			}
			return true;
		}
		FORCE_INLINE static bool BlocktypeTransparent ( const ushort blockType )
		{
			if ( blockType == EB_NONE || blockType == EB_WATER ) {
				return false;
			}
			return true;
		}
	};
};

#endif//_TERRAIN_BLOCKTYPES_H_