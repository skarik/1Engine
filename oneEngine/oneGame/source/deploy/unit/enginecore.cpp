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
#include "audio/Manager.h"
#include "audio/Source.h"

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
	CGameSettings gameSettings ( (string)lpCmdLine, false );

	// Create jobs system
	core::jobs::System jobSystem (4);

	// Create resource system
	core::ArResourceManager::Active()->Initialize();

	// Initialize input
	core::Input::Initialize();

	// Create renderer
	RrRenderer* aRenderer = new RrRenderer(); // passing null creates default resource manager

	// Create Window
	RrWindow aWindow ( aRenderer, hPrevInstance, lpCmdLine, nCmdShow );
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
	
	// Set the window title
	aWindow.SetTitle("1Engine Test: Core Modules");

	// Set up renderer
	aRenderer->AddWorldDefault();
	aRenderer->AddOutput(RrOutputInfo(nullptr, &aWindow)); // Create an output using the window

	// Init Physics
	PrPhysics::Active()->Initialize();
	// Create Gamestate
	CGameState aGameState;

	// Create Audio
	audio::Manager aMaster;
	debug::Console->PrintMessage( "Audio manager created.\n" );

	// Inialize steam
	bool bSteamy = SteamAPI_Init();
	debug::Console->PrintMessage( "holy shit it's STEAMy!\n" );

	// Create the game scene
	RrCamera* l_cam = new RrCamera(false);
	engine::Sound* l_music = core::Orphan(engine::Audio.PlaySound("Music.TestAudio"));
	l_music->SetLooped(true);
	l_music->SetGain(0.0F);

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
			aMaster.Update(Time::deltaTime);
			TimeProfiler.EndTimeProfile( "MN_audio" );
			// Update gain:
			{
				// Fade the music in
				float t_currentGain = l_music->mySource->state.gain;
				t_currentGain = std::min(1.0F, t_currentGain + Time::deltaTime);
				l_music->SetGain(t_currentGain);
			}
			// Perform the debug rendering test:
			{
				// Draw the world grid
				const int maxDivs = 40;
				const float divSize = 2.0F;
				const float gridDistance = maxDivs * divSize;
				for (int i = -maxDivs; i <= maxDivs; ++i)
				{
					debug::Drawer->DrawLine(Vector3f(i * divSize, -gridDistance, 0), Vector3f(i * divSize, +gridDistance, 0), Color(0.25F, 0.25F, 0.25F, 0.5F));
					debug::Drawer->DrawLine(Vector3f(-gridDistance, i * divSize, 0), Vector3f(+gridDistance, i * divSize, 0), Color(0.25F, 0.25F, 0.25F, 0.5F));
				}
				// Create the seesaw line test
				Vector3f pointA = Vector3f(20, -10, 10 * sinf(Time::currentTime * 1.3F));
				Vector3f pointB = Vector3f(20, +10, 10 * cosf(Time::currentTime * 1.5F));
				debug::Drawer->DrawLine(pointA, Vector3f(pointA.x, pointA.y, 0), Color(1, 0, 0, 1));
				debug::Drawer->DrawLine(pointB, Vector3f(pointB.x, pointB.y, 0), Color(1, 0, 0, 1));
				debug::Drawer->DrawLine(pointA, pointB, Color(1, 1, 0, 1));
				
				// Move camera above a bit
				l_cam->transform.position.z = 4.0F;
				// Rotate camera a bit
				l_cam->transform.rotation = Rotator(0, 0, sinf(Time::currentTime) * 30.0F);
			}
			// Redraw window
			TimeProfiler.BeginTimeProfile( "MN_renderer" );
			aRenderer->Render();
			TimeProfiler.EndTimeProfile( "MN_renderer" );
			// Clear all inputs
			core::Input::PreUpdate();
			// Update the resource system
			core::ArResourceManager::Active()->Update();
			// Update the title with the framerate
			char szTitle[512] = {0};
			sprintf(szTitle, "1Engine Test: Core Modules, (FPS: %d) (FT: %d ms)", int(1.0F / Time::smoothDeltaTime), int(Time::smoothDeltaTime * 1000.0F));
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
	core::Input::Free();

	return 0;
}
