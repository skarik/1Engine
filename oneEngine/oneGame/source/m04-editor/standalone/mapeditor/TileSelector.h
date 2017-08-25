#ifndef _M04_EDITOR_TILE_SELECTOR_
#define _M04_EDITOR_TILE_SELECTOR_

namespace Engine2D
{
	class TileMap;
	class Tileset;
}

namespace M04
{
	class TileSelectorUI;

	class TileSelector
	{
	public:
		explicit		TileSelector ( void );
						~TileSelector( void );

		bool			Update ( void );

		//		SetTileMap ( )
		// Sets the tilemap containing the tileset data that need to pull from
		void			SetTileMap ( Engine2D::TileMap* target );

		//		GetTileSelect ( )
		// return the tile selection currently used for the selector
		int				GetTileSelection ( void ) const;

		//		SetVisible ( visible? )
		// sets if the UI is visible or not
		void			SetVisible ( const bool visibility );
	protected:
		TileSelectorUI*	ui;
		int				m_tileselection;
	};
};

#endif//_M04_EDITOR_TILE_SELECTOR_