
#ifndef _AFTER_TERRAIN_DATA_GAMESTATE_PAYLOAD_H_
#define _AFTER_TERRAIN_DATA_GAMESTATE_PAYLOAD_H_

#include "after/types/WorldVector.h"
#include <vector>

class CWeaponItem;
class CTerraGrass;

namespace Terrain
{
	//=========================================//
	//	class CTtGameState
	// 
	// Container for game-state specific objects.
	// Typically refers to a 64x64x64 area.
	class CTtGameState
	{
	public:
		CTerraGrass*				grass_renderer;
		std::vector<CWeaponItem*>	item_list;

		bool						has_block_data;
		bool						npcs_loaded;

		WorldVector					world_vector;
	};
	typedef CTtGameState AreaGameState;
};

#endif//_AFTER_TERRAIN_DATA_GAMESTATE_PAYLOAD_H_