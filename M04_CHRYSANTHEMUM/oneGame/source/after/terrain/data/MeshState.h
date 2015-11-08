
#ifndef _AFTER_TERRAIN_DATA_MESHSTATE_PAYLOAD_H_
#define _AFTER_TERRAIN_DATA_MESHSTATE_PAYLOAD_H_

#include "after/types/WorldVector.h"

namespace Terrain
{
	//=========================================//
	//	class CTtMeshState
	// 
	// Container for game-state specific information.
	// Typically refers to a 64x64x64 area.
	class CTtMeshState
	{
	public:
		bool						has_mesh;
		bool						has_collision;

		WorldVector					world_vector;
	};
	typedef CTtMeshState AreaMeshState;
};

#endif//_AFTER_TERRAIN_DATA_MESHSTATE_PAYLOAD_H_