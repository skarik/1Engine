#include "sceneTilesetTest.h"

// Include game settings
#include "core/settings/CGameSettings.h"
// Include debug console
#include "core/debug/Console.h"
#include "engine/utils/CDeveloperConsole.h"
#include "engine-common/utils/CDeveloperConsoleUI.h"
// Include 2d camera
#include "render2d/camera/COrthoCamera.h"
// Include tileset system
#include "engine2d/entities/map/TileMap.h"

// Include resource system in case want to muck around with manual tileset loading
#include "core-ext/system/io/Resources.h"

void sceneTilesetTest::LoadScene ( void )
{
	debug::Console->PrintMessage( "Loading test scene 0: tilesets.\n" );

	// Create orthographic camera
	{
		COrthoCamera* cam = new COrthoCamera();
		// Set camera options
		cam->pixel_scale_mode = orthographicScaleMode_t::ORTHOSCALE_MODE_SIMPLE;
		cam->viewport_target.size = Vector2f( 1280,720 ) * 0.5f;
		cam->SetActive(); // Mark it as the main camera to use IMMEDIATELY
	}

	// Create normal orthographic camera
	/*{
		RrCamera* cam = new RrCamera();
		cam->orthographic = true;
		cam->transform.rotation = Rotator( 0,45,45 );

		cam->ortho_size = Vector2f( 1000,1000 );
		cam->SetActive(); // Mark it as the main camera to use IMMEDIATELY
	}*/

	// Create the tileset tester
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
	}

	// Print a prompt
	debug::Console->PrintMessage( "You are running a debug build of M04.\n" );
	debug::Console->PrintMessage( "This test is meant to show both the tileset system's capabilities and a simple realtime lighting system.\n" );
}