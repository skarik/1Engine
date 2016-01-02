
#include "MapEditor.h"

#include "core/input/CInput.h"
#include "core/math/Math.h"


#include "engine-common/dusk/CDuskGUI.h"
#include "engine-common/entities/CRendererHolder.h"

#include "renderer/texture/CBitmapFont.h"
#include "renderer/camera/CCamera.h"
#include "renderer/debug/CDebugDrawer.h"
#include "render2d/camera/COrthoCamera.h"
#include "render2d/object/Background2D.h"

#include "./mapeditor/TileSelector.h"

#include "engine2d/entities/map/TileMap.h"
#include "engine2d/entities/Area2DBase.h"

#include "m04/states/MapInformation.h"
#include "m04/interfaces/MapIO.h"

#include "m04-editor/renderer/object/AreaRenderer.h"

using namespace M04;

MapEditor::MapEditor ( void )
	: CGameBehavior(),
	m_current_mode(Mode::TileEdit), m_current_submode(SubMode::None), m_navigation_busy(false),
	m_current_savetarget("")
{
	// Create map info
	{
		m_mapinfo = new M04::MapInformation ();
		// Set default map size
		m_mapinfo->tilesize_x = 40;
		m_mapinfo->tilesize_y = 40;
	}
	// Craete tile map
	{
		Engine2D::TileMap* tilemap = new Engine2D::TileMap();
		tilemap->RemoveReference(); // So it can be destroyed when the game quits

		// Set the tileset sprite
		tilemap->SetTilesetFile( "tileset/default.txt" );

		// Set map data
		tilemap->SetDebugTileMap(
			m_mapinfo->tilesize_x,m_mapinfo->tilesize_y	// size of map in tiles
		);

		// Rebuild the map visuals
		tilemap->Rebuild();
		
		// Save it
		m_tilemap = tilemap;
	}

	// Create orthographic camera
	{
		COrthoCamera* cam = new COrthoCamera();
		// Set camera options
		cam->pixel_scale_mode = orthographicScaleMode_t::ORTHOSCALE_MODE_SIMPLE;
		cam->viewport_target.size = Vector2d( 1280,720 ) * 0.5f;
		cam->SetActive(); // Mark it as the main camera to use IMMEDIATELY
		// Use this new camera as our main camera
		m_target_camera = cam;
	}
	// Create background
	{
		CRenderable2D* bg = new Renderer::Background2D();
		(new CRendererHolder (bg))->RemoveReference();
	}
	// Create area renderer
	{
		CRenderableObject* area = new M04::AreaRenderer();
		area->transform.position.z = -100;
		(new CRendererHolder (area))->RemoveReference();
	}
	// Build Dusk Gui
	{
		dusk = new CDuskGUI();
		dusk->SetDefaultFont( new CBitmapFont( "YanoneKaffeesatz-R.otf", 16 ) );
		dusk->SetPixelMode(true);

		uiCreate();
	}
	// Build other UI elements
	{
		m_tile_selector = new M04::TileSelector();
		m_tile_selector->SetTileMap( m_tilemap );
	}
	// Rebuild the map again
	{
		// Rebuild the map visuals
		m_tilemap->Rebuild();
	}
}

MapEditor::~MapEditor ( void )
{
	delete_safe(m_target_camera);
	delete_safe_decrement(dusk);
	delete_safe(m_tile_selector);
}

//===============================================================================================//
// Update
//===============================================================================================//

void MapEditor::Update ( void )
{
	if ( dusk->HasOpenDialogue() )
	{
		// Close all modes for the dialogue system
		m_current_mode = Mode::None;

		// Update the UI for the dialogues
		uiStepDialogues();
	}
	else
	{
		if ( !dusk->GetMouseInGUI() )
		{
			// Only do map mouse events if the mouse is not in the dusk GUI
			switch ( m_current_mode )
			{
			case Mode::None:
				doViewNavigationDrag();
				break;

			case Mode::TileEdit:
				doViewNavigationDrag();
				if ( !m_tile_selector->Update() )
				{
					doTileEditing();
				}
				break;
			
			case Mode::AreaEdit:
				doViewNavigationDrag();
				doAreaEditing();
				break;
			}
		}
		else
		{
			// Update the UI if the mouse is in the UI
			uiStepTopEdge();

			// Do shortcut checking
			uiStepKeyboardShortcuts();
		}

		// Update portions of the UI if can be updated
		if ( m_current_mode == Mode::Properties ) uiStepShitPanel();

		// Do shortcut checking
		uiStepKeyboardShortcuts();
	}

	// Update visibilty of elements based on the modes
	m_tile_selector->SetVisible ( m_current_mode == Mode::TileEdit );
	ui_panel_shit->visible = m_current_mode == Mode::Properties;
}

