#include "deploy/unit/unit.h"

// System Includes
#include "core/math/random/Random.h"
#include "core/time.h"
#include "core/settings/CGameSettings.h"
#include "core/input/CInput.h"
#include "core/debug/console.h"
#include "core/types/arBaseObject.h"

#include "core-ext/profiler/CTimeProfiler.h"
#include "core-ext/threads/Jobs.h"
#include "core-ext/resources/ResourceManager.h"
#include "core-ext/system/shell/Status.h"

// Include audio
#include "audio/AudioMaster.h"

// Include physics
#include "physical/module_physical.h"
#include "physical/physics/PrPhysics.h"

// Include gamestate
#include "engine/module_engine.h"
#include "engine/state/CGameState.h"

// Include renderer
#include "renderer/camera/RrCamera.h"
#include "renderer/module_renderer.h"
#include "renderer/windowing/RrWindow.h"
#include "renderer/state/RrRenderer.h"
#include "renderer/utils/RrScreenshot.h"
#include "renderer/debug/RrDebugDrawer.h"

// Steam Include
#include "steam/steam_api.h"

// Classes for engine test
#include "engine/audio/AudioInterface.h"

int ARUNIT_CALL ARUNIT_MAIN ( ARUNIT_ARGS )
{	ARUNIT_BUILD_CMDLINE
	
	// Load window settings
	CGameSettings gameSettings ( (string)lpCmdLine );

	// Create jobs system
	Jobs::System jobSystem (4);

	// Create resource system
	core::ArResourceManager::Active()->Initialize();

	// Initialize input
	CInput::Initialize();

	// Create Window
	RrWindow aWindow ( hInstance, hPrevInstance, lpCmdLine, nCmdShow );
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

	// Create Renderstate
	RrRenderer* aRenderer = new RrRenderer(NULL); // passing null creates default resource manager
	aWindow.AttachRenderer(aRenderer); // Set the window's renderer (multiple possible render states)

	// Init Physics
	PrPhysics::Active()->Initialize();
	// Create Gamestate
	CGameState aGameState;

	// Create Audio
	audio::Master aMaster;
	debug::Console->PrintMessage( "Audio master created.\n" );

	// Inialize steam
	bool bSteamy = SteamAPI_Init();
	debug::Console->PrintMessage( "holy shit it's STEAMy!\n" );

	// Create the game scene
	RrCamera* l_cam = new RrCamera;
	engine::Sound* l_music = core::Orphan(engine::Audio.PlaySound("Music.TestAudio"));
	l_music->SetLooped(true);

	// Start off the clock timer
	Time::Init();
	// Run main loop
	while ( !aWindow.IsDone() )
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
				aWindow.SetFullscreen(!aWindow.IsFullscreen());
			}
			// Take screenshot
			if ( Input::Keydown( Keys.F11 ) ) {
				RrScreenshot ss;
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
			// Perform the debug rendering test:
			{
				debug::Drawer->DrawLine(Vector3f(-100, -100, -100), Vector3f(+100, +100, +100), Color(1, 1, 0, 1));
			}
			// Redraw window
			TimeProfiler.BeginTimeProfile( "MN_renderer" );
			aRenderer->Render();
			TimeProfiler.EndTimeProfile( "MN_renderer" );
			// Clear all inputs
			Input::PreUpdate();
		}
		// Check for exiting type of input
		if ( aWindow.IsActive() )
		{
			if ( ( aGameState.EndingGame() ) || ( Input::Key( Keys.Alt ) && Input::Keydown( Keys.F4 ) ) )
			{
				aWindow.PostEndMessage();
			}
		}
	}

	// Free up the game scene created
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
	CInput::Free();

	return 0;
}
