#include "DeveloperMenu.h"

#include "core/input/CInput.h"
#include "core/math/Math.h"
#include "core/debug/Console.h"

#include "engine/utils/CDeveloperConsole.h"

#include "engine-common/dusk/UI.h"
#include "engine-common/dusk/controls/Button.h"
#include "engine-common/dusk/controls/Panel.h"
#include "engine-common/dusk/controls/Label.h"
#include "engine-common/dusk/layouts/Vertical.h"
#include "engine-common/entities/CRendererHolder.h"

#include "renderer/texture/RrFontTexture.h"
#include "renderer/windowing/RrWindow.h"
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
		dusk = new dusk::UserInterface( RrWindow::List()[0] );
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

		// Make base panel
		panel = dusk->Add<dusk::elements::Panel>( dusk::ElementCreationDescription{NULL, Rect( 100,100,200,400 )} );

		dusk::layouts::Vertical* layout = dusk->Add<dusk::layouts::Vertical>( dusk::LayoutCreationDescription{panel} );
		layout->m_margin = Vector2f(10.F, 20.F);

		// Create label
		dusk->Add<dusk::elements::Label>( dusk::ElementCreationDescription{layout, Rect( 0, 0, 150, 25 )} )
			->m_contents = "Developer Menu";

		// Create actual buttons
		ui_seq_editor = dusk->Add<dusk::elements::Button>( dusk::ElementCreationDescription{layout, Rect( 0, 0, 150, 25 )} );
		ui_seq_editor->m_contents = "Sequence Editor";

		ui_cts_editor = dusk->Add<dusk::elements::Button>( dusk::ElementCreationDescription{layout, Rect( 0, 0, 150, 25 )} );
		ui_cts_editor->m_contents = "OLD: Cutscene Editor";

		// Create buttons
		ui_main_editor = dusk->Add<dusk::elements::Button>( dusk::ElementCreationDescription{layout, Rect( 0, 0, 150, 25 )} );
		ui_main_editor->m_contents = "OLD: Editor (Test 1)";

		ui_main_testg = dusk->Add<dusk::elements::Button>( dusk::ElementCreationDescription{layout, Rect( 0, 0, 150, 25 )} );
		ui_main_testg->m_contents = "OLD: Actually game";

		ui_main_test0 = dusk->Add<dusk::elements::Button>( dusk::ElementCreationDescription{layout, Rect( 0, 0, 150, 25 )} );
		ui_main_test0->m_contents = "OLD: Test 0";

		ui_main_game = dusk->Add<dusk::elements::Button>( dusk::ElementCreationDescription{layout, Rect( 0, 0, 150, 25 )} );
		ui_main_game->m_contents = "OLD: Game (don't click)";
	}
}

//===============================================================================================//

//		uiStepMainPanel () : main panel update
// handle inputs to the buttons on main panel
void DeveloperMenu::uiStepMainPanel ( void )
{
	if (ui_seq_editor->as<dusk::elements::Button>()->Pressed())
	{
		engine::Console->RunCommand( "scene SequenceEditor" );
		DeleteObject(this);
	}
	else if (ui_cts_editor->as<dusk::elements::Button>()->Pressed())
	{
		engine::Console->RunCommand( "scene editorcts" );
		DeleteObject(this);
	}
	else if ( ui_main_editor->as<dusk::elements::Button>()->Pressed() )
	{
		engine::Console->RunCommand( "scene editorm04" );
		DeleteObject(this);
	}
	else if ( ui_main_testg->as<dusk::elements::Button>()->Pressed() )
	{
		engine::Console->RunCommand( "scene game_luvppl" );
		DeleteObject(this);
	}
	else if ( ui_main_test0->as<dusk::elements::Button>()->Pressed() )
	{
		engine::Console->RunCommand( "scene test0" );
		DeleteObject(this);
	}
	else if ( ui_main_game->as<dusk::elements::Button>()->Pressed() )
	{
		while ( true )
			debug::Console->PrintError("HA ");
	}
}