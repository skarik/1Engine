#include "sceneGameLuvPpl.h"

#include "m04/m04-common.h"

// Include game settings
#include "core/settings/CGameSettings.h"

// Include debug console
#include "core/debug/CDebugConsole.h"
#include "engine/utils/CDeveloperConsole.h"
#include "engine-common/utils/CDeveloperConsoleUI.h"

// Include 2d camera
//#include "render2d/camera/COrthoCamera.h"
// Include tileset system
#include "engine2d/entities/map/TileMap.h"
#include "engine2d/entities/map/CollisionMap.h"
#include "m04/states/MapInformation.h"

// Include resource system in case want to muck around with manual tileset loading
#include "core-ext/system/io/Resources.h"

// Include MapIO so we can load up the first map
#include "m04/interfaces/MapIO.h"

// Include debug tools
#include "engine-common/entities/CRendererHolder.h"
//#include "render2d/object/debug/Box2DDebugger.h"
#include "renderer/debug/RrBtDebugDraw.h"

#include "renderer/state/Settings.h"

#include "engine/physics/motion/CRigidbody.h"
#include "physical/physics/shapes/PrMesh.h"


void sceneGameLuvPpl::LoadScene ( void )
{
	debug::Console->PrintMessage( "Loading sceneGameLuvPpl.\n" );

	// Hide the cursor
	ActiveCursor->SetVisible(false);

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

	// Print a prompt
	debug::Console->PrintMessage( "You are running a debug build of M04.\n" );
	debug::Console->PrintMessage( "Loading Luv/ppl...\n" );

	// Create needed objects:
	Engine2D::TileMap* m_tilemap = new Engine2D::TileMap; m_tilemap->RemoveReference();
	M04::MapInformation* m_mapinfo = new M04::MapInformation;

	// Open the file
	FILE* fp = core::Resources::Open(M04::m04NextLevelToLoad.c_str(), "rb");
	if ( fp == NULL )
		throw core::NullReferenceException();

	// Create mapio, set options
	M04::MapIO io;
	io.m_file = fp;
	io.m_mapinfo = m_mapinfo;
	io.m_tilemap = m_tilemap;
	io.m_io_areas = true;
	io.m_io_objects_game = true;

	// load
	io.Load();

	// Close file
	fclose( fp );

	// Update the world
	renderer::Settings.ambientColor.SetCode( m_mapinfo->env_ambientcolor );

	// Create collision from world
	Engine2D::CollisionMap* m_collision = new Engine2D::CollisionMap; m_collision->RemoveReference();
	m_collision->m_tilemap = m_tilemap; m_collision->Rebuild();
	{
		PrMesh* shape = new PrMesh();
		shape->Initialize(&m_collision->m_mesh, true);

		prRigidbodyCreateParams params = {0};
		params.shape = shape;
		params.owner = NULL;
		params.ownerType = core::kBasetypeVoidPointer;
		params.group = physical::layer::PHYS_LANDSCAPE;
		params.mass = 0.0F;

		CRigidbody* bod = new CRigidbody(params);
		bod->ApiBody()->setLinearFactor(btVector3(0,0,0));
		bod->ApiBody()->setAngularFactor(btVector3(0,0,0));
		bod->PushTransform();
		bod->RemoveReference();
	}

	// Create debugger
	auto debug_holder = new CRendererHolder( new RrBtDebugDraw(NULL) );
	debug_holder->RemoveReference();
}