//===============================================================================================//
// Update subroutines
//===============================================================================================//

// move the map around when middle button pressed
void MapEditor::doViewNavigationDrag ( void )
{
	m_navigation_busy = false;
	if ( Input::Mouse( Input::MBMiddle ) || ( Input::Mouse( Input::MBLeft ) && Input::Key( Keys.Control ) ) )
	{
		m_navigation_busy = true;
		m_target_camera->transform.position -= Vector2d(
			Input::DeltaMouseX(), Input::DeltaMouseY()
			);
	}
	if ( Input::DeltaMouseW() != 0 )
	{
		m_target_camera->transform.position.y += Input::DeltaMouseW() * 0.05f;
	}
}

// edit the tile when clicking happens
void MapEditor::doTileEditing ( void )
{
	if ( !m_navigation_busy )
	{
		// Stop rebuild
		m_tilemap->ProcessPause();
		// Build div count needed 
		int divsNeeded = int(Vector2d( Input::DeltaMouseX(),Input::DeltaMouseY() ).magnitude() / std::min( m_tilemap->m_tileset->tilesize_x,m_tilemap->m_tileset->tilesize_y )) + 1;
		for ( int i = 0; i <= divsNeeded; ++i )
		{
			// Do tile editing across the entire range to compensate for lower framerates
			float percent = i / (float)divsNeeded;
			_doTileEditingSub( Input::MouseX() - Input::DeltaMouseX() * percent, Input::MouseY() - Input::DeltaMouseY() * percent );
		}
		// Resume rebuild
		m_tilemap->ProcessResume();
	}
}
void MapEditor::_doTileEditingSub ( float mousex, float mousey )
{
	// Grab mouse position in the world
	Vector3d worldpos = m_target_camera->ScreenToWorldPos( Vector2d( mousex/(Real)Screen::Info.width, mousey/(Real)Screen::Info.height ) );
	// convert worldpos to indexes
	int ix = (int) (worldpos.x / m_tilemap->m_tileset->tilesize_x); ix *= m_tilemap->m_tileset->tilesize_x;
	int iy = (int) (worldpos.y / m_tilemap->m_tileset->tilesize_y); iy *= m_tilemap->m_tileset->tilesize_y;

	// Generate limits
	int min_x = 0;
	int min_y = 0;
	int max_x = (m_mapinfo->tilesize_x+0)*m_tilemap->m_tileset->tilesize_x;
	int max_y = (m_mapinfo->tilesize_y+0)*m_tilemap->m_tileset->tilesize_y;

	// Tile addition/change
	if ( Input::Mouse( Input::MBLeft ) )
	{
		// loop through tiles to find the one under the mouse
		bool changed_tile = false;
		int next_tile = m_tile_selector->GetTileSelection();
		for ( auto itr_tile = m_tilemap->m_tiles.begin(); itr_tile != m_tilemap->m_tiles.end(); ++itr_tile )
		{
			if ( itr_tile->x == ix && itr_tile->y == iy )
			{
				changed_tile = true;
				// Perform change if there's a difference
				if ( itr_tile->type != next_tile )
				{
					itr_tile->type = next_tile;
					// Rebuild the map visuals
					m_tilemap->Rebuild();
				}
				break;
			}
		}
		// There was no tile there. May need to make a new tile.
		if ( changed_tile == false )
		{
			// Make sure the tile is in range
			if ( ix >= min_x && iy >= min_x && ix < max_x && iy < max_y )
			{
				// Create a new tile object
				mapTile_t tile;
				tile.type = next_tile;
				tile.depth = 0;
				tile.collision_override = 0;
				tile.x = ix;
				tile.y = iy;
				// Add the tile to the map
				m_tilemap->m_tiles.push_back(tile);
				// Rebuild the map visuals
				m_tilemap->Rebuild();
			}
		}
	}
	else if ( Input::Mouse( Input::MBRight ) )
	{
		// loop through tiles to find the one under the mouse
		int next_tile = m_tile_selector->GetTileSelection();
		for ( auto itr_tile = m_tilemap->m_tiles.begin(); itr_tile != m_tilemap->m_tiles.end(); ++itr_tile )
		{
			if ( itr_tile->x == ix && itr_tile->y == iy )
			{
				// remove the tile from it all
				m_tilemap->m_tiles.erase(itr_tile);
				// Rebuild the map visuals
				m_tilemap->Rebuild();
				break;
			}
		}
	}
	else if ( Input::Keypress( 'F' ) )
	{

	}
}

