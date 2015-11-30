#include "sceneTilesetTest.h"

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

// Include resource system in case want to muck around with manual tileset loading
#include "core-ext/system/io/Resources.h"

void sceneTilesetTest::LoadScene ( void )
{
	Debug::Console->PrintMessage( "Loading test scene 0: tilesets.\n" );

	// Create orthographic camera
	{
		COrthoCamera* cam = new COrthoCamera();
		// Set camera options
		cam->pixel_scale_mode = orthographicScaleMode_t::ORTHOSCALE_MODE_SIMPLE;
		cam->SetActive(); // Mark it as the main camera to use IMMEDIATELY
	}

	// Create the tileset tester
	{
		Engine2D::TileMap* tilemap = new Engine2D::TileMap();
		tilemap->RemoveReference(); // So it can be destroyed when the game quits
	}

	// Print a prompt
	Debug::Console->PrintMessage( "You are running a debug build of M04.\n" );
	Debug::Console->PrintMessage( "This test is meant to show both the tileset system's capabilities and a simple realtime lighting system.\n" );
}