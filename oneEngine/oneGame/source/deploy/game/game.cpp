//#include "physical/liscensing.cxx" // Include liscense info

#include "deploy/game/game.h"

// System Includes
#include "core/math/random/Random.h"
#include "core/time.h"
#include "core/settings/CGameSettings.h"
#include "core/input/CInput.h"
#include "core/debug/console.h"

#include "core-ext/profiler/CTimeProfiler.h"
#include "core-ext/threads/Jobs.h"
#include "core-ext/system/shell/Status.h"

// Include audio
#include "audio/CAudioMaster.h"

// Include physics
#include "physical/module_physical.h"
//#include "physical/physics/CPhysics.h"
#include "physical/physics/PrPhysics.h"

// Include gamestate
#include "engine/module_engine.h"
#include "engine/state/CGameState.h"
#include "engine/utils/CDeveloperConsole.h"

// Include renderer
#include "renderer/camera/CCamera.h"
#include "renderer/module_renderer.h"
#include "renderer/window/RrWindow.h"
#include "renderer/state/CRenderState.h"
#include "renderer/utils/glScreenshot.h"

// Include engine-common
//#include "engine-common/engine-common.h"
//#include "engine-common/utils/CDeveloperConsoleUI.h"
#include "engine-common/lua/CLuaController.h"
#include "engine-common/scenes/gmsceneSystemLoader.h"

//#include "m04/m04-common.h"
//#include "renderer/debug/CDebugDrawer.h"

// Steam Include
#include "steam/steam_api.h"


DEPLOY_API int _ARUNIT_CALL Deploy::Game ( _ARUNIT_ARGS )
{	_ARUNIT_BUILD_CMDLINE

	// Load window settings
	CGameSettings gameSettings;
	gameSettings.s_cmd = lpCmdLine;
	if ( CGameSettings::Active()->b_ro_Enable30Steroscopic )
	{
		std::cerr << "Stereoscopic 3D mode either currently cascades into memory hell or isn't implemented." << std::endl;
		return 0;
	}

	// Create jobs system
	Jobs::System jobSystem (4);

	// Initialize input
	CInput::Initialize();

	// Create Window
	RrWindow aWindow( hInstance, hPrevInstance, lpCmdLine, nCmdShow );
	debug::Console->PrintMessage( "Windowing system initialized.\n" );
	std::cout << __OS_STRING_NAME__ " Build (" __DATE__ ") Indev" << std::endl;
	
	// Set shell status (loading engine)
	core::shell::SetTaskbarProgressHandle(aWindow.GetShellHandle());
	core::shell::SetTaskbarProgressValue(NIL, 100, 100);
	core::shell::SetTaskbarProgressState(NIL, core::shell::kTaskbarStateIndeterminate);

	// Create Renderstate
	CRenderState aRenderer (NULL); // passing null creates default resource manager
	aWindow.mRenderer = &aRenderer; // Set the window's renderer (multiple possible render states)

	// Init Physics
	PrPhysics::Active()->Initialize();
	// Create Gamestate
	CGameState aGameState;

	// Create Audio
	CAudioMaster aMaster;
	
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

	// Create the debug drawers
	//debug::CDebugDrawer debugDrawer;
	//debug::CDebugRTInspector debugRTInspector;

	// Create the game scene
	CGameScene* pNewScene = CGameScene::NewScene<gmsceneSystemLoader>();
	CGameScene::SceneGoto( pNewScene );
	//EngineCommon::LoadScene("default");

	// Set shell state (done loading engine)
	core::shell::SetTaskbarProgressValue(NIL, 100, 100);

	// Start off the clock timer
	Time::Init();
	// Run main loop
	while ( aWindow.canContinue() )
	{
		// Only update when all the messages have been looked at
		if ( aWindow.UpdateMessages() ) // (this returns true when messages done)
		{
			// Update delta time since last step
			Time::Tick();
			// Update Steam's state
			if ( bSteamy ) {
				SteamAPI_RunCallbacks();
			}
			// Toggle fullscreen
			if ( Input::Keydown( Keys.F4 ) ) {
				aWindow.toggleFullscren();
			}
			// Take screenshot
			if ( Input::Keydown( Keys.F11 ) ) {
				glScreenshot ss;
				ss.SaveTimestampedToPNG();
			}
			// Update game
			TimeProfiler.BeginTimeProfile( "MN_gamestate" );
			aGameState.Update();
			aGameState.LateUpdate();
			TimeProfiler.EndTimeProfile( "MN_gamestate" );
			// Grab inputs
			Input::Update();
			// Update audio
			TimeProfiler.BeginTimeProfile( "MN_audio" );
			aMaster.Update();
			TimeProfiler.EndTimeProfile( "MN_audio" );
			// Redraw window
			TimeProfiler.BeginTimeProfile( "MN_renderer" );
			aWindow.Redraw();
			TimeProfiler.EndTimeProfile( "MN_renderer" );
			// Clear all inputs
			Input::PreUpdate();
		}
		// Check for exiting type of input
		if ( aWindow.isActive() )
		{
			if ( ( aGameState.EndingGame() ) || ( Input::Key( Keys.Alt ) && Input::Keydown( Keys.F4 ) ) )
			{
				aWindow.sendEndMessage();
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
	CInput::Free();

	return 0;
}