//		doAreaEditing () : area editing
// do area editing and such when clicking happens
void MapEditor::doAreaEditing ( void )
{
	// First do a debug render of all areas
	/*for ( auto area = Engine2D::Area2D::Areas().begin(); area != Engine2D::Area2D::Areas().end(); ++area )
	{
		Rect rect = (*area)->m_rect;
		Vector3d offset = Vector3d( 0,0,-50 );
		Debug::Drawer->DrawLine( rect.pos + offset, rect.pos + Vector2d( rect.size.x,0 ) + offset );
		Debug::Drawer->DrawLine( rect.pos + offset, rect.pos + Vector2d( 0,rect.size.y ) + offset );
		Debug::Drawer->DrawLine( rect.pos + Vector2d( rect.size.x,0 ) + offset, rect.pos + rect.size + offset );
		Debug::Drawer->DrawLine( rect.pos + Vector2d( 0,rect.size.y ) + offset, rect.pos + rect.size + offset );
	}*/

	// Click to add 
	if ( !m_navigation_busy )
	{
		// Grab mouse position in the world
		Vector3d worldpos = m_target_camera->ScreenToWorldPos( Vector2d( Input::MouseX()/(Real)Screen::Info.width, Input::MouseY()/(Real)Screen::Info.height ) );

		if ( Input::MouseDown( Input::MBLeft ) )
		{
			if ( Input::Key( Keys.Shift ) )
			{
				Engine2D::Area2D* area = new Engine2D::Area2D();
				area->m_rect.pos = worldpos;
				area->m_rect.size = Vector2d( 32,32 );
				area->RemoveReference();
				m_current_submode = SubMode::Dragging;
				m_area_target = area;
				m_area_corner_selection = 2;
			}
			else
			{
				// Check all areas to find the best match
			}
		}

		if ( !Input::Mouse( Input::MBLeft ) )
		{
			m_current_submode = SubMode::None;
		}

		if ( m_current_submode == SubMode::Dragging && m_area_target != NULL && m_area_corner_selection >= 0 )
		{
			// Move the x coordinate of the rect
			if ( m_area_corner_selection == 1 || m_area_corner_selection == 2 )
				m_area_target->m_rect.size.x = worldpos.x - m_area_target->m_rect.pos.x;
			else {
				m_area_target->m_rect.size.x = -(worldpos.x - m_area_target->m_rect.pos.x) + m_area_target->m_rect.size.x;
				m_area_target->m_rect.pos.x = worldpos.x;
			}
			// Move the y coordinate of the rect
			if ( m_area_corner_selection == 2 || m_area_corner_selection == 3 )
				m_area_target->m_rect.size.y = worldpos.y - m_area_target->m_rect.pos.y;
			else {
				m_area_target->m_rect.size.y = -(worldpos.y - m_area_target->m_rect.pos.y) + m_area_target->m_rect.size.y;
				m_area_target->m_rect.pos.y = worldpos.y;
			}

			// Keep the X size positive
			if ( m_area_target->m_rect.size.x < 0 )
			{
				if ( m_area_corner_selection == 0 )
					m_area_corner_selection = 1;
				else if ( m_area_corner_selection == 1 )
					m_area_corner_selection = 0;

				else if ( m_area_corner_selection == 2 )
					m_area_corner_selection = 3;
				else if ( m_area_corner_selection == 3 )
					m_area_corner_selection = 2;

				m_area_target->m_rect.pos.x += m_area_target->m_rect.size.x;
				m_area_target->m_rect.size.x *= -1;
			}
			// Keep the Y size positive
			if ( m_area_target->m_rect.size.y < 0 )
			{
				if ( m_area_corner_selection == 0 )
					m_area_corner_selection = 3;
				else if ( m_area_corner_selection == 3 )
					m_area_corner_selection = 0;

				else if ( m_area_corner_selection == 2 )
					m_area_corner_selection = 1;
				else if ( m_area_corner_selection == 1 )
					m_area_corner_selection = 2;

				m_area_target->m_rect.pos.y += m_area_target->m_rect.size.y;
				m_area_target->m_rect.size.y *= -1;
			}
		}
	}
}

