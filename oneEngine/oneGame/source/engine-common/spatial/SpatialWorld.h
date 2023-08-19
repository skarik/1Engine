#ifndef ENGINE_COMMON_SPATIAL_WORLD_H_
#define ENGINE_COMMON_SPATIAL_WORLD_H_

#include "SpatialStorage.h"

namespace engine
{
	// @brief Storage for a world's objects and behaviors.
	//		World's have two LOD's: lo and hi.
	class SpatialWorld
	{
	public:

		// Call OnLodEnter/OnLodExit( SpatialEntity& ) depending on the PoV

		void SetPlayerPosition ( Vector3f& nextPlayerPosition );
		
		void OnLodEnter ( SpatialSector& sector );
		void OnLodExit ( SpatialSector& sector );
		void OnLodEnter ( SpatialEntity& ent );
		void OnLodExit ( SpatialEntity& ent );

	public:
		SpatialStorage sectorLods;
		Vector3f playerPosition;

		// Sectors that the player is currently inside.
		std::vector<SpatialSector*> currentSectors;
	};
};

#endif//ENGINE_COMMON_SPATIAL_WORLD_H_