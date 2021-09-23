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
	// Build Dusk Gui
	{
		dusk = new dusk::UserInterface( RrWindow::List()[0] );

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
		ui_paletted_3d_test = dusk->Add<dusk::elements::Button>( dusk::ElementCreationDescription{layout, Rect( 0, 0, 150, 25 )} );
		ui_paletted_3d_test->m_contents = "Paletted 3D Test";
		
		ui_seq_editor = dusk->Add<dusk::elements::Button>( dusk::ElementCreationDescription{layout, Rect( 0, 0, 150, 25 )} );
		ui_seq_editor->m_contents = "Sequence Editor";

		ui_noise_editor = dusk->Add<dusk::elements::Button>( dusk::ElementCreationDescription{layout, Rect( 0, 0, 150, 25 )} );
		ui_noise_editor->m_contents = "Noise Editor";

		ui_material_editor = dusk->Add<dusk::elements::Button>( dusk::ElementCreationDescription{layout, Rect( 0, 0, 150, 25 )} );
		ui_material_editor->m_contents = "Material Editor";
		ui_material_editor->m_onActivation = []() { engine::Console->RunCommand( "scene MaterialEditor" ); };

		ui_material_browser = dusk->Add<dusk::elements::Button>( dusk::ElementCreationDescription{layout, Rect( 0, 0, 150, 25 )} );
		ui_material_browser->m_contents = "Material Browser";
		ui_material_browser->m_onActivation = []() { engine::Console->RunCommand( "scene MaterialBrowser" ); };

		ui_model_editor = dusk->Add<dusk::elements::Button>( dusk::ElementCreationDescription{layout, Rect( 0, 0, 150, 25 )} );
		ui_model_editor->m_contents = "Model Editor";
		ui_model_editor->m_onActivation = []() { engine::Console->RunCommand( "scene ModelEditor" ); };

		ui_level_builder = dusk->Add<dusk::elements::Button>( dusk::ElementCreationDescription{layout, Rect( 0, 0, 150, 25 )} );
		ui_level_builder->m_contents = "Level Builder";
		ui_level_builder->m_onActivation = []() { engine::Console->RunCommand( "scene LevelBuilder" ); };

		auto ui_bottler = dusk->Add<dusk::elements::Button>( dusk::ElementCreationDescription{layout, Rect( 0, 0, 150, 25 )} );
		ui_bottler->m_contents = "Bottler";
		ui_bottler->m_onActivation = []() { engine::Console->RunCommand( "scene BottlerUI" ); };
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
	}
	else if (ui_paletted_3d_test->as<dusk::elements::Button>()->Pressed())
	{
		engine::Console->RunCommand( "scene Palette3DTest0" );
		DeleteObject(this);
	}
	else if (ui_noise_editor->as<dusk::elements::Button>()->Pressed())
	{
		engine::Console->RunCommand( "scene NoiseEditor" );
	}
	//else if ( ui_main_game->as<dusk::elements::Button>()->Pressed() )
	//{
	//	while ( true )
	//		debug::Console->PrintError("HA ");
	//}
}