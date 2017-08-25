
#ifdef _WIN32

//#include "physical/liscensing.cxx" // Include liscense info

#include "deploy/unit/unit.h"

// System Includes
#include "core/math/random/Random.h"
#include "core/time.h"
#include "core/settings/CGameSettings.h"
#include "core/input/CInput.h"
#include "core/debug/console.h"

#include "core-ext/profiler/CTimeProfiler.h"
#include "core-ext/threads/Jobs.h"

// Include audio
#include "audio/CAudioMaster.h"

// Include physics
#include "physical/module_physical.h"
#include "physical/physics/PrPhysics.h"

// Include gamestate
#include "engine/module_engine.h"
#include "engine/state/CGameState.h"

// Include renderer
#include "renderer/camera/CCamera.h"
#include "renderer/module_renderer.h"
#include "renderer/window/RrWindow.h"
#include "renderer/state/CRenderState.h"
#include "renderer/utils/glScreenshot.h"

// Steam Include
#include "steam/steam_api.h"

DEPLOY_API int ARUNIT_CALL Unit::Test_EngineCore ( ARUNIT_ARGS )
{	ARUNIT_BUILD_CMDLINE
	// Load window settings
	CGameSettings gameSettings;
	gameSettings.s_cmd = lpCmdLine;
	if ( CGameSettings::Active()->b_ro_Enable30Steroscopic ) {
		MessageBox( NULL, "Stereoscopic 3D mode either currently cascades into memory hell or isn't implemented.", "Invalid system setting", 0 );
		return 0;
	}

	// Create jobs system
	Jobs::System jobSystem (4);

	// Initialize input
	CInput::Initialize();

	// Create Window
	RrWindow aWindow( hInstance, hPrevInstance, lpCmdLine, nCmdShow );
	debug::Console->PrintMessage( "Main system initialized properly. I think.\n" );
	std::cout << "Win32 Build (" << __DATE__ << ") Prealpha" << std::endl;

	// Init Physics
	PrPhysics::Active()->Initialize();
	// Create Renderstate
	CRenderState aRenderer (NULL); // passing null creates default resource manager
	aWindow.mRenderer = &aRenderer; // Set the window's renderer (multiple possible render states)
	// Create Gamestate
	CGameState aGameState;
	// Create Audio
	CAudioMaster aMaster;
	
	// Inialize steam
	bool bSteamy = SteamAPI_Init();
	debug::Console->PrintMessage( "holy shit it's STEAMy!\n" );

	// Create the debug drawers
	//debug::CDebugDrawer debugDrawer;
	//debug::CDebugRTInspector debugRTInspector;
	// Create the sprite drawer
	//CSpriteContainer spriteContainer;

	// Create the game scene
	//CGameScene* pNewScene = CGameScene::NewScene<gmsceneSystemLoader> ();
	//CGameScene::SceneGoto( pNewScene );
	CCamera* cam = new CCamera;

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

	delete cam;

	// Save all app data
	gameSettings.SaveSettings();
	// End Steam
	SteamAPI_Shutdown();
	// Clean game
	aGameState.CleanWorld();

	// Free Physics
	PrPhysics::FreeInstance();
	// Free input
	CInput::Free();

	return 0;
}

#endif