//		doMapResize () : resize the map
// using the size given in m_mapinfo structure, change the map size.
// fills layer zero with a bunch of default tiles and destroys any tiles out of range
void MapEditor::doMapResize ( void )
{
	int min_x = -m_tilemap->m_tileset->tilesize_x;
	int min_y = -m_tilemap->m_tileset->tilesize_y;
	int max_x = (m_mapinfo->tilesize_x+1)*m_tilemap->m_tileset->tilesize_x;
	int max_y = (m_mapinfo->tilesize_y+1)*m_tilemap->m_tileset->tilesize_y;
	// Loop through all the tiles to check their positions
	for ( auto itr_tile = m_tilemap->m_tiles.begin(); itr_tile != m_tilemap->m_tiles.end(); )
	{
		// Remove all tiles out of range
		if ( itr_tile->x < min_x || itr_tile->y < min_y || itr_tile->x >= max_x || itr_tile->y >= max_y ) {
			itr_tile = m_tilemap->m_tiles.erase(itr_tile);
		}
		else {
			++itr_tile;
		}
	}
	// Force a rebuild at this point
	m_tilemap->Rebuild();
}

//===============================================================================================//
// IO saving shit
//===============================================================================================//

//		doIOSaving () : saving
// save the map to the file in m_current_savetarget
void MapEditor::doIOSaving ( void )
{
	// Open the file
	FILE* fp = fopen( m_current_savetarget.c_str(), "wb" );
	if ( fp == NULL )
		throw Core::NullReferenceException();

	// Create mapio, set options
	M04::MapIO io;
	io.m_file = fp;
	io.m_mapinfo = m_mapinfo;
	io.m_tilemap = m_tilemap;

	// save
	io.Save();

	// Close file
	fclose( fp );
}

//===============================================================================================//
// UI Specific
//===============================================================================================//

//		uiCreate () : create the dusk UI
// create entirety of the dusk gui shit
void MapEditor::uiCreate ( void )
{
	// Top bar
	{
		Dusk::Handle panel;
		Dusk::Handle button, label;

		// Create the panel
		panel = dusk->CreateEdgePanel();
		panel.SetRect( Rect(0,0,1280,40) );

		// Create labels
		label = dusk->CreateText( panel, "FILE" );
		label.SetRect(Rect(20,1,0,0));

		label = dusk->CreateText( panel, "EDIT MODE" );
		label.SetRect(Rect(240,1,0,0));

		// Create file buttons
		button = dusk->CreateButton( panel );
		button.SetText("New");
		button.SetRect(Rect(50,5,45,30));
		ui_file_new = button;

		button = dusk->CreateButton( panel );
		button.SetText("Save");
		button.SetRect(Rect(100,5,45,30));
		ui_file_save = button;

		button = dusk->CreateButton( panel );
		button.SetText("Load");
		button.SetRect(Rect(150,5,45,30));
		ui_file_load = button;

		// Create mode buttons
		button = dusk->CreateButton( panel );
		button.SetText("Shit");
		button.SetRect(Rect(300,5,45,30));
		ui_mode_shit = button;

		button = dusk->CreateButton( panel );
		button.SetText("Map");
		button.SetRect(Rect(350,5,45,30));
		ui_mode_map = button;

		button = dusk->CreateButton( panel );
		button.SetText("Areas");
		button.SetRect(Rect(400,5,45,30));
		ui_mode_area = button;

		button = dusk->CreateButton( panel );
		button.SetText("Actors");
		button.SetRect(Rect(450,5,45,30));
		ui_mode_actors = button;
	}

	// Shit panel
	{
		Dusk::Handle panel;
		Dusk::Handle button, label, field;

		// Create the panel
		panel = dusk->CreatePanel();
		panel.SetRect( Rect(0,40,200,680) );
		ui_panel_shit = panel;

		// Create labels
		label = dusk->CreateText( panel, "S.H.I.T." );
		label.SetRect(Rect(11,1,0,0));
		label = dusk->CreateText( panel, "for map properties" );
		label.SetRect(Rect(11,21,0,0));

		// Create fields
		label = dusk->CreateText( panel, "Map Name" );
		label.SetRect(Rect(20,50,0,0));
		field = dusk->CreateTextfield( panel );
		field.SetRect(Rect(20,75,160,30) );
		ui_fld_map_name = field;

		label = dusk->CreateText( panel, "Area" );
		label.SetRect(Rect(20,100,0,0));
		label = dusk->CreateText( panel, "(eg \"mountains\" or \"ruins\")" );
		label.SetRect(Rect(20,125,0,0));
		ui_lbl_map_area = label;
		field = dusk->CreateTextfield( panel );
		field.SetRect(Rect(20,125,160,30) );
		ui_fld_map_area = field;

		label = dusk->CreateText( panel, "Map Size (in tiles)" );
		label.SetRect(Rect(20,150,0,0));
		field = dusk->CreateTextfield( panel, "40" );
		field.SetRect(Rect(20,175,70,30) );
		ui_fld_map_size_x = field;
		label = dusk->CreateText( panel, "x" );
		label.SetRect(Rect(95,175,0,0));
		field = dusk->CreateTextfield( panel, "40" );
		field.SetRect(Rect(110,175,70,30) );
		ui_fld_map_size_y = field;
		label = dusk->CreateText( panel, "in pixels: 1280 x 1280" );
		label.SetRect(Rect(20,200,0,0));
		ui_lbl_map_size = label;

		// Create button
		button = dusk->CreateButton( panel );
		button.SetText("Apply");
		button.SetRect(Rect(20,640,45,30));
		ui_btn_apply_shit = button;

		button = dusk->CreateButton( panel );
		button.SetText("Cancel");
		button.SetRect(Rect(80,640,45,30));
		ui_btn_cancel_shit = button;
	}
}

