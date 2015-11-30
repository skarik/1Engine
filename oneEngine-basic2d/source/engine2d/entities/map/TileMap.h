
#ifndef _ENGINE2D_TILEMAP_H_
#define _ENGINE2D_TILEMAP_H_

#include "core/types/ModelData.h"
#include "engine/behavior/CGameBehavior.h"
#include "engine2d/interface/Tileset.h"

namespace Engine2D
{
	class TileMap : public CGameBehavior
	{
	public:
		ENGINE2D_API explicit		TileMap ( void );
		ENGINE2D_API 				~TileMap ( void );

		//=========================================//

		ENGINE2D_API void			Update ( void );

		//=========================================//

		ENGINE2D_API void			Rebuild ( void );

		//		RebuildMeshs( layer list )
		// Given the list of layers, build a mesh for each layer
		//	layer					- layer to build
		//	start_offset			- beginning index of the tile
		//	predictive_tile_count	- when to watch out for the end of the mesh. Used to allocate triangles
		//void			RebuildMeshs ( int * layers, int layer_count );
		ENGINE2D_API void			RebuildMesh ( int layer, int start_offset = 0, int predictive_tile_count = 0 );


	public:
		Tileset*		m_tileset;
		std::vector<mapTile_t>	m_tiles;
	};
};

#endif//_ENGINE2D_TILEMAP_H_