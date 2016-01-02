#include "sceneEditorMain.h"

// Include game settings
#include "core/settings/CGameSettings.h"
// Include debug console
#include "core/debug/CDebugConsole.h"
#include "engine/utils/CDeveloperConsole.h"
#include "engine-common/utils/CDeveloperConsoleUI.h"
// Include 2d camera
#include "render2d/camera/COrthoCamera.h"
// Include tileset system
#include "engine2d/entities/map/TileMap.h"
// Include map editor
#include "m04-editor/standalone/MapEditor.h"

// Include resource system in case want to muck around with manual tileset loading
#include "core-ext/system/io/Resources.h"

void sceneEditorMain::LoadScene ( void )
{
	Debug::Console->PrintMessage( "Loading test scene 1: editor.\n" );

	// Create orthographic camera
	/*{
		COrthoCamera* cam = new COrthoCamera();
		// Set camera options
		cam->pixel_scale_mode = orthographicScaleMode_t::ORTHOSCALE_MODE_SIMPLE;
		cam->viewport_target.size = Vector2d( 1280,720 ) * 0.5f;
		cam->SetActive(); // Mark it as the main camera to use IMMEDIATELY
	}*/

	// Create normal orthographic camera
	/*{
		CCamera* cam = new CCamera();
		cam->orthographic = true;
		cam->transform.rotation = Rotator( 0,45,45 );

		cam->ortho_size = Vector2d( 1000,1000 );
		cam->SetActive(); // Mark it as the main camera to use IMMEDIATELY
	}*/

	// Create the tileset tester
	/*{
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
	}*/

	// Create the map editor
	{
		M04::MapEditor* editor = new M04::MapEditor();
		editor->RemoveReference(); // So it can be destroyed when the game quits
	}

	// Print a prompt
	Debug::Console->PrintMessage( "You are running a debug build of M04.\n" );
	Debug::Console->PrintMessage( "This test is meant to show both the tileset system's capabilities and a simple realtime lighting system.\n" );
}