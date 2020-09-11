#include <map>

#include "core/input/CInput.h"
#include "core/math/Math.h"
#include "core/settings/CGameSettings.h"
#include "core/system/io/FileUtils.h"
#include "core/system/Screen.h"
#include "core/system/System.h"

#include "engine/utils/CDeveloperConsole.h"

#include "engine-common/dusk/UI.h"
#include "engine-common/dusk/controls/Button.h"
#include "engine-common/dusk/controls/Panel.h"
#include "engine-common/dusk/controls/Label.h"
#include "engine-common/dusk/controls/DockablePanel.h"
#include "engine-common/dusk/controls/DropdownList.h"
#include "engine-common/dusk/controls/Slider.h"
#include "engine-common/dusk/layouts/Horizontal.h"
#include "engine-common/dusk/layouts/Vertical.h"
#include "engine-common/entities/CRendererHolder.h"

#include "renderer/state/Settings.h"
#include "renderer/texture/RrFontTexture.h"
#include "renderer/camera/RrCamera.h"
#include "renderer/debug/RrDebugDrawer.h"
#include "renderer/light/RrLight.h"
#include "renderer/material/Material.h"
#include "render2d/camera/COrthoCamera.h"
#include "render2d/object/Background2D.h"
#include "render2d/object/TileMapLayer.h"

#include "engine2d/entities/map/TileMap.h"
#include "engine2d/entities/map/CollisionMap.h"
#include "engine2d/entities/Area2DBase.h"
#include "engine2d/entities/AreaTeleport.h"
#include "engine2d/entities/AreaTrigger.h"
#include "engine2d/entities/AreaPlayerSpawn.h"

#include "m04/m04-common.h"

#include "m04/states/MapInformation.h"
#include "m04/interfaces/MapIO.h"

#include "m04-editor/standalone/mapeditor/ObjectEditorListing.h"
#include "m04-editor/entities/UIDragHandle.h"
#include "m04-editor/entities/UILightHandle.h"

#include "m04-editor/standalone/mapeditor/TileSelector.h"
#include "m04-editor/standalone/mapeditor/EditorObject.h"

#include "m04-editor/renderer/object/AreaRenderer.h"
#include "m04-editor/renderer/object/CollisionMapRenderer.h"
#include "m04-editor/renderer/object/GizmoRenderer.h"

#include "MapEditor.h"

using namespace M04;

