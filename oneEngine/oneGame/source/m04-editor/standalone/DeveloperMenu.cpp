
#include "DeveloperMenu.h"

#include "core/input/CInput.h"
#include "core/math/Math.h"
#include "core/debug/CDebugConsole.h"

#include "engine/utils/CDeveloperConsole.h"

#include "engine-common/dusk/CDuskGUI.h"
#include "engine-common/entities/CRendererHolder.h"

#include "renderer/texture/CBitmapFont.h"
#include "render2d/object/Background2D.h"

using namespace M04;

DeveloperMenu::DeveloperMenu ( void )
	: CGameBehavior()
{
	// Create background
	{
		CRenderable2D* bg = new renderer::Background2D();
		(new CRendererHolder (bg))->RemoveReference();
	}
	// Build Dusk Gui
	{
		dusk = new CDuskGUI();
		dusk->SetDefaultFont( new CBitmapFont( "YanoneKaffeesatz-R.otf", 16 ) );
		dusk->SetPixelMode(true);

		uiCreate();
	}
}

DeveloperMenu::~DeveloperMenu ( void )
{
	delete_safe_decrement(dusk);
}

//===============================================================================================//
// Update
//===============================================================================================//

void DeveloperMenu::Update ( void )
{
	// Update the main panel
	uiStepMainPanel();
}

//===============================================================================================//
// UI Specific
//===============================================================================================//

//		uiCreate () : create the dusk UI
// create entirety of the dusk gui shit
void DeveloperMenu::uiCreate ( void )
{
	{
		Dusk::Handle panel;
		Dusk::Handle button, label;

		// Make base panel
		panel = dusk->CreatePanel();
		panel.SetRect( Rect( 100,100,200,400 ) );

		// Create buttons
		button = dusk->CreateButton( panel );
		button.SetRect( Rect( 20,50,150,20 ) );
		button.SetText( "Editor (Test 1)" );
		ui_main_editor = button;

		button = dusk->CreateButton( panel );
		button.SetRect( Rect( 20,100,150,20 ) );
		button.SetText( "Actually game" );
		ui_main_testg = button;

		button = dusk->CreateButton( panel );
		button.SetRect( Rect( 20,150,150,20 ) );
		button.SetText( "Test 0" );
		ui_main_test0 = button;

		button = dusk->CreateButton( panel );
		button.SetRect( Rect( 20,200,150,20 ) );
		button.SetText( "Game (don't click)" );
		ui_main_game = button;
	}
}

//===============================================================================================//

//		uiStepMainPanel () : main panel update
// handle inputs to the buttons on main panel
void DeveloperMenu::uiStepMainPanel ( void )
{
	if ( ui_main_editor.GetButtonClicked() )
	{
		Engine::Console->RunCommand( "scene test1" );
	}
	else if ( ui_main_testg.GetButtonClicked() )
	{
		Engine::Console->RunCommand( "scene game_luvppl" );
	}
	else if ( ui_main_test0.GetButtonClicked() )
	{
		Engine::Console->RunCommand( "scene test0" );
	}
	else if ( ui_main_game.GetButtonClicked() )
	{
		while ( true )
			debug::Console->PrintError("HA ");
	}
}