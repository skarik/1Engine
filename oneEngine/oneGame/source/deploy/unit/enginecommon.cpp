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
#include "core-ext/system/shell/Message.h"

// Include audio
#include "audio/AudioMaster.h"

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

int ARUNIT_CALL Unit::Test_EngineCommon ( ARUNIT_ARGS )
{	ARUNIT_BUILD_CMDLINE

	// Load window settings
	CGameSettings gameSettings ( (string)lpCmdLine );

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
	CInput::Initialize();

	// Create Window
	RrWindow aWindow( hInstance, hPrevInstance, lpCmdLine, nCmdShow );
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

	// Initialize steam
	bool bSteamy = SteamAPI_Init();
	debug::Console->PrintMessage( "holy shit it's STEAMy!\n" );
	
	// Create the engine systems
	//Lua::CLuaController* luaController = new Lua::CLuaController();
	engine::CDeveloperConsole* engConsole = new engine::CDeveloperConsole();

	// Set up engine component
	EngineCommonInitialize();

	// Create the game scene
	CGameScene* pNewScene = CGameScene::NewScene<gmsceneSystemLoader> ();
	CGameScene::SceneGoto( pNewScene );

	// Create debug camera to show stuff
	RrCamera* l_cam = new RrCamera;

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
			Input::PreUpdate();
			// Update the title with the framerate
			char szTitle[512] = {0};
			sprintf(szTitle, "1Engine Test: Game Common Modules, (FPS: %d) (FT: %d ms)", int(1.0F / Time::smoothDeltaTime), int(Time::smoothDeltaTime * 1000.0F));
			aWindow.SetTitle(szTitle);
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
	CInput::Free();

	return 0;
}
