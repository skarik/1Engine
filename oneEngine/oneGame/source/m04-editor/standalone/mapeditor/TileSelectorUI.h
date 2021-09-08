#ifndef _M04_EDITOR_TILE_SELECTOR_UI_
#define _M04_EDITOR_TILE_SELECTOR_UI_

#include "renderer/object/RrRenderObject.h"
#include "renderer/object/mesh/system/rrMeshBuffer.h"

namespace Engine2D
{
	class TileMap;
	class Tileset;
}

namespace M04
{
	class TileSelectorUI : public RrRenderObject
	{
	public:
		explicit	TileSelectorUI ( void );
					~TileSelectorUI ( void );

		bool		CreateConstants ( rrCameraPass* passs ) override;
		bool		Render ( const rrRenderParams* params ) override;
		bool		EndRender ( void ) override;

		//		SetTileMap ( )
		// Sets the tilemap containing the tileset data that need to pull from
		void		SetTileMap ( Engine2D::TileMap* target );
		//		SetTileset ( )
		// Sets the tileset to pull data from
		void		SetTileset ( Engine2D::Tileset* target );

		//		UIMouseoverTiletype ( )
		// Get the tileset tile type the mouse is currently over.
		// Returns negative value when below 
		int			UIMouseoverTiletype ( void );

		//		UISetSelection ( selection )
		// Overrides the selection to actually use when rendering.
		void		UISetSelection ( int );
	public:
		Engine2D::Tileset*	m_tileset;

	protected:
		int					ui_columns;
		int					ui_spacing;
		Vector2f			ui_scale;
		Vector2f			ui_base_offset;

		int					ui_mouseover;

		uint				mesh_index_count_tiles;
		uint				mesh_index_count_ui;
		rrMeshBuffer		mesh_buffer_tiles;
		rrMeshBuffer		mesh_buffer_ui;
	};
};

#endif//_M04_EDITOR_TILE_SELECTOR_UI_