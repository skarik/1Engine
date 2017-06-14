
#ifndef _AFTER_VOXEL_TERRAIN_RENDERER_H_
#define _AFTER_VOXEL_TERRAIN_RENDERER_H_

#include "renderer/object/CRenderableObject.h"
#include "after/terrain/data/MeshState.h"

class CVoxelTerrain;

namespace Terrain
{
	class TerrainRenderer : public CRenderableObject
	{
	public:
		explicit				TerrainRenderer ( CVoxelTerrain* n_terrain );
								~TerrainRenderer ( void );

		//=========================================//
		// Render step

		bool					Render ( const char pass ) override;

		//=========================================//
		// Terrain-related systems

		//		ClearAll
		// clears mesh and collision data for all current sectors/areas
		void					ClearAll ( void );
		//		ClearAreaAt
		// clears mesh and collision data at the given sector
		void					ClearAreaAt ( const WorldVector& n_sector );
		void					ClearAreaAt ( const Vector3d& n_position );
		//		InvalidateAll
		// marks all sectors/areas as out-of-date
		void					InvalidateAll ( void );
		//		InvalidateAreaAt
		// marks out-of-date flag at the given sector
		void					InvalidateAreaAt ( const WorldVector& n_sector );
		void					InvalidateAreaAt ( const Vector3d& n_position );

		//		AquireAreaGamestate
		// Grabs an area's gamestate from the given terrain, given the input gameplay position.
		// AreaGameState is specifically for data that can change often and extremely quickly.
		// Returns the found area gamestate (or NULL if not found) along with an indexer for the lock system.
		// Remember to call ReleaseAreaGamestate() with the given indexer, or the system will lock up during saving.
		//Terrain::AreaMeshState*	AquireAreaMeshstate ( const Vector3d& n_approximatePosition, uint32_t& o_indexer );

		//		GetAreaMeshstate
		// Grabs an area's meshstate from the given terrain, given the input gameplay position.
		// This is safe without locks because all mesh and collision updates must happen in the main thread.
		Terrain::AreaMeshState*	GetAreaMeshstate ( const Vector3d& n_approximatePosition );

	protected:
		// Current references

		CVoxelTerrain*			m_terrain;
	};
}

#endif//_AFTER_VOXEL_TERRAIN_DATA_SAMPLER_H_