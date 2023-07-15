// If the environment is destroyed, add the following to the PATH variable in the runtime options:
// PATH=%PATH%;$(ProjectDir)

#include "core/types/types.h"
#include "core/types/exports.h"
#include "core-ext/core-ext.h"
#include "deploy/game/game.h"

DEPLOY_API int _ARUNIT_CALL RunSeqEditor ( _ARUNIT_ARGS );

#ifdef _WIN32

// Instance Limiting
#ifdef _ENGINE_RELEASE
#include "deploy/win32/LimitSingleInstance.h"
CLimitSingleInstance g_SingleInstanceObj(TEXT("Global\\{479DFDD7-3051-4a3f-B1C2-6ECCCB2A362D}"));
/*static const GUID <<name>> = 
{ 0x479dfdd7, 0x3051, 0x4a3f, { 0xb1, 0xc2, 0x6e, 0xcc, 0xcb, 0x2a, 0x36, 0x2d } };*/
#endif

#include <crtdbg.h>

// Program entry point
int WINAPI WinMain(	HINSTANCE	hInstance,			// Instance
					HINSTANCE	hPrevInstance,		// Previous Instance
					LPSTR		lpCmdLine,			// Command Line Parameters
					int			nCmdShow)			// Window Show State
{
	// Limit the program to a single instance
	#ifdef _ENGINE_RELEASE
		if ( g_SingleInstanceObj.IsAnotherInstanceRunning() )
		{
			MessageBox( NULL, "Another instance of the application is already running.", "Aborting", 0 );
			return 0;
		}
	#endif

	// Throw exception or crash when the memory heap gets corrupted in order to track down larger errors.
	BOOL f = HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

	// Call core init CBs
	core::OnApplicationStartup();

	// Call the Unit to run
	//int returnCode = Deploy::Game(hInstance,hPrevInstance,lpCmdLine,nCmdShow);
	int returnCode = RunSeqEditor(hInstance,hPrevInstance,lpCmdLine,nCmdShow);

	// Call core free CBs
	core::OnApplicationEnd();

	return returnCode;
}

#endif

// System Includes
#include "core/math/random/Random.h"
#include "core/time.h"
#include "core/settings/CGameSettings.h"
#include "core/input/CInput.h"
#include "core/debug/console.h"

#include "core-ext/core-ext.h"
#include "core-ext/profiler/CTimeProfiler.h"
#include "core-ext/threads/Jobs.h"
#include "core-ext/resources/ResourceManager.h"
#include "core-ext/system/shell/Status.h"

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

// Include editor
#include "m04-editor/standalone/seqeditor/SequenceEditor.h"

DEPLOY_API int _ARUNIT_CALL RunSeqEditor ( _ARUNIT_ARGS )
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

	// Create the engine systems
	engine::CDeveloperConsole* engConsole = new engine::CDeveloperConsole();

	// Now, create the fake scene
	m04::editor::SequenceEditor* editor;
	{
		// Create the sequence editor
		{
			editor = new m04::editor::SequenceEditor();
			editor->RemoveReference(); // So it can be destroyed when the game quits
		}

		// Print a prompt
		debug::Console->PrintMessage( "You are running a debug build of M04.\n" );
		debug::Console->PrintMessage( "This scene provides a sequence editor and no other items.\n" );
		debug::Console->PrintMessage( "Enjoy.\n" );
	}

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
			core::OnApplicationGlobalTick();
			// Update delta time since last step
			Time::Tick();
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
			/*TimeProfiler.BeginTimeProfile( "MN_audio" );
			aMaster.Update(Time::deltaTime);
			TimeProfiler.EndTimeProfile( "MN_audio" );*/
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

		// Check for closed editor
		if ( !aGameState.ObjectExists(editor) )
		{
			aWindow.PostEndMessage();
		}
	}

	// Save all app data
	gameSettings.SaveSettings();
	// End Steam
	//if (bSteamy) SteamAPI_Shutdown();
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