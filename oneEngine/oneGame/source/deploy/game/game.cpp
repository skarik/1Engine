#include "deploy/game/game.h"

// System Includes
#include "core/math/random/Random.h"
#include "core/time.h"
#include "core/settings/CGameSettings.h"
#include "core/input/CInput.h"
#include "core/debug/console.h"

#include "core-ext/profiler/CTimeProfiler.h"
#include "core-ext/threads/Jobs.h"
#include "core-ext/resources/ResourceManager.h"
#include "core-ext/system/shell/Status.h"

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

// Include engine-common
#include "engine-common/lua/CLuaController.h"
#include "engine-common/scenes/gmsceneSystemLoader.h"

// Steam Include
#include "steam/steam_api.h"

// Point at the game setup:
AR_IMPORT int GameInitialize ( void );

DEPLOY_API int _ARUNIT_CALL Deploy::Game ( _ARUNIT_ARGS )
{	_ARUNIT_BUILD_CMDLINE

	// Load window settings
	CGameSettings gameSettings ( (string)lpCmdLine, false );

	// Create console window
	debug::ConsoleWindow::Init(true);

	// Create jobs system
	core::jobs::System jobSystem (std::max(4u, std::thread::hardware_concurrency()));

	// Create resource system
	core::ArResourceManager::Active()->Initialize();

	// Initialize input
	core::Input::Initialize();

	// Create renderer
	RrRenderer* aRenderer = new RrRenderer();

	// Create main window
	RrWindow aWindow ( aRenderer, hInstance );
	if (!aWindow.Show())
	{
		debug::Console->PrintError( "Could not show windowing system.\n" );
		abort();
	}
	debug::Console->PrintMessage( "Windowing system initialized.\n" );
	std::cout << __OS_STRING_NAME__ " Build (" __DATE__ ") Indev" << std::endl;

	// Set shell status (loading engine)
	core::shell::SetTaskbarProgressHandle(aWindow.OsShellHandle());
	core::shell::SetTaskbarProgressValue(NIL, 100, 100);
	core::shell::SetTaskbarProgressState(NIL, core::shell::kTaskbarStateIndeterminate);
	
	// Set up renderer
	uint worldIndex = aRenderer->AddWorldDefault();
	RrOutputInfo mainRenderOutput(aRenderer->GetWorld(worldIndex), &aWindow);
	mainRenderOutput.name = "MainOutput";
	mainRenderOutput.camera = new RrCamera(false);
	aRenderer->AddOutput(mainRenderOutput); // Create an output using the window

	// Init Physics
	PrPhysics::Active()->Initialize();
	// Create Gamestate
	CGameState aGameState;

	// Create Audio
	audio::Manager aMaster;
	
	// Inialize steam
	bool bSteamy = false;
#	ifdef STEAM_ENABLED
	bSteamy = SteamAPI_Init();
#	endif
	if (bSteamy)
		debug::Console->PrintMessage( "STEAM library initialized: holy shit it's STEAMy!\n" );
	else
		debug::Console->PrintWarning( "STEAM was not initialized\n" );

	// Create the engine systems
	//Lua::CLuaController* luaController = new Lua::CLuaController();
	engine::CDeveloperConsole* engConsole = new engine::CDeveloperConsole();

	// Set up engine component - GameInitialized is declared in OneGame and is game-specific.
	GameInitialize();

	// Create the game scene
	CGameScene* pNewScene = CGameScene::NewScene<gmsceneSystemLoader>();
	CGameScene::SceneGoto( pNewScene );
	//EngineCommon::LoadScene("default");

	// Set shell state (done loading engine)
	core::shell::SetTaskbarProgressValue(NIL, 100, 100);

	// Start off the clock timer
	Time::Init();
	// Run main loop
	while ( !aWindow.IsDone() && !aWindow.WantsClose() )
	{
		// Pump message loop for all open windows.
		for (RrWindow* window : RrWindow::List())
		{
			while ( !window->UpdateMessages() ) // Returns true when messages are done
			{
				; // Nothing.
			}
		}
		
		// Only update when all the messages have been looked at
		//if ( aWindow.UpdateMessages() ) // (this returns true when messages done)
		{
			// Update delta time since last step
			Time::Tick();
			// Update Steam's state
			if ( bSteamy ) {
				SteamAPI_RunCallbacks();
			}
			// Toggle fullscreen
			if ( core::Input::Keydown( core::kVkF4 ) && !core::Input::Key( core::kVkAlt ) ) {
				aWindow.SetFullscreen(!aWindow.IsFullscreen());
			}
			// Take screenshot
			/*if ( core::Input::Keydown( core::kVkF11 ) ) {
				RrScreenshot ss;
				ss.SaveTimestampedToPNG();
			}*/
			// Grab inputs
			core::Input::Update();
			// Update game
			TimeProfiler.BeginTimeProfile( "MN_gamestate" );
			aGameState.Update();
			aGameState.LateUpdate();
			TimeProfiler.EndTimeProfile( "MN_gamestate" );
			// Update audio
			TimeProfiler.BeginTimeProfile( "MN_audio" );
			aMaster.Update(Time::deltaTime);
			TimeProfiler.EndTimeProfile( "MN_audio" );
			// Redraw window
			TimeProfiler.BeginTimeProfile( "MN_renderer" );
			aRenderer->Render();
			TimeProfiler.EndTimeProfile( "MN_renderer" );
			// Clear all inputs
			core::Input::PreUpdate();
			// Update resources
			core::ArResourceManager::Active()->Update();
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

	// Save all app data
	gameSettings.SaveSettings();
	// End Steam
	if (bSteamy) SteamAPI_Shutdown();
	// Clean game
	aGameState.CleanWorld();

	// Free Physics
	PrPhysics::FreeInstance();
	// Free input
	core::Input::Free();

	// Free the renderer
	delete aRenderer;
	aWindow.Close();

	// Free resource system
	core::ArResourceManager::FreeInstance();

	// Free console window
	debug::ConsoleWindow::Free();

	return 0;
}
