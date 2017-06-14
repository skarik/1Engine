#ifndef _M04_EDITOR_TILE_SELECTOR_UI_
#define _M04_EDITOR_TILE_SELECTOR_UI_

#include "renderer/object/CRenderableObject.h"

namespace Engine2D
{
	class TileMap;
	class Tileset;
}

namespace M04
{
	class TileSelectorUI : public CRenderableObject
	{
	public:
		explicit	TileSelectorUI ( void );
					~TileSelectorUI ( void );

		bool		Render ( const char pass ) override;

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
		RrMaterial*			ui_material;

		int					ui_columns;
		int					ui_spacing;
		Vector2d			ui_scale;
		Vector2d			ui_base_offset;

		int					ui_mouseover;
	};
};

#endif//_M04_EDITOR_TILE_SELECTOR_UI_