
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
		explicit		TileMap ( void );
						~TileMap ( void );

		//=========================================//

		void			Rebuild ( void );

		//		RebuildMeshs( layer list )
		// Given the list of layers, build a mesh for each layer
		//void			RebuildMeshs ( int * layers, int layer_count );

		void			RebuildMesh ( int layer, int start_offset = 0, int predictive_tile_count = 0 );


	public:
		Tileset*		m_tileset;
		std::vector<mapTile_t>	m_tiles;
	};
};

#endif//_ENGINE2D_TILEMAP_H_