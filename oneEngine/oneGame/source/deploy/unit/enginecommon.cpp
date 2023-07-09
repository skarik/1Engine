#include "deploy/unit/unit.h"

// System Includes
#include "core/math/random/Random.h"
#include "core/time.h"
#include "core/settings/CGameSettings.h"
#include "core/input/CInput.h"
#include "core/debug/console.h"
#include "core/types/arBaseObject.h"

#include "core-ext/core-ext.h"
#include "core-ext/profiler/CTimeProfiler.h"
#include "core-ext/threads/Jobs.h"
#include "core-ext/resources/ResourceManager.h"
#include "core-ext/system/shell/Status.h"
#include "core-ext/system/shell/Message.h"

// Include audio
#include "audio/Manager.h"

// Include physics
#include "physical/module_physical.h"
#include "physical/physics/PrPhysics.h"

// Include gamestate
#include "engine/module_engine.h"
#include "engine/state/CGameState.h"
#include "engine/utils/CDeveloperConsole.h"

// Include renderer
#include "renderer/camera/RrCamera.h"
#include "renderer/module_renderer.h"
#include "renderer/windowing/RrWindow.h"
#include "renderer/state/RrRenderer.h"
#include "renderer/utils/RrScreenshot.h"
#include "renderer/debug/RrDebugDrawer.h"

// Include engine-common
#include "engine-common/engine-common.h"
//#include "engine-common/lua/CLuaController.h"
#include "engine-common/scenes/gmsceneSystemLoader.h"

// Steam Include
#include "steam/steam_api.h"

// Include Dusk UI
#include "engine-common/dusk/UI.h"
#include "engine-common/dusk/controls/Panel.h"
#include "engine-common/dusk/controls/Button.h"
#include "engine-common/dusk/controls/Label.h"
#include "engine-common/dusk/controls/Slider.h"
#include "engine-common/dusk/layouts/Vertical.h"

