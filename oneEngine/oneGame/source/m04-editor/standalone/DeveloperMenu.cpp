#include "DeveloperMenu.h"

#include "core/input/CInput.h"
#include "core/math/Math.h"
#include "core/debug/CDebugConsole.h"

#include "engine/utils/CDeveloperConsole.h"

#include "engine-common/dusk/UI.h"
#include "engine-common/dusk/controls/Button.h"
#include "engine-common/dusk/controls/Panel.h"
#include "engine-common/dusk/controls/Label.h"
#include "engine-common/entities/CRendererHolder.h"

#include "renderer/texture/RrFontTexture.h"
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
		dusk = new dusk::UserInterface;
		//dusk->SetPixelMode(true);

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
		dusk::Element* panel;
		dusk::Element* button;

		// Make base panel
		panel = dusk->Add<dusk::elements::Panel>( dusk::ElementCreationDescription{NULL, Rect( 100,100,200,400 )} );

		// Create buttons
		button = dusk->Add<dusk::elements::Button>( dusk::ElementCreationDescription{panel, Rect( 20,50,150,20 )} );
		button->m_contents = "Editor (Test 1)";
		ui_main_editor = button;

		button = dusk->Add<dusk::elements::Button>( dusk::ElementCreationDescription{panel, Rect( 20,100,150,20 )} );
		button->m_contents = "Actually game";
		ui_main_testg = button;

		button = dusk->Add<dusk::elements::Button>( dusk::ElementCreationDescription{panel, Rect( 20,150,150,20 )} );
		button->m_contents = "Test 0";
		ui_main_test0 = button;

		button = dusk->Add<dusk::elements::Button>( dusk::ElementCreationDescription{panel, Rect( 20,200,150,20 )} );
		button->m_contents = "Game (don't click)";
		ui_main_game = button;
	}
}

//===============================================================================================//

//		uiStepMainPanel () : main panel update
// handle inputs to the buttons on main panel
void DeveloperMenu::uiStepMainPanel ( void )
{
	if ( ui_main_editor->as<dusk::elements::Button>()->Pressed() )
	{
		engine::Console->RunCommand( "scene editorm04" );
	}
	else if ( ui_main_testg->as<dusk::elements::Button>()->Pressed() )
	{
		engine::Console->RunCommand( "scene game_luvppl" );
	}
	else if ( ui_main_test0->as<dusk::elements::Button>()->Pressed() )
	{
		engine::Console->RunCommand( "scene test0" );
	}
	else if ( ui_main_game->as<dusk::elements::Button>()->Pressed() )
	{
		while ( true )
			debug::Console->PrintError("HA ");
	}
}