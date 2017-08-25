
#ifndef _AFTER_TERRAIN_DATA_GAME_PAYLOAD_H_
#define _AFTER_TERRAIN_DATA_GAME_PAYLOAD_H_

#include <vector>

#include "core/types/types.h"
#include "core/math/Vector3d.h"
#include "after/types/terrain/Grass.h"

class CFoliage;
class CTerrainProp;

namespace Terrain
{
	//=========================================//
	//	struct foliage_t
	// Internal and offline storage of foliage objects.
	struct foliage_t
	{
		unsigned short	foliage_index;
		Vector3d		position;
		char			userdata [48];
	};
	static_assert( sizeof(foliage_t)<=64, "Foliage storage is not 64 bytes" );

	//=========================================//
	//	struct prop_t
	// Internal and offline storage of terrain prop objects.
	struct prop_t
	{
		unsigned short	component_index;
		Vector3d		position;
		Vector3d		rotation;
		Vector3d		scaling;
		uint64_t		userdata;
		// Indexing information
		uint16_t		block_pos_x;
		uint16_t		block_pos_y;
		uint16_t		block_pos_z;
	};
	static_assert( sizeof(prop_t)<=64, "Prop storage is not 64 bytes" );

	//=========================================//
	//	class CTtGamePayload
	// 
	// Container for miscellenous data.
	class CTtGamePayload
	{
	public:
		bool		loaded_macro;
		bool		loaded_small;

		std::vector<CTerrainProp*>	props;
		std::vector<CFoliage*>		foliage;
		std::vector<grass_t>		grass;

		std::vector<foliage_t>		foliage_queue;
		std::vector<prop_t>			prop_queue;
	};
	typedef CTtGamePayload GamePayload;
};

#endif//_AFTER_TERRAIN_DATA_GAME_PAYLOAD_H_