MapEditor::MapEditor ( void )
	: CGameBehavior(),
	m_current_mode(Mode::TileEdit), m_current_submode(SubMode::None), m_navigation_busy(false),
	m_current_savetarget(""),
	m_drag_handle(NULL),
	m_area_target(NULL), m_object_target(NULL)
{
	// Create orthographic camera
	{
		COrthoCamera* cam = new COrthoCamera();
		// Set camera options
		cam->pixel_scale_mode = orthographicScaleMode_t::ORTHOSCALE_MODE_SIMPLE;
		cam->viewport_target.size = Vector2f( 1280,720 ) * 0.5f;
		cam->SetActive(); // Mark it as the main camera to use IMMEDIATELY
						  // Use this new camera as our main camera
		m_target_camera = cam;
	}
	// Create background
	{
		CRenderable2D* bg = new renderer::Background2D();
		(new CRendererHolder (bg))->RemoveReference();
	}

	// Load up the options file
	{
		statusLoad();
	}

	// Load editor listing
	{
		m_listing = new M04::ObjectEditorListing ();
		m_listing->LoadListing( CGameSettings::Active()->sysprop_editor.c_str() );
	}
	// Create map info
	{
		m_mapinfo = new M04::MapInformation ();
		// Set default map size
		m_mapinfo->tilesize_x = 40;
		m_mapinfo->tilesize_y = 40;
		// Set default ambient
		m_mapinfo->env_ambientcolor = 0x808080FF;
		renderer::Settings.ambientColor.SetCode( m_mapinfo->env_ambientcolor );
	}
	// Craete tile map
	{
		Engine2D::TileMap* tilemap = new Engine2D::TileMap();
		tilemap->RemoveReference(); // So it can be destroyed when the game quits

		// Set the tileset sprite
		tilemap->SetTilesetFile( "tileset/ld40.txt" );

		// Set map data
		tilemap->SetDebugTileMap(
			m_mapinfo->tilesize_x,m_mapinfo->tilesize_y	// size of map in tiles
		);

		// Rebuild the map visuals
		tilemap->Rebuild();
		
		// Save it
		m_tilemap = tilemap;
	}

	// Create area renderer
	{
		m_area_renderer = new M04::AreaRenderer();
		m_area_renderer->transform.world.position.z = -99;
	}
	// Create gizmo renderer
	{
		m_gizmo_renderer = new M04::GizmoRenderer();
		m_gizmo_renderer->transform.world.position.z = -100;
	}
	// Build Dusk Gui
	{
		dusk = new dusk::UserInterface;
		//dusk->SetPixelMode(true);

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
	// Create the collision and rebuild
	{
		m_collisionmap = new Engine2D::CollisionMap();
		m_collisionmap->RemoveReference(); // So it can be destroyed when the game quits

		// Point at the tilemap
		m_collisionmap->m_tilemap = m_tilemap;

		// Rebuild it now
		m_collisionmap->Rebuild();
	}
	// Create the collision visualizer
	{
		m_tile_collision_renderer = new M04::CollisionMapRenderer();
		m_tile_collision_renderer->m_collision = m_collisionmap;
		m_tile_collision_renderer->transform.world.position.z = -99;
		m_tile_collision_renderer->m_drawSolids = false;
		m_tile_collision_renderer->m_drawWireframe = true;
	}

	// Load up the level (if it's there)
	if (!m_current_savetarget.empty())
	{
		FILE* fp = fopen(m_current_savetarget.c_str(), "rb");
		if (fp != NULL)
		{
			fclose(fp);
			doIOLoading();
		}
	}
}

MapEditor::~MapEditor ( void )
{
	statusSave();

	delete_safe(m_target_camera);
	delete_safe_decrement(dusk);
	delete_safe(m_tile_selector);
	delete_safe(m_area_renderer);
	delete_safe(m_gizmo_renderer);
	
	delete_safe(m_listing);
	delete_safe_decrement(m_drag_handle);
	delete_safe_decrement(m_light_handle);

	delete_safe(m_tile_collision_renderer);
}

//===============================================================================================//
// Option Subroutines
//===============================================================================================//

//		statusLoad () : load status from file
// opens .game/.editorpersistent, loads options
void MapEditor::statusLoad ( void )
{
	FILE* fp = fopen(".game/.editorpersistent", "rb");
	if ( fp != NULL )
	{
		grEditorPersistentOptions options;
		size_t readSize = fread(&options, sizeof(grEditorPersistentOptions), 1, fp);
		if (readSize == 1)
		{
			// Load up the target
			m_current_savetarget = options.current_file.c_str();
			// Load up the prev camera position
			m_target_camera_position.x = options.camera_position.x;
			m_target_camera_position.y = options.camera_position.y;
		}
		fclose(fp);
	}
}
//		statusSave () : saves status to file
// saves options to .game/.editorpersistent
void MapEditor::statusSave ( void )
{
	FILE* fp = fopen(".game/.editorpersistent", "wb");
	if ( fp != NULL )
	{
		grEditorPersistentOptions options;
		options.current_file = m_current_savetarget.c_str();
		options.camera_position = m_target_camera_position;

		fwrite(&options, sizeof(grEditorPersistentOptions), 1, fp);
		fclose(fp);
	}
}

//===============================================================================================//
// Update
//===============================================================================================//

void MapEditor::Update ( void )
{
	// Update renderer
	renderer::Settings.ambientColor.SetCode( m_mapinfo->env_ambientcolor );

	// Update the editor
	//dusk::Element* openDialogue = dusk->GetOpenDialogue();
	//if ( dusk->HasOpenDialogue() && openDialogue != ui_fld_area_type && openDialogue != ui_fld_object_type )
	//if ( dusk->HasOpenDialogue() && (openDialogue == ui_dg_save || openDialogue == ui_dg_load) )
	if (ui_dg_save->m_isOpen || ui_dg_load->m_isOpen)
	{
		// Close all modes for the dialogue system
		m_current_mode = Mode::None;
	
		// Update the UI for the dialogues
		uiStepDialogues();
	}
	else
	{
		//if ( !dusk->GetMouseInGUI() && !dusk->HasOpenDialogue() )
		if ( !dusk->IsMouseInside() )
		{
			// Enable navigation events
			m_preclude_navigation = false;

			// Only do map mouse events if the mouse is not in the dusk GUI
			switch ( m_current_mode )
			{
			case Mode::None:
			case Mode::UtilityEdit:
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

			case Mode::ObjectEdit:
				if ( Input::Key( Keys.Shift ) && Input::Key( Keys.Alt ) )
				{
					m_preclude_navigation = true;
				}
				doViewNavigationDrag();
				doObjectEditing();
				break;
			}
		}
		else
		{
			// Save current mode in case changes in next inputs
			Mode previous_mode = m_current_mode;

			// Update the UI if the mouse is in the UI
			uiStepTopEdge();
			// Do shortcut checking
			uiStepKeyboardShortcuts();

			// Check for mode changes
			if ( previous_mode != m_current_mode )
			{
				if ( previous_mode == Mode::TileEdit )
					_uiStepTilePanel_End();
			}
		}

		// Update portions of the UI if can be updated
		if ( m_current_mode == Mode::Properties ) uiStepShitPanel();
		if ( m_current_mode == Mode::TileEdit ) uiStepTilePanel();
		if ( m_current_mode == Mode::AreaEdit ) uiStepAreaPanel();
		if ( m_current_mode == Mode::ObjectEdit ) uiStepObjectPanel();
		if ( m_current_mode == Mode::Preferences ) uiStepPreferencesPanel();

		// Update the lower status UI
		uiStepBottomEdge();

		// Do shortcut checking
		uiStepKeyboardShortcuts();
	}

	// Update visibilty of elements based on the modes
	m_tile_selector->SetVisible ( m_current_mode == Mode::TileEdit );
	ui_panel_tiles->m_visible = m_current_mode == Mode::TileEdit;
	ui_panel_shit->m_visible = m_current_mode == Mode::Properties;
	ui_panel_area->m_visible = m_current_mode == Mode::AreaEdit;
	ui_panel_object->m_visible = m_current_mode == Mode::ObjectEdit;
	ui_panel_preferences->m_visible = m_current_mode == Mode::Preferences;

	if ( m_current_mode != Mode::ObjectEdit )
	{
		_doObjectEditingSub_GizmoDisable();
	}

}

//===============================================================================================//
// Update subroutines
//===============================================================================================//

// move the map around when middle button pressed
void MapEditor::doViewNavigationDrag ( void )
{
	if ( m_preclude_navigation == false )
	{
		m_navigation_busy = false;
		// Drag navigation
		if ( Input::Mouse( Input::MBMiddle ) || ( Input::Mouse( Input::MBRight ) && Input::Key( Keys.Control ) ) )
		{
			m_navigation_busy = true;
			m_target_camera_position -= Vector2f(
				Input::DeltaMouseX(), Input::DeltaMouseY()
			);
		}
		if ( Input::DeltaMouseW() != 0 )
		{
			m_target_camera_position.y += Input::DeltaMouseW() * 0.05f;
		}
		// Navigation via arrow keys for bonus usability
		if ( Input::Key( Keys.Left ) )  m_target_camera_position.x -= m_tilemap->m_tileset->tilesize_x * Time::deltaTime * 4.0F;
		if ( Input::Key( Keys.Right ) ) m_target_camera_position.x += m_tilemap->m_tileset->tilesize_x * Time::deltaTime * 4.0F;
		if ( Input::Key( Keys.Up ) )	m_target_camera_position.y -= m_tilemap->m_tileset->tilesize_y * Time::deltaTime * 4.0F;
		if ( Input::Key( Keys.Down ) )	m_target_camera_position.y += m_tilemap->m_tileset->tilesize_y * Time::deltaTime * 4.0F;

		// Update camera position
		m_target_camera->transform.position = m_target_camera_position;
	}
}

// edit the tile when clicking happens
void MapEditor::doTileEditing ( void )
{
	if ( !m_navigation_busy )
	{
		if ( m_current_submode == SubMode::TilesVisual )
		{
			// Stop rebuild
			m_tilemap->ProcessPause();
			// Build div count needed 
			int divsNeeded = int(Vector2f( Input::DeltaMouseX(),Input::DeltaMouseY() ).magnitude() / std::min( m_tilemap->m_tileset->tilesize_x,m_tilemap->m_tileset->tilesize_y )) + 1;
			for ( int i = 0; i <= divsNeeded; ++i )
			{
				// Do tile editing across the entire range to compensate for lower framerates
				float percent = i / (float)divsNeeded;
				_doTileEditingSub( Input::MouseX() - Input::DeltaMouseX() * percent, Input::MouseY() - Input::DeltaMouseY() * percent );
			}
			// Resume rebuild
			m_tilemap->ProcessResume();

			// Rebuild the collision map
			m_collisionmap->m_tilemap = m_tilemap;
			m_collisionmap->Rebuild();
			
			// Draw only wireframe
			m_tile_collision_renderer->m_drawSolids = false;
			m_tile_collision_renderer->m_drawWireframe = true;
		}
		else if ( m_current_submode == SubMode::TilesCollision )
		{
			// Draw solids for easier view
			m_tile_collision_renderer->m_drawSolids = true;
			m_tile_collision_renderer->m_drawWireframe = true;
		}
		else if ( m_current_submode == SubMode::TilesHeight )
		{
		}
	}
}
void MapEditor::_doTileEditingSub ( float mousex, float mousey )
{
	// Grab mouse position in the world
	Vector3f worldpos = m_target_camera->ScreenToWorldPos( Vector2f( mousex/(Real)Screen::Info.width, mousey/(Real)Screen::Info.height ) );
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
			if ( itr_tile->x == ix && itr_tile->y == iy && itr_tile->depth == m_tile_layer_current )
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
			if ( Input::Key( Keys.Shift ) || (ix >= min_x && iy >= min_x && ix < max_x && iy < max_y) )
			{
				// Create a new tile object
				mapTile_t tile;
				tile.type = next_tile;
				tile.depth = m_tile_layer_current;
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
			if ( itr_tile->x == ix && itr_tile->y == iy && itr_tile->depth == m_tile_layer_current )
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
	// Click to add 
	if ( !m_navigation_busy )
	{
		// Grab mouse position in the world
		Vector3f worldpos = m_target_camera->ScreenToWorldPos( Vector2f( Input::MouseX()/(Real)Screen::Info.width, Input::MouseY()/(Real)Screen::Info.height ) );

		Engine2D::Area2D* t_area_selection = NULL;
		int t_corner_selection = -1;
		// Grab area list
		std::vector<Engine2D::Area2D*> t_area_listing = Engine2D::Area2D::Areas();
		// If there's a selection, put that at the front of the list
		if ( m_area_target != NULL ) t_area_listing.insert( t_area_listing.begin(), m_area_target );
		// Check all areas to find the best match
		for ( auto area = t_area_listing.begin(); area != t_area_listing.end(); ++area )
		{
			Rect rect = (*area)->m_rect;
			rect.pos -= Vector2f(1,1);
			rect.size += Vector2f(1,1)*2;
			if ( rect.Contains( worldpos ) )
			{
				// Mouse is in, use this
				t_area_selection = *area;
				// Check the corners for shit
				if ( worldpos.x < rect.pos.x + 6 ) {
					if ( worldpos.y < rect.pos.y + 6 ) {
						t_corner_selection = 0;
					}
					else if ( worldpos.y > rect.pos.y + rect.size.y - 6 ) {
						t_corner_selection = 3;
					}
				}
				else if ( worldpos.x > rect.pos.x + rect.size.x - 6 ) {
					if ( worldpos.y < rect.pos.y + 6 ) {
						t_corner_selection = 1;
					}
					else if ( worldpos.y > rect.pos.y + rect.size.y - 6 ) {
						t_corner_selection = 2;
					}
				}
				// We're going with the first match
				break;
			}
		}
		// Set tips for the area renderer
		m_area_renderer->m_target_glow = t_area_selection;
		m_area_renderer->m_target_corner = t_corner_selection;

		if ( Input::MouseDown( Input::MBLeft ) )
		{
			if ( Input::Key( Keys.Shift ) )
			{
				// Create a new area
				Engine2D::Area2D* area = new Engine2D::Area2D();
				area->m_rect.pos = worldpos;
				area->m_rect.size = Vector2f( 32,32 );
				area->RemoveReference();
				m_current_submode = SubMode::Dragging;
				m_area_target = area;
				m_area_corner_selection = 2;
				// Set tips for the area renderer
				m_area_renderer->m_target_selection = m_area_target;
			}
			else
			{
				// Set target to what the mouse is hovering over
				m_area_target = t_area_selection;
				m_area_corner_selection = t_corner_selection;
				// Set tips for the area renderer
				m_area_renderer->m_target_selection = m_area_target;
				// If clicked a corner, do some dragging.
				if ( m_area_target != NULL && m_area_corner_selection != -1 ) {
					m_current_submode = SubMode::Dragging;
				}
			}
		}
		// Mouse released. Round the position, reset the mode.
		if ( !Input::Mouse( Input::MBLeft ) )
		{
			if ( m_area_target != NULL )
			{
				// Round the rect size
				m_area_target->m_rect.size.x = (Real)math::round(m_area_target->m_rect.size.x);
				m_area_target->m_rect.size.y = (Real)math::round(m_area_target->m_rect.size.y);
				// Round the rect position
				m_area_target->m_rect.pos.x = (Real)math::round(m_area_target->m_rect.pos.x);
				m_area_target->m_rect.pos.y = (Real)math::round(m_area_target->m_rect.pos.y);
			}
			m_current_submode = SubMode::None;
			
			// Deselect areas on right click
			if ( Input::MouseDown( Input::MBRight ) )
			{
				m_area_target = NULL;
				// Delete tiles with shift right click
				if ( Input::Key( Keys.Shift ) )
				{
					if ( t_area_selection != NULL ) {
						delete t_area_selection;
					}
				}
			}
		}
		// Are we dragging a corner to edit the area?
		if ( m_current_submode == SubMode::Dragging && m_area_target != NULL && m_area_corner_selection >= 0 )
		{
			// Snap to half tile
			if ( Input::Key( Keys.Alt ) )
			{
				worldpos.x = (Real) math::round( worldpos.x * 2 / m_tilemap->m_tileset->tilesize_x ) * m_tilemap->m_tileset->tilesize_x * 0.5F;
				worldpos.y = (Real) math::round( worldpos.y * 2 / m_tilemap->m_tileset->tilesize_y ) * m_tilemap->m_tileset->tilesize_y * 0.5F;
			}

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
		// Check for area deletion
		if ( Input::Key( Keys.Delete ) )
		{
			if ( m_area_target != NULL )
			{
				delete m_area_target;
				m_area_target = NULL;
			}
		}
	}
}

//		doObjectEditing () : actor editing
// do actor selection, moving, deleting, and such
void MapEditor::doObjectEditing ( void )
{
	// Click to add 
	if ( !m_navigation_busy )
	{
		// Grab mouse position in the world
		Vector3f worldpos = m_target_camera->ScreenToWorldPos( Vector2f( Input::MouseX()/(Real)Screen::Info.width, Input::MouseY()/(Real)Screen::Info.height ) );
		worldpos.z = 0;

		M04::EditorObject* t_object_selection = NULL;
		// Grab area list
		std::vector<M04::EditorObject*> t_object_listing = M04::EditorObject::Objects();
		// If there's a selection, put that at the front of the list
		if ( m_object_target != NULL ) t_object_listing.insert( t_object_listing.begin(), m_object_target );
		// Check all areas to find the best match
		for ( auto object = t_object_listing.begin(); object != t_object_listing.end(); ++object )
		{
			Rect rect = (*object)->GetSpriteRect();
			rect.pos += (*object)->position;
			if ( rect.Contains( worldpos ) )
			{
				// Mouse is in, use this
				t_object_selection = *object;
				// We're going with the first match
				break;
			}
		}

		// Set tips for the gizmo itself
		if ( m_drag_handle == NULL ) {
			m_drag_handle = new UIDragHandle();
			m_drag_handle->active = false;
		}
		if ( m_light_handle == NULL ) {
			m_light_handle = new UILightHandle();
			m_light_handle->active = false;
		}

		// Set tips for the gizmo renderer: Update UI glowing
		m_gizmo_renderer->m_target_glow = t_object_selection;
		m_gizmo_renderer->m_target_selection = m_object_target;

		if ( Input::MouseDown( Input::MBLeft ) )
		{
			if ( Input::Key( Keys.Shift ) && Input::Key( Keys.Alt ) )
			{
				// Create the object
				EditorObject* object = new EditorObject( ui_fld_object_type->as<dusk::elements::DropdownList<int>>()->Selection().first.c_str() );
				object->position = worldpos;
				object->RemoveReference();
				// Set UI
				m_object_target = object;
				_doObjectEditingSub_GizmoEnable();
			}
			else if ( !m_drag_handle->HasFocus() && !m_light_handle->HasFocus() )
			{
				// Select the object
				m_object_target = t_object_selection;
				// Set UI
				_doObjectEditingSub_GizmoEnable();
				_uiStepObjectPanelSub_ClearProperties();
			}
		}
		// Update properties based on the gizmo
		if ( m_object_target != NULL )
		{
			// Transform gizmo:
			m_object_target->position = m_drag_handle->GetGizmoPosition();
			// Transform gizmo position snapping:
			if ( m_drag_handle->HasFocus() && Input::Key( Keys.Alt ) )
			{
				// Set visual snapping
				m_drag_handle->SetSnapping( Vector2f( m_tilemap->m_tileset->tilesize_x * 0.5F, m_tilemap->m_tileset->tilesize_y * 0.5F ) );
				Vector3f objpos = m_object_target->position;
				// Snap to half-tile
				objpos.x = (Real) math::round( objpos.x * 2 / m_tilemap->m_tileset->tilesize_x ) * m_tilemap->m_tileset->tilesize_x * 0.5F;
				objpos.y = (Real) math::round( objpos.y * 2 / m_tilemap->m_tileset->tilesize_y ) * m_tilemap->m_tileset->tilesize_y * 0.5F;
				// Set positions to snapped values
				m_object_target->position = objpos;
				m_drag_handle->SetRenderPosition( Vector3f(objpos.x, objpos.y, -495.0F) );
				//m_drag_handle->SetRenderPosition( Vector3f(objpos.x, objpos.y, objpos.z) );
			}
			else
			{	// Reset snapping of tool
				m_drag_handle->SetSnapping( Vector2f(0,0) );
			}

			// Light gizmo:
			if ( m_object_target->light != NULL )
			{
				m_object_target->light->falloff_range = m_light_handle->GetRange();
				m_object_target->light->falloff_invpower = m_light_handle->GetPower();
				// Update gizmo position
				m_light_handle->SetRenderPosition( m_object_target->position );
			}
		}
		// Check for object deletion
		if ( Input::Key( Keys.Delete ) )
		{
			if ( m_object_target != NULL )
			{
				_doObjectEditingSub_GizmoDisable();

				delete m_object_target;
				m_object_target = NULL;
			}
		}
	}
}
void MapEditor::_doObjectEditingSub_GizmoEnable ( void )
{
	if ( m_object_target != NULL )
	{
		// Enable transform gizmo
		m_drag_handle->active = true;
		m_drag_handle->SetRenderPosition( m_object_target->position );

		// Enable light gizmo
		if ( m_object_target->light != NULL )
		{
			m_light_handle->active = true;
			m_light_handle->SetRenderPosition( m_object_target->position );
			m_light_handle->SetRange( m_object_target->light->falloff_range );
			m_light_handle->SetPower( m_object_target->light->falloff_invpower );
		}
		else
		{
			m_light_handle->active = false;
		}
	}
	else
	{
		_doObjectEditingSub_GizmoDisable();
	}
}
void MapEditor::_doObjectEditingSub_GizmoDisable ( void )
{
	if ( m_drag_handle != NULL )
		m_drag_handle->active = false;
	if ( m_light_handle != NULL )
		m_light_handle->active = false;
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
		throw core::NullReferenceException();

	// Create mapio, set options
	M04::MapIO io;
	io.m_file = fp;
	io.m_mapinfo = m_mapinfo;
	io.m_tilemap = m_tilemap;
	io.m_io_areas = true;
	io.m_io_objects_editor = true;

	// save
	io.Save();

	// Close file
	fclose( fp );
}

//		doIOLoading () : loading
// load the map from the file in m_current_savetarget
// uses doNewMap() to clear out the data first
void MapEditor::doIOLoading ( void )
{
	// Reset map
	doNewMap();

	// Open the file
	FILE* fp = fopen( m_current_savetarget.c_str(), "rb" );
	if ( fp == NULL )
		throw core::NullReferenceException();

	// Create mapio, set options
	M04::MapIO io;
	io.m_file = fp;
	io.m_mapinfo = m_mapinfo;
	io.m_tilemap = m_tilemap;
	io.m_io_areas = true;
	io.m_io_objects_editor = true;

	// load
	io.Load();

	// Close file
	fclose( fp );

	// Do map finalization
	m_tile_selector->SetTileMap(m_tilemap);

	// Force an update now
	Update();
	// Update the UI to new values
	dusk->SetColorPicker( ui_fld_map_ambient_color, renderer::Settings.ambientColor );
}

//		doNewMap () : delete all items in map, clear out tilemap
// clears out all information for the map
void MapEditor::doNewMap ( void )
{
	// Delete all areas
	while ( !Engine2D::Area2D::Areas().empty() )
	{
		delete *Engine2D::Area2D::Areas().begin();
	}
	// Delete all objects
	while ( !M04::EditorObject::Objects().empty() )
	{
		delete *M04::EditorObject::Objects().begin();
	}
	// Delete all tiles
	m_tilemap->m_tiles.clear();
	m_tile_selector->SetTileMap(m_tilemap);

	// Clear out selection
	m_area_target = NULL;
	m_object_target = NULL;

	// Update tilemap
	try
	{
		m_tilemap->Rebuild();
		m_collisionmap->Rebuild();
	}
	catch (core::InvalidCallException&) {}
}

//===============================================================================================//
// UI Specific
//===============================================================================================//

std::map<string,int> lcl_areatype_map;

//		uiCreate () : create the dusk UI
// create entirety of the dusk gui shit
void MapEditor::uiCreate ( void )
{
	// Top bar
	{
		dusk::Element* label;

		// Create the panel
		dusk::elements::DockablePanel* panel = dusk->Add<dusk::elements::DockablePanel>( dusk::ElementCreationDescription{NULL, Rect( 0,0,1280,40 )} );
		panel->m_dockPosition = dusk::elements::DockPosition::kScreenTop;
		panel->m_dockFlags = dusk::elements::DockFlags::kLocked;

		// Create layout for the panel
		dusk::layouts::Horizontal* layout = dusk->Add<dusk::layouts::Horizontal>( dusk::LayoutCreationDescription{panel} );
		layout->m_margin = Vector2f(20, 5);
		layout->m_padding = Vector2f(5, 5);

		// File:

		label = dusk->Add<dusk::elements::Label>( dusk::ElementCreationDescription{layout, Rect(20,1,25,0)} );
		label->m_contents = "FILE";

		ui_file_new = dusk->Add<dusk::elements::Button>( dusk::ElementCreationDescription{layout, Rect(50,5,45,30)} );
		ui_file_new->m_contents = "New";

		ui_file_save = dusk->Add<dusk::elements::Button>( dusk::ElementCreationDescription{layout, Rect(100,5,45,30)} );
		ui_file_save->m_contents = "Save";

		ui_file_load = dusk->Add<dusk::elements::Button>( dusk::ElementCreationDescription{layout, Rect(150,5,45,30)} );
		ui_file_load->m_contents = "Load";

		// Mode:

		label = dusk->Add<dusk::elements::Label>( dusk::ElementCreationDescription{layout, Rect(240,1,55,0)} );
		label->m_contents = "EDIT MODE";

		ui_mode_shit = dusk->Add<dusk::elements::Button>( dusk::ElementCreationDescription{layout, Rect(300,5,45,30)} );
		ui_mode_shit->m_contents = "Shit";

		ui_mode_map = dusk->Add<dusk::elements::Button>( dusk::ElementCreationDescription{layout, Rect(350,5,45,30)} );
		ui_mode_map->m_contents = "Map";

		ui_mode_area = dusk->Add<dusk::elements::Button>( dusk::ElementCreationDescription{layout, Rect(400,5,45,30)} );
		ui_mode_area->m_contents = "Areas";

		ui_mode_object = dusk->Add<dusk::elements::Button>( dusk::ElementCreationDescription{layout, Rect(450,5,45,30)} );
		ui_mode_object->m_contents = "Objects";

		ui_mode_script = dusk->Add<dusk::elements::Button>( dusk::ElementCreationDescription{layout, Rect(500,5,45,30)} );
		ui_mode_script->m_contents = "Script";

		ui_mode_utils = dusk->Add<dusk::elements::Button>( dusk::ElementCreationDescription{layout, Rect(550,5,45,30)} );
		ui_mode_utils->m_contents = "Utils";

		// Toolbox:

		label = dusk->Add<dusk::elements::Label>( dusk::ElementCreationDescription{layout, Rect(640,1,55,0)} );
		label->m_contents = "TOOLBOX";

		ui_toolbox_cutscene = dusk->Add<dusk::elements::Button>( dusk::ElementCreationDescription{layout, Rect(700,5,95,30)} );
		ui_toolbox_cutscene->m_contents = "Cutscene Editor";

		ui_toolbox_global = dusk->Add<dusk::elements::Button>( dusk::ElementCreationDescription{layout, Rect(800,5,95,30)} );
		ui_toolbox_global->m_contents = "Global Settings";

		ui_toolbox_playtest = dusk->Add<dusk::elements::Button>( dusk::ElementCreationDescription{layout, Rect(1080,5,95,30)} );
		ui_toolbox_playtest->m_contents = "Playtest";

		ui_mode_preferences = dusk->Add<dusk::elements::Button>( dusk::ElementCreationDescription{layout, Rect(1180,5,95,30)} );
		ui_mode_preferences->m_contents = "Preferences...";
	}

	// Bottom bar
	{
		dusk::elements::DockablePanel* panel = dusk->Add<dusk::elements::DockablePanel>( dusk::ElementCreationDescription{NULL, Rect(0,690,1280,30)} );
		panel->m_dockPosition = dusk::elements::DockPosition::kScreenBottom;
		panel->m_dockFlags = dusk::elements::DockFlags::kLocked;

		// Create layout for the panel
		dusk::layouts::Horizontal* layout = dusk->Add<dusk::layouts::Horizontal>( dusk::LayoutCreationDescription{panel} );
		layout->m_margin = Vector2f(10, 1);
		layout->m_padding = Vector2f(10, 5);

		ui_lbl_mode		= dusk->Add<dusk::elements::Label>( dusk::ElementCreationDescription{layout, Rect(10,1,100,0)} );
		ui_lbl_mode->m_contents = "???";
		
		ui_lbl_mousex	= dusk->Add<dusk::elements::Label>( dusk::ElementCreationDescription{layout, Rect(200,1,100,0)} );
		ui_lbl_mousex->m_contents = "X: ???";
		ui_lbl_mousey	= dusk->Add<dusk::elements::Label>( dusk::ElementCreationDescription{layout, Rect(300,1,100,0)} );
		ui_lbl_mousey->m_contents = "Y: ???";

		ui_lbl_file		= dusk->Add<dusk::elements::Label>( dusk::ElementCreationDescription{layout, Rect(400,1,100,0)} );
		ui_lbl_file->m_contents = "";
	}

	// Map panel
	{
		dusk::Element* button;
		dusk::Element* label;

		// Create the panel
		ui_panel_tiles = dusk->Add<dusk::elements::Panel>( dusk::ElementCreationDescription{NULL, Rect(256, 40, 1280 - 256, 40)} );

		// Create labels
		label = dusk->Add<dusk::elements::Label>( dusk::ElementCreationDescription{ui_panel_tiles, Rect(10,5,0,0)} );
		label->m_contents = "LAYER";

		label = dusk->Add<dusk::elements::Label>( dusk::ElementCreationDescription{ui_panel_tiles, Rect(160,5,0,0)} );
		label->m_contents = "MODE";

		// Create layer thingies
		ui_btn_dec_layer = dusk->Add<dusk::elements::Button>( dusk::ElementCreationDescription{ui_panel_tiles, Rect(50,5,30,30)} );
		ui_btn_dec_layer->m_contents = "-";

		ui_fld_current_layer = dusk->Add<dusk::elements::Label>( dusk::ElementCreationDescription{ui_panel_tiles, Rect(90,5,0,0)} );
		ui_fld_current_layer->m_contents = "0";

		ui_btn_inc_layer = dusk->Add<dusk::elements::Button>( dusk::ElementCreationDescription{ui_panel_tiles, Rect(110,5,30,30)} );
		ui_btn_inc_layer->m_contents = "+";

		// Create edit mode toggles
		ui_btn_tile_mode_visual = dusk->Add<dusk::elements::Button>( dusk::ElementCreationDescription{ui_panel_tiles, Rect(200,5,50,30)} );
		ui_btn_tile_mode_visual->m_contents = "Visuals";

		ui_btn_tile_mode_collision = dusk->Add<dusk::elements::Button>( dusk::ElementCreationDescription{ui_panel_tiles, Rect(255,5,50,30)} );
		ui_btn_tile_mode_collision->m_contents = "Collision";

		ui_btn_tile_mode_height = dusk->Add<dusk::elements::Button>( dusk::ElementCreationDescription{ui_panel_tiles, Rect(310,5,50,30)} );
		ui_btn_tile_mode_height->m_contents = "Height";
	}

	// Shit panel
	{
		dusk::Element* button;
		dusk::elements::Label* label;
		dusk::Element* field;
		dusk::layouts::Horizontal* sublayout;

		// Create the panel
		dusk::elements::DockablePanel* panel = dusk->Add<dusk::elements::DockablePanel>( dusk::ElementCreationDescription{NULL, Rect(0,40,200,650)} );
		panel->m_dockPosition = dusk::elements::DockPosition::kScreenLeft;
		panel->m_dockFlags = dusk::elements::DockFlags::kLocked;
		panel->m_dockOrder = 2;
		ui_panel_shit = panel;

		// Create layout for the panel
		dusk::layouts::Vertical* layout = dusk->Add<dusk::layouts::Vertical>( dusk::LayoutCreationDescription{panel} );
		layout->m_margin = {10, 10};
		layout->m_padding = {0, 0};

		// Create labels
		label = dusk->Add<dusk::elements::Label>( dusk::ElementCreationDescription{layout, Rect(11,1,15,25)} );
		label->m_style = dusk::elements::kLabelStyle_Heading1;
		label->m_contents = "S.H.I.T.";
		label = dusk->Add<dusk::elements::Label>( dusk::ElementCreationDescription{layout, Rect(11,21,15,25)} );
		label->m_style = dusk::elements::kLabelStyle_Heading2;
		label->m_contents = "for map properties";

		// Create fields
		label = dusk->Add<dusk::elements::Label>( dusk::ElementCreationDescription{layout, Rect(20,50,20,25)} );
		label->m_contents = "Map Name";
		ui_fld_map_name = dusk->Add<dusk::elements::Textfield>( dusk::ElementCreationDescription{layout, Rect(20,75,160,30)} );

		label = dusk->Add<dusk::elements::Label>( dusk::ElementCreationDescription{layout, Rect(20,100,20,25)} );
		label->m_contents = "Area";
		ui_lbl_map_area = dusk->Add<dusk::elements::Label>( dusk::ElementCreationDescription{layout, Rect(20,125,20,25)} );
		ui_lbl_map_area->m_contents = "(eg \"mountains\" or \"ruins\")";
		ui_fld_map_area = dusk->Add<dusk::elements::Textfield>( dusk::ElementCreationDescription{layout, Rect(20,125,160,30)} );

		label = dusk->Add<dusk::elements::Label>( dusk::ElementCreationDescription{layout, Rect(20,150,20,25)} );
		label->m_contents = "Map Size (in tiles)";
		sublayout = dusk->Add<dusk::layouts::Horizontal>( dusk::LayoutCreationDescription{layout} );
		{
			ui_fld_map_size_x = dusk->Add<dusk::elements::Textfield>( dusk::ElementCreationDescription{sublayout, Rect(20,175,70,30)} );
			ui_fld_map_size_x->m_contents = "40";
			label = dusk->Add<dusk::elements::Label>( dusk::ElementCreationDescription{sublayout, Rect(95,175,15,15)} );
			label->m_contents = "x";
			ui_fld_map_size_y = dusk->Add<dusk::elements::Textfield>( dusk::ElementCreationDescription{sublayout, Rect(110,175,70,30)} );
			ui_fld_map_size_y->m_contents = "40";
		}
		ui_lbl_map_size = dusk->Add<dusk::elements::Label>( dusk::ElementCreationDescription{layout, Rect(20,200,0,0)} );
		ui_lbl_map_size->m_contents = "in pixels: 1280 x 1280";

		label = dusk->Add<dusk::elements::Label>( dusk::ElementCreationDescription{layout, Rect(20,250,20,25)} );
		label->m_contents = "Ambient Light";
		ui_fld_map_ambient_color = dusk->Add<dusk::elements::Colorpicker>( dusk::ElementCreationDescription{layout, Rect(20,275,70,30} );
		ui_fld_map_ambient_color->m_color = renderer::Settings.ambientColor;

		// Create button
		sublayout = dusk->Add<dusk::layouts::Horizontal>( dusk::LayoutCreationDescription{layout} );
		{
			ui_btn_apply_shit = dusk->Add<dusk::elements::Button>( dusk::ElementCreationDescription{sublayout, Rect(20,610,45,30)} );
			ui_btn_apply_shit->m_contents = "Apply";

			ui_btn_cancel_shit = dusk->Add<dusk::elements::Button>( dusk::ElementCreationDescription{sublayout, Rect(80,610,45,30)} );
			ui_btn_cancel_shit->m_contents = "Cancel";
		}
	}

	// Area panel
	{
		dusk::Element* button;
		dusk::elements::Label* label;
		dusk::Element* field;

		// Create the panel
		dusk::elements::DockablePanel* panel = dusk->Add<dusk::elements::DockablePanel>( dusk::ElementCreationDescription{NULL, Rect(0,40,200,650)} );
		panel->m_dockPosition = dusk::elements::DockPosition::kScreenLeft;
		panel->m_dockFlags = dusk::elements::DockFlags::kLocked;
		panel->m_dockOrder = 2;
		ui_panel_area = panel;

		// Create layout for the panel
		dusk::layouts::Vertical* layout = dusk->Add<dusk::layouts::Vertical>( dusk::LayoutCreationDescription{panel} );
		layout->m_margin = {10, 10};
		layout->m_padding = {0, 0};

		// Create labels
		label = dusk->Add<dusk::elements::Label>( dusk::ElementCreationDescription{layout, Rect(11,1,15,25)} );
		label->m_style = dusk::elements::kLabelStyle_Heading1;
		label->m_contents = "AREAS";
		label = dusk->Add<dusk::elements::Label>( dusk::ElementCreationDescription{layout, Rect(11,21,15,25)} );
		label->m_style = dusk::elements::kLabelStyle_Heading2;
		label->m_contents = "for triggers and effects";

		// Create help info
		label = dusk->Add<dusk::elements::Label>( dusk::ElementCreationDescription{layout, Rect(20,50,20,25)} );
		label->m_contents = "SHIFT+LMB to create area";
		label = dusk->Add<dusk::elements::Label>( dusk::ElementCreationDescription{layout, Rect(20,70,20,25)} );
		label->m_contents = "SHIFT+RMB to delete area";
		label = dusk->Add<dusk::elements::Label>( dusk::ElementCreationDescription{layout, Rect(20,90,20,25)} );
		label->m_contents = "ALT to snap to half-tile";

		// Create dropdown list type
		label = dusk->Add<dusk::elements::Label>( dusk::ElementCreationDescription{layout, Rect(20,125,20,25)} );
		label->m_contents = "Area Type";

		ui_fld_area_type = dusk->Add<dusk::elements::DropdownList<int>>( dusk::ElementCreationDescription{layout, Rect(20,150,160,30)} );
		if ( lcl_areatype_map.empty() ) {
			lcl_areatype_map["<no selection>"] = -1;
			lcl_areatype_map["Area2DBase"] = 0;
			lcl_areatype_map["AreaTeleport"] = 1;
			lcl_areatype_map["AreaTrigger"] = 2;
			lcl_areatype_map["AreaPlayerSpawn"] = 3;
		}
		for ( auto pair = lcl_areatype_map.begin(); pair != lcl_areatype_map.end(); ++pair )
			ui_fld_area_type->as<dusk::elements::DropdownList<int>>()->m_list.push_back({pair->first, pair->second});
		ui_fld_area_type = field;
	}

	// Object panel
	{
		dusk::Element* button;
		dusk::elements::Label* label;
		dusk::Element* field;

		// Create the panel
		dusk::elements::DockablePanel* panel = dusk->Add<dusk::elements::DockablePanel>( dusk::ElementCreationDescription{NULL, Rect(0,40,200,650)} );
		panel->m_dockPosition = dusk::elements::DockPosition::kScreenLeft;
		panel->m_dockFlags = dusk::elements::DockFlags::kLocked;
		panel->m_dockOrder = 2;
		ui_panel_object = panel;

		// Create layout for the panel
		dusk::layouts::Vertical* layout = dusk->Add<dusk::layouts::Vertical>( dusk::LayoutCreationDescription{panel} );
		layout->m_margin = {10, 10};
		layout->m_padding = {0, 0};

		// Create labels
		label = dusk->Add<dusk::elements::Label>( dusk::ElementCreationDescription{layout, Rect(11,1,15,25)} );
		label->m_style = dusk::elements::kLabelStyle_Heading1;
		label->m_contents = "OBJECTS";
		label = dusk->Add<dusk::elements::Label>( dusk::ElementCreationDescription{layout, Rect(11,21,15,25)} );
		label->m_style = dusk::elements::kLabelStyle_Heading2;
		label->m_contents = "entities and game objects";

		// Create help info
		label = dusk->Add<dusk::elements::Label>( dusk::ElementCreationDescription{layout, Rect(20,50,20,25)} );
		label->m_contents = "SHIFT+LMB to create object";
		label = dusk->Add<dusk::elements::Label>( dusk::ElementCreationDescription{layout, Rect(20,70,20,25)} );
		label->m_contents = "LMB to select object";
		label = dusk->Add<dusk::elements::Label>( dusk::ElementCreationDescription{layout, Rect(20,90,20,25)} );
		label->m_contents = "ALT to snap to half-tile";

		// Create dropdown list type
		label = dusk->Add<dusk::elements::Label>( dusk::ElementCreationDescription{layout, Rect(20,125,20,25)} );
		label->m_contents = "New Object";

		ui_fld_object_type = dusk->Add<dusk::elements::DropdownList<int>>( dusk::ElementCreationDescription{layout, Rect(20,150,160,30)} );
		{
			int i = 0;
			for ( auto entry = m_listing->List().begin(); entry != m_listing->List().end(); ++entry )
				ui_fld_area_type->as<dusk::elements::DropdownList<int>>()->m_list.push_back({entry->name.c_str(), ++i});
		}

		// Create labels
		ui_lbl_object_properties = dusk->Add<dusk::elements::Label>( dusk::ElementCreationDescription{layout, Rect(11,181,0,0)} );
		ui_lbl_object_properties->as<dusk::elements::Label>()->m_style = dusk::elements::kLabelStyle_Heading2;
		ui_lbl_object_properties->m_contents = "PROPERTIES";
	}


	// Preferences panel
	{
		dusk::Element* button;
		dusk::elements::Label* label;
		dusk::Element* field;

		// Create the panel
		dusk::elements::DockablePanel* panel = dusk->Add<dusk::elements::DockablePanel>( dusk::ElementCreationDescription{NULL, Rect(0,40,200,650)} );
		panel->m_dockPosition = dusk::elements::DockPosition::kScreenLeft;
		panel->m_dockFlags = dusk::elements::DockFlags::kLocked;
		panel->m_dockOrder = 2;
		ui_panel_preferences = panel;

		// Create layout for the panel
		dusk::layouts::Vertical* layout = dusk->Add<dusk::layouts::Vertical>( dusk::LayoutCreationDescription{panel} );
		layout->m_margin = {10, 10};
		layout->m_padding = {0, 0};

		// Create labels
		label = dusk->Add<dusk::elements::Label>( dusk::ElementCreationDescription{layout, Rect(11,1,15,25)} );
		label->m_style = dusk::elements::kLabelStyle_Heading1;
		label->m_contents = "PREFERENCES";
		label = dusk->Add<dusk::elements::Label>( dusk::ElementCreationDescription{layout, Rect(11,21,15,25)} );
		label->m_style = dusk::elements::kLabelStyle_Heading2;
		label->m_contents = "user options and such";

		// Mouse sensitivity field
		label = dusk->Add<dusk::elements::Label>( dusk::ElementCreationDescription{layout, Rect(20,50,20,25)} );
		label->m_contents = "Mouse Sensitivity";

		ui_fld_pref_mouse_sensitivity = dusk->Add<dusk::elements::Slider<float>>( dusk::ElementCreationDescription{layout, Rect(20,70,160,30)} );
		ui_fld_pref_mouse_sensitivity->as<dusk::elements::Slider<float>>()->m_range_min = 0.2F;
		ui_fld_pref_mouse_sensitivity->as<dusk::elements::Slider<float>>()->m_range_max = 5.0F;
		ui_fld_pref_mouse_sensitivity->as<dusk::elements::Slider<float>>()->m_snap = true;
		ui_fld_pref_mouse_sensitivity->as<dusk::elements::Slider<float>>()->m_snap_divisor = 0.1F;
		ui_fld_pref_mouse_sensitivity->as<dusk::elements::Slider<float>>()->m_value = 1.0F;

		label = dusk->Add<dusk::elements::Label>( dusk::ElementCreationDescription{layout, Rect(20,100,20,25)} );
		label->m_contents = "The engine ignores OS mouse";
		label = dusk->Add<dusk::elements::Label>( dusk::ElementCreationDescription{layout, Rect(20,120,20,25)} );
		label->m_contents = "settings.";
	}
}

//===============================================================================================//

//		uiStepKeyboardShortcuts () : do shortcut shit
// handles keyboard inputs (shortcuts) to do things fast
void MapEditor::uiStepKeyboardShortcuts ( void )
{
	if ( Input::Key( Keys.Control ) )
	{
		if ( Input::Keydown( 'S' ) )
		{
			if ( m_current_savetarget.empty() )
			{
				m_current_mode = Mode::None; // Close out everything to prevent UI from locking up

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
		if ( Input::Keydown( 'O' ) )
		{
			m_current_mode = Mode::None; // Close out everything to prevent UI from locking up

			System::sFileDialogueEntry filetypes [1];
			strcpy( filetypes[0].extension, "m04" );
			strcpy( filetypes[0].filetype, "OneEngine M04 Map" );
			ui_dg_load = dusk->DialogueOpenFilename(filetypes,1,"./.res-0/");
		}

		if ( Input::Keydown( '1' ) )
		{
			m_current_mode = Mode::Properties;
		}
		if ( Input::Keydown( '2' ) )
		{
			m_current_mode = Mode::TileEdit;
		}
		if ( Input::Keydown( '3' ) )
		{
			m_current_mode = Mode::AreaEdit;
		}
		if ( Input::Keydown( '4' ) )
		{
			m_current_mode = Mode::ObjectEdit;
		}
		if ( Input::Keydown( '5' ) )
		{
			m_current_mode = Mode::ScriptEdit;
		}
		if ( Input::Keydown( '6' ) )
		{
			m_current_mode = Mode::UtilityEdit;
		}
		if ( Input::Keydown( '7' ) )
		{
			m_current_mode = Mode::Toolbox;
		}
		if ( Input::Keydown( '8' ) )
		{
			m_current_mode = Mode::Preferences;
		}
	}
}

//		uiStepTopEdge () : top edge update
// handle inputs to the buttons on the top edge
void MapEditor::uiStepTopEdge ( void )
{
	if ( ui_file_new->m_isActivated )
	{
		doNewMap();

		// Clear save target (so not overwrite on accident)
		m_current_savetarget = "";
		// Reset camera
		m_target_camera_position.x = 0;
		m_target_camera_position.y = 0;

		// Set default tileset TODO: Add a UI for this.
		//m_tilemap->SetTilesetFile( "tileset/ld40.txt" );

		try {
			m_tilemap->Rebuild();
		}
		catch ( core::InvalidCallException& ) {
			printf( "No tiles on new map. This is normal.\n" );
		}
	}
	if ( ui_file_save->m_isActivated )
	{
		m_current_mode = Mode::None; // Close out everything to prevent UI from locking up

		System::sFileDialogueEntry filetypes [1];
		strcpy( filetypes[0].extension, "m04" );
		strcpy( filetypes[0].filetype, "OneEngine M04 Map" );
		ui_dg_save = dusk->DialogueSaveFilename(filetypes,1,"./.res-0/");
	}
	if ( ui_file_load->m_isActivated )
	{
		m_current_mode = Mode::None; // Close out everything to prevent UI from locking up

		System::sFileDialogueEntry filetypes [1];
		strcpy( filetypes[0].extension, "m04" );
		strcpy( filetypes[0].filetype, "OneEngine M04 Map" );
		ui_dg_load = dusk->DialogueOpenFilename(filetypes,1,"./.res-0/");
	}

	if ( ui_mode_shit->m_isActivated )
	{
		m_current_mode = Mode::Properties;
	}
	if ( ui_mode_map->m_isActivated )
	{
		m_current_mode = Mode::TileEdit;
	}
	if ( ui_mode_area->m_isActivated )
	{
		m_current_mode = Mode::AreaEdit;
	}
	if ( ui_mode_object->m_isActivated )
	{
		m_current_mode = Mode::ObjectEdit;
	}
	if ( ui_mode_script->m_isActivated )
	{
		m_current_mode = Mode::ScriptEdit;
	}
	if ( ui_mode_utils->m_isActivated )
	{
		m_current_mode = Mode::UtilityEdit;
	}

	if ( ui_toolbox_cutscene->m_isActivated )
	{
		m_current_mode = Mode::Toolbox;
	}
	if ( ui_toolbox_global->m_isActivated )
	{
		m_current_mode = Mode::Toolbox;
	}
	if ( ui_toolbox_playtest->m_isActivated )
	{
		std::string filename = m_current_savetarget;
		std::transform(filename.begin(), filename.end(), filename.begin(), [](unsigned char c){ return std::tolower(c); });
		auto res_pos = filename.find_first_of(".res-");
		if (res_pos != string::npos)
		{
			filename = filename.substr(res_pos + 9);
		}
		M04::m04NextLevelToLoad = filename.c_str();
		engine::Console->RunCommand( "scene game_luvppl" );
	}

	if ( ui_mode_preferences->m_isActivated )
	{
		m_current_mode = Mode::Preferences;
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
		// Perform loading
		doNewMap();
		doIOLoading();
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
			ui_lbl_map_area->m_visible = true;
		else
			ui_lbl_map_area->m_visible = false;
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
	ui_lbl_map_size->m_contents = "in pixels: " + std::to_string(sizex*m_tilemap->m_tileset->tilesize_x) + " x " + std::to_string(sizey*m_tilemap->m_tileset->tilesize_y);

	// Apply new map properties
	if ( ui_btn_apply_shit->m_isActivated )
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
	if ( ui_btn_cancel_shit->m_isActivated )
	{	// Just reload all fields
		uiDoShitRefresh();
	}

	// Do realtime changes:
	{
		dusk->GetColorPicker( ui_fld_map_ambient_color, renderer::Settings.ambientColor );
		m_mapinfo->env_ambientcolor = renderer::Settings.ambientColor.GetCode();
	}
}
//		uiDoShitRefresh () : S.H.I.T. panel reinit
// resets all input in the shit panel with the current values
void MapEditor::uiDoShitRefresh ( void )
{
	ui_fld_map_name->m_contents = string( m_mapinfo->map_name );
	ui_fld_map_area->m_contents = string( m_mapinfo->area_name );
	ui_fld_map_size_x->m_contents = std::to_string( m_mapinfo->tilesize_x );
	ui_fld_map_size_y->m_contents = std::to_string( m_mapinfo->tilesize_y );
}

//		uiStepTilePanel () : tile editor panel update
// handles inputs and updates to the tile panel
void MapEditor::uiStepTilePanel ( void )
{
	// Ensure correct tile mode is up
	switch (m_current_submode)
	{
	case SubMode::TilesVisual:
	case SubMode::TilesCollision:
	case SubMode::TilesHeight:
		// nothing;
		break;
	default:
		m_current_submode = SubMode::TilesVisual;
	}

	// Do button stuffs
	if ( ui_btn_inc_layer->m_isActivated )
	{
		if ( Input::Key(Keys.Control) )
			m_tile_layer_current += 16;
		else
			m_tile_layer_current += 1;
		ui_fld_current_layer->m_contents = std::to_string(m_tile_layer_current);
	}
	if ( ui_btn_dec_layer->m_isActivated )
	{
		if ( Input::Key(Keys.Control) )
			m_tile_layer_current -= 16;
		else
			m_tile_layer_current -= 1;
		ui_fld_current_layer->m_contents = std::to_string(m_tile_layer_current);
	}
	
	// Change mode with buttons
	if ( ui_btn_tile_mode_visual->m_isActivated )
	{
		m_current_submode = SubMode::TilesVisual;
		// Draw only wireframe
		m_tile_collision_renderer->m_drawSolids = false;
		m_tile_collision_renderer->m_drawWireframe = true;
	}
	if ( ui_btn_tile_mode_collision->m_isActivated )
	{
		m_current_submode = SubMode::TilesCollision;
		// Draw solids and wireframe
		m_tile_collision_renderer->m_drawSolids = true;
		m_tile_collision_renderer->m_drawWireframe = true;
	}
	if ( ui_btn_tile_mode_height->m_isActivated )
	{
		m_current_submode = SubMode::TilesHeight;
	}

	// Change tile brightness to help w/ UI editing
	if ( m_current_submode == SubMode::TilesVisual )
	{
		// In visual mode, change brightness of tiles

		// Change brightness of tiles being edited
		for ( renderer::TileMapLayer* layer : m_tilemap->m_render_layers )
		{
			if ( layer == NULL ) continue;

			if ( layer->source_layer_id == m_tile_layer_current )
			{
				layer->PassGetSurface(0).diffuseColor = Color(1.25F, 1.25F, 1.25F, 1.0F);
			}
			else
			{
				layer->PassGetSurface(0).diffuseColor = Color(0.25F, 0.25F, 0.25F, 1.0F);
			}
		}
		// Anything else?
	}
	// Other modes, use normal layer brightness
	else
	{
		// Change brightness of tiles being edited
		for ( renderer::TileMapLayer* layer : m_tilemap->m_render_layers )
		{
			if ( layer == NULL ) continue;

			layer->PassGetSurface(0).diffuseColor = Color(1.0F, 1.0F, 1.0F, 1.0F); // TODO: the GetSurface marks the surface as edited. Don't need to edit if already white.
		}
	}

}
// Called when tile-editing mode ends
void MapEditor::_uiStepTilePanel_End ( void )
{
	// Reset tile brightneess
	for ( renderer::TileMapLayer* layer : m_tilemap->m_render_layers )
	{
		if ( layer == NULL ) continue;

		layer->PassGetSurface(0).diffuseColor = Color(1.0F, 1.0F, 1.0F, 1.0F);
	}
}

//		uiStepAreaPanel () : area panel update
// handles input and updates to the area panel
void MapEditor::uiStepAreaPanel ( void )
{
	if ( dusk->GetOpenDialogue() == ui_fld_area_type )
	{
		// Mark as possible change made
		m_current_submode = SubMode::Dropdown;
	}
	else
	{
		// If was in the dropdown
		if ( m_current_submode == SubMode::Dropdown && m_area_target != NULL )
		{
			// Check for any changes
			string areaStringId = m_area_target->GetTypeName();
			int areaIntID = lcl_areatype_map[areaStringId];
			// Grab target
			int targetIntID = ui_fld_area_type->as<dusk::elements::DropdownList<int>>()->Selection().second;
			string targetStringId = "Area2DBase";
			for ( auto pair = lcl_areatype_map.begin(); pair != lcl_areatype_map.end(); ++pair )
			{
				if ( targetIntID == pair->second )
				{
					targetStringId = pair->first;
					break;
				}
			}
			if ( areaIntID != targetIntID )
			{
				// Create new area if needed
				Engine2D::Area2DBase* area;
				if ( targetStringId == "Area2DBase" )
					area = new Engine2D::Area2DBase;
				else if ( targetStringId == "AreaTeleport" )
					area = new Engine2D::AreaTeleport;
				else if ( targetStringId == "AreaTrigger" )
					area = new Engine2D::AreaTrigger;
				else if ( targetStringId == "AreaPlayerSpawn" )
					area = new Engine2D::AreaPlayerSpawn;
				else 
					area = new Engine2D::Area2DBase;
				area->RemoveReference();
				// Have it grab the rect of the old area
				area->m_rect = m_area_target->m_rect;
				// Delete old area
				delete m_area_target;
				// Set new area as the target
				m_area_target = area;
				// Set tips for the area renderer
				m_area_renderer->m_target_selection = m_area_target;
			}
			// Go back to normal mode
			m_current_submode = SubMode::None;
		}
		// If no selection, empty out the target type
		if ( m_area_target == NULL )
		{
			ui_fld_area_type->as<dusk::elements::DropdownList<int>>()->SetSelection(-1);
		}
		else
		{
			ui_fld_area_type->as<dusk::elements::DropdownList<int>>()->SetSelection(lcl_areatype_map[m_area_target->GetTypeName()]);
		}
	}
}

//		uiStepObjectPanel () : object panel update
// handles input and updates to the object panel
void MapEditor::uiStepObjectPanel ( void )
{
	if ( dusk->CurrentDialogue() == ui_fld_object_type )
	{
		// We stop doing shit
	}
	else
	{
		if ( m_object_target == NULL )
		{
			ui_lbl_object_properties->m_visible = false;
			_uiStepObjectPanelSub_ClearProperties();
		}
		else
		{
			ui_lbl_object_properties->m_visible = true;

			// Get the object's metadata
			const engine::MetadataTable* metadata = m_object_target->m_object->GetMetadata();
			// Generate the UI for the object's editable properties
			if ( ui_lbl_object_keys.empty() )
			{
				// Force update on its metadata
				m_object_target->WorldToMetadata();

				dusk::Element* panel = ui_panel_object;
				dusk::Element* label;
				dusk::Element* field;

				for ( uint i = 0; i < metadata->data.size(); ++i )
				{
					char* target_data = m_object_target->m_data_storage_buffer + metadata->data[i].first;

					// Create label for the data
					label = dusk->CreateText( panel, metadata->data_name[i].second.c_str() );
					label.SetRect(Rect(20.0F, 200.0F + i * 50.0F,0,0));
					// Create field for the data
					switch (metadata->data_type[i].second)
					{
					case VALUE_INT32:
						field = dusk->CreateTextfield( panel, std::to_string( *((uint32_t*)target_data) ) );
					case VALUE_FLOAT:
						field = dusk->CreateTextfield( panel, std::to_string( *((float*)target_data) ) );
						break;
					case VALUE_FLOAT4:
						if (metadata->data_field[i].second == FIELD_COLOR)
							field = dusk->CreateColorPicker( panel, *((Color*)target_data) );
						break;
					default:
						field = NULL;
						break;
					}
					if (field != NULL)
					{
						field.SetRect(Rect(20.0F,225.0F + i * 50.0F,160,30));
					}
					// Pull current state for the data

					ui_lbl_object_keys.push_back(label);
					ui_lbl_object_values.push_back(field);
				}
			}

			if ( !dusk->IsMouseInside() && !dusk->HasOpenDialogue() )
			{
				// Force update on its metadata
				m_object_target->WorldToMetadata();

				for ( uint i = 0; i < ui_lbl_object_values.size(); ++i )
				{
					if ( ui_lbl_object_values[i] == NULL ) continue;

					char* target_data = m_object_target->m_data_storage_buffer + metadata->data[i].first;

					// Update field data from world
					switch (metadata->data_type[i].second)
					{
					case VALUE_INT32:
						ui_lbl_object_values[i].SetText( std::to_string( *((uint32_t*)target_data) ) );
					case VALUE_FLOAT:
						ui_lbl_object_values[i].SetText( std::to_string( *((float*)target_data) ) );
						break;
					case VALUE_FLOAT4:
						if (metadata->data_field[i].second == FIELD_COLOR)
							dusk->SetColorPicker( ui_lbl_object_values[i], *((Color*)target_data) );
						break;
					}
				}
			}
			else
			{
				// UI Is generated, has valid data. We pull information from the UI and place them into the metadata
				for ( uint i = 0; i < ui_lbl_object_values.size(); ++i )
				{
					if ( ui_lbl_object_values[i] == NULL ) continue;

					char* target_data = m_object_target->m_data_storage_buffer + metadata->data[i].first;

					string s_temp;
					Color c_temp;
					uint32_t i_temp;
					float f_temp;

					// Update field based on data
					switch (metadata->data_type[i].second)
					{
					case VALUE_INT32:
						dusk->GetTextfieldData( ui_lbl_object_values[i], s_temp );
						i_temp = std::atoi( s_temp.c_str() );
						memcpy( target_data, &i_temp, sizeof(uint32_t) );
						break;
					case VALUE_FLOAT:
						dusk->GetTextfieldData( ui_lbl_object_values[i], s_temp );
						f_temp = (float)std::atof( s_temp.c_str() );
						memcpy( target_data, &f_temp, sizeof(float) );
						break;
					case VALUE_FLOAT4:
						if (metadata->data_field[i].second == FIELD_COLOR)
						{
							dusk->GetColorPicker( ui_lbl_object_values[i], c_temp );
							memcpy( target_data, &c_temp, sizeof(Color) );
						}
						break;
					}
				}
				// Force update from its metadata
				m_object_target->MetadataToWorld();
				// Update gizmos
				_doObjectEditingSub_GizmoEnable();
			}
		}
	}
}
void MapEditor::_uiStepObjectPanelSub_ClearProperties ( void )
{
	// Delete all objects created for the UI
	if ( !ui_lbl_object_keys.empty() )
	{
		for ( dusk::Element* &handle : ui_lbl_object_keys )
		{
			if ( handle != NULL )
				handle->m_destructionRequested = true;
			handle = NULL;
		}
		ui_lbl_object_keys.clear();
	}
	if ( !ui_lbl_object_values.empty() )
	{
		for ( dusk::Element* &handle : ui_lbl_object_values )
		{
			if ( handle != NULL )
				handle->m_destructionRequested = true;
			handle = NULL;
		}
		ui_lbl_object_values.clear();
	}
}


//		uiStepObjectPanel () : preferencess panel update
// handles input and updates to the preferencess panel
void MapEditor::uiStepPreferencesPanel ( void )
{
	static Real sensitivity = 1.0F;
	dusk->UpdateSlider( ui_fld_pref_mouse_sensitivity, sensitivity );
	CInput::SetMouseSensitivity( sensitivity );
}

//		uiStepBottomEdge () : status panel update
// updates display of the current editor state
void MapEditor::uiStepBottomEdge ( void )
{
	// Update the status on the bottom
	if ( m_navigation_busy )
		ui_lbl_mode->m_contents = "Moving view";
	else switch ( m_current_mode )
	{
	case Mode::Properties:	ui_lbl_mode->m_contents = "S.H.I.T.";
		break;
	case Mode::TileEdit:	ui_lbl_mode->m_contents = "Map Editing";
		break;
	case Mode::AreaEdit:	ui_lbl_mode->m_contents = "Area Editing";
		break;
	case Mode::ObjectEdit:	ui_lbl_mode->m_contents = "Object Editing";
		break;
	case Mode::ScriptEdit:	ui_lbl_mode->m_contents = "Script Editing";
		break;
	case Mode::Toolbox:		ui_lbl_mode->m_contents = "In Toolbox";
		break;
	case Mode::None:
	default:				ui_lbl_mode->m_contents = "Ready.";
		break;
	}

	ui_lbl_mode->m_contents = m_current_savetarget;
	
	if ( !dusk->IsMouseInside() )
	{
		// Update mouse position on the GUI
		Vector3f worldpos = m_target_camera->ScreenToWorldPos( Vector2f( Input::MouseX()/(Real)Screen::Info.width, Input::MouseY()/(Real)Screen::Info.height ) );

		// Round the position to half-tile if ALT is held down
		if ( !Input::Key( Keys.Alt ) )
		{
			ui_lbl_mousex->m_contents = "X: " + std::to_string(math::round(worldpos.x));
			ui_lbl_mousey->m_contents = "Y: " + std::to_string(math::round(worldpos.y));
		}
		else
		{
			ui_lbl_mousex->m_contents = "X: " + std::to_string((int)(math::round(worldpos.x*2.0F/m_tilemap->m_tileset->tilesize_x) * m_tilemap->m_tileset->tilesize_x * 0.5F));
			ui_lbl_mousey->m_contents = "Y: " + std::to_string((int)(math::round(worldpos.y*2.0F/m_tilemap->m_tileset->tilesize_y) * m_tilemap->m_tileset->tilesize_y * 0.5F));
		}
	}
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