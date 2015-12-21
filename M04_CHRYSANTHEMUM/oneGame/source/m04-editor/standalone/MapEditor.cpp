
#include "MapEditor.h"

#include "core/input/CInput.h"

#include "engine-common/dusk/CDuskGUI.h"

#include "renderer/texture/CBitmapFont.h"
#include "renderer/camera/CCamera.h"
#include "render2d/camera/COrthoCamera.h"

#include "./mapeditor/TileSelector.h"

using namespace M04;

MapEditor::MapEditor ( void )
	: CGameBehavior(), m_current_mode(Mode::None)
{
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
	// Build Dusk Gui
	{
		dusk = new CDuskGUI();
		dusk->SetDefaultFont( new CBitmapFont( "YanoneKaffeesatz-R.otf", 13 ) );
	}
	// Build other UI elements
	{
		m_tile_selector = new M04::TileSelector();
	}
}

MapEditor::~MapEditor ( void )
{
	delete_safe(m_target_camera);
	delete_safe_decrement(dusk);
	delete_safe(m_tile_selector);
}

void MapEditor::Update ( void )
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
			m_tile_selector->Update();
			break;
		}
	}
}

// move the map around when middle button pressed
void MapEditor::doViewNavigationDrag ( void )
{
	if ( Input::Mouse( Input::MBMiddle ) )
	{
		m_target_camera->transform.position -= Vector2d(
			Input::DeltaMouseX(), Input::DeltaMouseY()
			);
	}
	if ( Input::DeltaMouseW() != 0 )
	{
		m_target_camera->transform.position.y += Input::DeltaMouseW();
	}
}