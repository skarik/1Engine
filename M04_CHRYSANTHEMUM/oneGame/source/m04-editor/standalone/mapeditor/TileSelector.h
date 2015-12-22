#ifndef _M04_EDITOR_TILE_SELECTOR_
#define _M04_EDITOR_TILE_SELECTOR_

namespace M04
{
	class TileSelectorUI;

	class TileSelector
	{
	public:
		explicit		TileSelector ( void );
						~TileSelector( void );

		void			Update ( void );

	protected:
		TileSelectorUI*	ui;
	};
};

#endif//_M04_EDITOR_TILE_SELECTOR_