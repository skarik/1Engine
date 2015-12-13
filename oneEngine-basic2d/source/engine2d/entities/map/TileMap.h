
#ifndef _ENGINE2D_TILEMAP_H_
#define _ENGINE2D_TILEMAP_H_

#include <vector>
#include <mutex>

#include "core/types/ModelData.h"
#include "core/containers/arstring.h"
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

		//		Rebuild ()
		// Rebuilds the meshes used to render the map.
		ENGINE2D_API void			Rebuild ( void );

		//		RebuildMeshs( ... )
		// Given the list of layers, build a mesh for each layer
		//	layer					- layer to build
		//	start_offset			- beginning index of the tile
		//	predictive_tile_count	- when to watch out for the end of the mesh. Used to allocate triangles
		//void			RebuildMeshs ( int * layers, int layer_count );
		ENGINE2D_API void			RebuildMesh ( int layer, int start_offset = 0, int predictive_tile_count = 0 );

		//		SetSpriteFile ( cstring sprite_file )
		// Sets sprite file to use for it
		//	n_sprite_file			- resource path to the sprite
		ENGINE2D_API void			SetSpriteFile ( const char* n_sprite_file );

		//		SetTileset ( Tileset* tileset_to_use )
		// Sets a tileset to use for the layers
		//	tileset_to_use			- tileset that will be used for building the tile information
		ENGINE2D_API void			SetTileset ( Tileset* tileset_to_use );

		//		SetTilesetFile ( cstring sprite_file )
		// Sets the tileset file.
		//	n_tileset_file			- resource path to the tileset info file
		// Internally calls SetSpriteFile and SetTileset. Both sets of needed data are specified by the tileset file
		ENGINE2D_API void			SetTilesetFile ( const char* n_tileset_file ) { ; }

		//=========================================//

		//		SetDebugTileMap ( ... )
		// Builds a debug map that is the size specified in the arguments. If tile data does not exist, it will be created.
		//	n_map_w					- width of the map
		//	n_map_h					- height of the map
		ENGINE2D_API void			SetDebugTileMap ( const uint n_map_w, const uint n_map_h );

	public:
		Tileset*				m_tileset;
		std::vector<mapTile_t>	m_tiles;

		std::mutex				m_mut_meshstorage;
		std::vector<ModelData>	m_meshstorage;

		arstring<256>			m_sprite_file;
	};
};

#endif//_ENGINE2D_TILEMAP_H_