//===============================================================================================//

//		uiStepKeyboardShortcuts () : do shortcut shit
// handles keyboard inputs (shortcuts) to do things fast
void MapEditor::uiStepKeyboardShortcuts ( void )
{
	if ( Input::Key( Keys.Control ) && Input::Keydown( 'S' ) )
	{
		if ( m_current_savetarget.empty() )
		{
			System::sFileDialogueEntry filetypes [1];
			strcpy( filetypes[0].extension, "m04" );
			strcpy( filetypes[0].filetype, "OneEngine M04 Map" );
			ui_dg_save = dusk->DialogueSaveFilename(filetypes,1,"./.res-0/");
		}
		else
		{
			doIOSaving();
		}
	}
}

//		uiStepTopEdge () : top edge update
// handle inputs to the buttons on the top edge
void MapEditor::uiStepTopEdge ( void )
{
	if ( ui_file_save.GetButtonClicked() )
	{
		System::sFileDialogueEntry filetypes [1];
		strcpy( filetypes[0].extension, "m04" );
		strcpy( filetypes[0].filetype, "OneEngine M04 Map" );
		ui_dg_save = dusk->DialogueSaveFilename(filetypes,1,"./.res-0/");
	}
	if ( ui_file_load.GetButtonClicked() )
	{
		System::sFileDialogueEntry filetypes [1];
		strcpy( filetypes[0].extension, "m04" );
		strcpy( filetypes[0].filetype, "OneEngine M04 Map" );
		ui_dg_load = dusk->DialogueOpenFilename(filetypes,1,"./.res-0/");
	}

	if ( ui_mode_shit.GetButtonClicked() )
	{
		m_current_mode = Mode::Properties;
	}
	if ( ui_mode_map.GetButtonClicked() )
	{
		m_current_mode = Mode::TileEdit;
	}
	if ( ui_mode_area.GetButtonClicked() )
	{
		m_current_mode = Mode::AreaEdit;
	}
	if ( ui_mode_actors.GetButtonClicked() )
	{
		m_current_mode = Mode::ActorsEdit;
	}
}
//		uiStepDialogues () : dialogue polling
// handles dialogue inputs and performs actions based on inputs to them
void MapEditor::uiStepDialogues ( void )
{
	if ( dusk->SaveDialogueHasSelection(ui_dg_save) )
	{
		// Get the filename to use
		char filename [1024];
		dusk->GetSaveFilename( filename );
		printf( "FILE: %s\n", filename );
		string file = filename;
		if ( file.find(".m04") == string::npos ) {
			file += ".m04";
		}
		// Set save target
		m_current_savetarget = file;
		// Perform saving
		doIOSaving();
	}
	if ( dusk->OpenDialogueHasSelection(ui_dg_load) )
	{
		// Get the filename to use
		char filename [1024];
		dusk->GetOpenFilename( filename );
		printf( "FILE: %s\n", filename );
		string file = filename;
		if ( file.find(".m04") == string::npos ) {
			file += ".m04";
		}
		// Set save target
		m_current_savetarget = file;

		// Open the file
		FILE* fp = fopen( file.c_str(), "rb" );
		if ( fp == NULL )
			throw Core::NullReferenceException();

		// Create mapio, set options
		M04::MapIO io;
		io.m_file = fp;
		io.m_mapinfo = m_mapinfo;
		io.m_tilemap = m_tilemap;

		// load
		io.Load();

		// Close file
		fclose( fp );

		// Reset UI states
		uiDoShitRefresh();
	}
}