int ARUNIT_CALL Unit::Test_EngineCommon ( ARUNIT_ARGS )
{	ARUNIT_BUILD_CMDLINE

	// Load window settings
	CGameSettings gameSettings ( (string)lpCmdLine, false );

	if ( CGameSettings::Active()->b_ro_Enable30Steroscopic )
	{
		core::shell::ShowErrorMessage( "Stereoscopic 3D mode either currently cascades into memory hell or isn't implemented." );
		abort();
	}

	// Create jobs system
	core::jobs::System jobSystem (4);

	// Create resource system
	core::ArResourceManager::Active()->Initialize();

	// Initialize input
	core::Input::Initialize();

	// Create renderer
	RrRenderer* aRenderer = new RrRenderer();

	// Create Window
	RrWindow aWindow( aRenderer, hInstance );
	if (!aWindow.Show())
	{
		core::shell::ShowErrorMessage( "Could not show windowing system.\n" );
		abort();
	}
	debug::Console->PrintMessage( "Windowing system initialized.\n" );
	std::cout << __OS_STRING_NAME__ " Build (" __DATE__ ") Indev" << std::endl;

	// Set shell status (loading engine)
	core::shell::SetTaskbarProgressHandle(aWindow.OsShellHandle());
	core::shell::SetTaskbarProgressValue(NIL, 100, 100);
	core::shell::SetTaskbarProgressState(NIL, core::shell::kTaskbarStateIndeterminate);

	// Set the window title
	aWindow.SetTitle("1Engine Test: Game Common Modules");

	// Init Physics
	PrPhysics::Active()->Initialize();
	// Create Gamestate
	CGameState aGameState;

	// Create Audio
	audio::Manager aManager;
	debug::Console->PrintMessage( "Audio manager created.\n" );

	// Initialize Steam
#if 0
	bool bSteamy = SteamAPI_Init();
	debug::Console->PrintMessage( "holy shit it's STEAMy!\n" );
#else
	bool bSteamy = false;
#endif

	// Create the engine systems
	//Lua::CLuaController* luaController = new Lua::CLuaController();
	engine::CDeveloperConsole* engConsole = new engine::CDeveloperConsole();

	// Set up engine component
	EngineCommonInitialize();

	// Create the game scene
	CGameScene* pNewScene = CGameScene::NewScene<gmsceneSystemLoader> ();
	CGameScene::SceneGoto( pNewScene );

	// Create debug camera to show stuff (overriding the existing camera)
	RrCamera* l_cam = new RrCamera(false);

	// Set up rendeer output
	uint worldIndex = aRenderer->AddWorldDefault();

	RrOutputInfo output(aRenderer->GetWorld(worldIndex), &aWindow);
	output.camera = l_cam;
	aRenderer->AddOutput(output);

	// Create a debugging Dusk menu
	{
		dusk::Element* panel;
		dusk::Element* button;

		dusk::UserInterface* dusk = new dusk::UserInterface(&aWindow);

		dusk::LayoutElement* layout;

		// Make base panel
		panel = dusk->Add<dusk::elements::Panel>( dusk::ElementCreationDescription{NULL, Rect( 100,100,200,400 )} );
		panel->m_contents = "Test Panel";

		// Create layout
		layout = dusk->Add<dusk::layouts::Vertical>( dusk::LayoutCreationDescription{panel} );
		static_cast<dusk::layouts::Vertical*>(layout)->m_margin = {0, 40};

		// Create test buttons
		button = dusk->Add<dusk::elements::Button>( dusk::ElementCreationDescription{layout, Rect( 20,45,150,30 )} );
		button->m_contents = "Button 1";
		button->as<dusk::elements::Button>()->m_emphasizeVisuals = true;

		button = dusk->Add<dusk::elements::Button>( dusk::ElementCreationDescription{layout, Rect( 20,70,150,30 )} );
		button->m_contents = "Button 2";

		button = dusk->Add<dusk::elements::Button>( dusk::ElementCreationDescription{layout, Rect( 20,95,150,30 )} );
		button->m_contents = "Button 3";

		button = dusk->Add<dusk::elements::Button>( dusk::ElementCreationDescription{layout, Rect( 20,120,150,30 )} );
		button->m_contents = "Button 4";

		auto slider = dusk->Add<dusk::elements::Slider<Real32>>( dusk::ElementCreationDescription{layout, Rect( 0,0, 150,30 )} );
		slider->m_range_min = 0.0F;
		slider->m_range_max = 4.0F;
		slider->m_value = 1.0F;

		dusk->RemoveReference();
	}

	// Start off the clock timer
	Time::Init();
	// Run main loop
	while ( !aWindow.IsDone() )
	{
		// Only update when all the messages have been looked at
		if ( aWindow.UpdateMessages() ) // (this returns true when messages done)
		{
			core::OnApplicationGlobalTick();
			// Update delta time since last step
			Time::Tick();
			// Update Steam's state
			if ( bSteamy ) {
				SteamAPI_RunCallbacks();
			}
			// Toggle fullscreen
			if ( core::Input::Keydown( core::kVkF4 ) ) {
				aWindow.SetFullscreen(!aWindow.IsFullscreen());
			}
			// Take screenshot
			if ( core::Input::Keydown( core::kVkF1 ) ) {
				RrScreenshot ss;
				ss.SaveTimestampedToPNG();
			}
			// Update game
			TimeProfiler.BeginTimeProfile( "MN_gamestate" );
			aGameState.Update();
			aGameState.LateUpdate();
			TimeProfiler.EndTimeProfile( "MN_gamestate" );
			// Grab inputs
			core::Input::Update();
			// Update audio
			TimeProfiler.BeginTimeProfile( "MN_audio" );
			aManager.Update(Time::deltaTime);
			TimeProfiler.EndTimeProfile( "MN_audio" );
			{
				// Draw a debug grid
				for (int x = -5; x <= 5; ++x)
				{
					for (int y = -5; y <= 5; ++y)
					{
						for (int z = -5; z <= 5; ++z)
						{
							debug::Drawer->DrawLine(Vector3f((Real32)x, (Real32)y, -10), Vector3f((Real32)x, (Real32)y, +10));
							debug::Drawer->DrawLine(Vector3f((Real32)x, -10, (Real32)z), Vector3f((Real32)x, +10, (Real32)z));
							debug::Drawer->DrawLine(Vector3f(-10, (Real32)y, (Real32)z), Vector3f(+10, (Real32)y, (Real32)z));
						}
					}
				}
				// Rotate camera a bit
				l_cam->transform.rotation = Rotator(0, 0, sinf(Time::currentTime) * 30.0F);
			}
			// Redraw window
			TimeProfiler.BeginTimeProfile( "MN_renderer" );
			aRenderer->Render();
			TimeProfiler.EndTimeProfile( "MN_renderer" );
			// Clear all inputs
			core::Input::PreUpdate();
			// Update resources
			core::ArResourceManager::Active()->Update();
			// Update the title with the framerate
			char szTitle[512] = {0};
			sprintf(szTitle, "1Engine Test: Game Common Modules, (FPS: %d) (FT: %d ms)", int(1.0F / Time::smoothDeltaTime), int(Time::smoothDeltaTime * 1000.0F));
			aWindow.SetTitle(szTitle);
		}
		// Check for exiting type of input
		if ( aWindow.IsActive() )
		{
			if ( ( aGameState.EndingGame() ) || ( core::Input::Key( core::kVkAlt ) && core::Input::Keydown( core::kVkF4 ) ) )
			{
				aWindow.PostEndMessage();
			}
		}
	}

	// Free up the scene objects created
	delete l_cam;

	// Save all app data
	gameSettings.SaveSettings();
	// End Steam
	SteamAPI_Shutdown();
	// Clean game
	aGameState.CleanWorld();

	// Free Physics
	PrPhysics::FreeInstance();
	// Free input
	core::Input::Free();

	return 0;
}
