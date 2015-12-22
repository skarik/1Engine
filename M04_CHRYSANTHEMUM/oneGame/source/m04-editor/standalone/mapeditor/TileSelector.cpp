
#include "TileSelector.h"
#include "TileSelectorUI.h"

#include "engine2d/entities/map/TileMap.h"

using namespace M04;

TileSelector::TileSelector ( void )
{
	ui = new TileSelectorUI();

	{
		Engine2D::TileMap* tilemap = new Engine2D::TileMap();
		tilemap->RemoveReference(); // So it can be destroyed when the game quits

		// Set the tileset sprite
		tilemap->SetSpriteFile( "textures/ruins.png" );
		// Set the tileset information
		Engine2D::Tileset* tileset = new Engine2D::Tileset ();
		tileset->atlassize_x = 256;
		tileset->atlassize_y = 256;
		tileset->tilecount_x = 8;
		tileset->tilecount_y = 8;
		tileset->tilesize_x = 32;
		tileset->tilesize_y = 32;
		tilemap->SetTileset( tileset );

		// Set map data
		tilemap->SetDebugTileMap(
			40,40			// size of map in tiles
		);

		// Rebuild the map visuals
		tilemap->Rebuild();

		ui->SetTileMap( tilemap );
	}
}
TileSelector::~TileSelector ( void )
{
	delete_safe(ui);
}

void TileSelector::Update ( void )
{
	// Nothing at this point
}