//		uiStepShitPanel () : S.H.I.T. panel update
// handles input and updates to the shit panel
void MapEditor::uiStepShitPanel ( void )
{
	// Update the map area hint label
	{
		string areaInfo;
		dusk->GetTextfieldData( ui_fld_map_area, areaInfo );
		if ( areaInfo.empty() )
			ui_lbl_map_area.SetVisible( true );
		else
			ui_lbl_map_area.SetVisible( false );
	}
	// Update the sizer
	string s_sizer;
	int sizex, sizey;
	dusk->GetTextfieldData( ui_fld_map_size_x, s_sizer );
	sizex = atoi( s_sizer.c_str() );
	if ( sizex == 0 ) {
		sizex = m_mapinfo->tilesize_x;
	}
	dusk->GetTextfieldData( ui_fld_map_size_y, s_sizer );
	sizey = atoi( s_sizer.c_str() );
	if ( sizey == 0 ) {
		sizey = m_mapinfo->tilesize_y;
	}
	// Set sizer string
	ui_lbl_map_size.SetText( "in pixels: " + std::to_string(sizex*m_tilemap->m_tileset->tilesize_x) + " x " + std::to_string(sizey*m_tilemap->m_tileset->tilesize_y) );

	// Apply new map properties
	if ( ui_btn_apply_shit.GetButtonClicked() )
	{
		string s_temp;
		// Set all values in the M04::MapInformation structure
		m_mapinfo->tilesize_x = sizex;
		m_mapinfo->tilesize_y = sizey;
		dusk->GetTextfieldData( ui_fld_map_name, s_temp );
		m_mapinfo->map_name = s_temp.c_str();
		dusk->GetTextfieldData( ui_fld_map_area, s_temp );
		m_mapinfo->area_name = s_temp.c_str();
		// Apply new map options
		doMapResize();
	}
	// Cancel all changes
	if ( ui_btn_cancel_shit.GetButtonClicked() )
	{	// Just reload all fields
		uiDoShitRefresh();
	}
}
//		uiDoShitRefresh () : S.H.I.T. panel reinit
// resets all input in the shit panel with the current values
void MapEditor::uiDoShitRefresh ( void )
{
	ui_fld_map_name.SetText( string( m_mapinfo->map_name ) );
	ui_fld_map_area.SetText( string( m_mapinfo->area_name ) );
	ui_fld_map_size_x.SetText( std::to_string( m_mapinfo->tilesize_x ) );
	ui_fld_map_size_y.SetText( std::to_string( m_mapinfo->tilesize_y ) );
}

//===============================================================================================//
// end of code
//===============================================================================================//

//===============================================================================================//
// Old unused useful code:
//===============================================================================================//

		/*tilemap->SetSpriteFile( "textures/ruins.png" );
		// Set the tileset information
		Engine2D::Tileset* tileset = new Engine2D::Tileset ();
		tileset->atlassize_x = 256;
		tileset->atlassize_y = 256;
		tileset->tilecount_x = 8;
		tileset->tilecount_y = 8;
		tileset->tilesize_x = 32;
		tileset->tilesize_y = 32;
		tilemap->SetTileset( tileset );*/

//===============================================================================================//