
#ifdef _WIN32

#include <ctime>

// System Includes
#include "core/math/random/Random.h"
#include "core/time.h"
#include "core/settings/CGameSettings.h"
#include "core/input/CInput.h"
#include "core/debug/console.h"

#include "renderer/window/COglWindow.h"
#include "renderer/state/CRenderState.h"

#include "engine/state/CGameState.h"

/*#include "CDebugDrawer.h"
#include "CDebugRTInspector.h"
#include "CSpriteContainer.h"
#include "CPhysics.h"
#include "audio/CAudioMaster.h"
#include "CLuaController.h"
#include "CResourceManager.h"

#include "CTimeProfiler.h"

#include "glScreenshot.h"


// Scene Includes
#include "gmsceneSystemLoader.h"*/

// Steam Include
#include "steam/steam_api.h"

// Instance Limiting
#ifdef _ENGINE_RELEASE
#include "LimitSingleInstance.h"
CLimitSingleInstance g_SingleInstanceObj(TEXT("Global\\{479DFDD7-3051-4a3f-B1C2-6ECCCB2A362D}"));
/*static const GUID <<name>> = 
{ 0x479dfdd7, 0x3051, 0x4a3f, { 0xb1, 0xc2, 0x6e, 0xcc, 0xcb, 0x2a, 0x36, 0x2d } };*/
#endif

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
	BOOL f=HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

	// Set randomizer based on current CPU time
	{	// This gets reset as soon as a Perlin noice class is added.
		LARGE_INTEGER i_rand;
		QueryPerformanceCounter( &i_rand );
		srand( (uint32_t)i_rand.QuadPart );
		rand();

		Random.Seed( (uint32_t)i_rand.QuadPart );
	}

	// Load window settings
	CGameSettings gameSettings;
	gameSettings.s_cmd = lpCmdLine;
	if ( CGameSettings::Active()->b_ro_Enable30Steroscopic ) {
		MessageBox( NULL, "Stereoscopic 3D mode either currently cascades into memory hell or isn't implemented.", "Invalid system setting", 0 );
		return 0;
	}

	// Initialize input
	CInput::Initialize();

	// Create Window
	COglWindow aWindow( hInstance, hPrevInstance, lpCmdLine, nCmdShow );
	Debug::Console->PrintMessage( "Main system initialized properly. I think.\n" );
	std::cout << "Win32 Build (" << __DATE__ << ") Prealpha" << std::endl;

	// Now that all systems are ready, create the resource manager.
	// The resource manager is given to gamestate and renderstate.
	//CResourceManager aResourceManager;
	//aRenderer.mResourceManager = &aResourceManager;
	// Create Renderstate
	CRenderState aRenderer (NULL); // passing null creates default resource manager
	aWindow.mRenderer = &aRenderer; // Set the window's renderer (multiple possible render states)
	// Create Gamestate
	CGameState aGameState;
	aGameState.mResourceManager = &aResourceManager;
	// Create Audio
	CAudioMaster aMaster;
	// Create Lua
	Lua::CLuaController aLuaMasta;
	// Init Physics
	Physics::Init();
	
	// Inialize steam
	bool bSteamy = SteamAPI_Init();
	Debug::Console->PrintMessage( "holy shit it's STEAMy!\n" );

	// Create the debug drawers
	//Debug::CDebugDrawer debugDrawer;
	//Debug::CDebugRTInspector debugRTInspector;
	// Create the sprite drawer
	//CSpriteContainer spriteContainer;

	// Create the game scene
	CGameScene* pNewScene = CGameScene::NewScene<gmsceneSystemLoader> ();
	CGameScene::SceneGoto( pNewScene );

	// Set up the thread priority
	{
		HANDLE currentThread = GetCurrentThread();
		SetThreadPriority( currentThread, THREAD_PRIORITY_HIGHEST );
	}

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
			CInput::Update();
			// Update audio
			TimeProfiler.BeginTimeProfile( "MN_audio" );
			aMaster.Update();
			TimeProfiler.EndTimeProfile( "MN_audio" );
			// Redraw window
			TimeProfiler.BeginTimeProfile( "MN_renderer" );
			aWindow.Redraw();
			TimeProfiler.EndTimeProfile( "MN_renderer" );
			// Clear all inputs
			CInput::PreUpdate();
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
	SteamAPI_Shutdown();
	// Clean game
	aGameState.CleanWorld();

	// Free Physics
	Physics::Free();
	// Free input
	CInput::Free();

	return 0;
}

#endif



/*{ // Test matrix math
	Matrix4x4 mat;
	mat.setRotation( 10,20,30 );
	for ( int i = 0; i < 16; ++i ) {
		cout << mat.pData[i] << ' ';
		if ( i%4==3) cout << endl;
	}

	Quaternion quat (mat.getEulerAngles());
	//Quaternion quat (mat);
	cout << quat << endl;

	mat.setRotation( quat );
	for ( int i = 0; i < 16; ++i ) {
		cout << mat.pData[i] << ' ';
		if ( i%4==3) cout << endl;
	}

	quat = Quaternion (mat.getEulerAngles());
	//quat = Quaternion (mat);
	cout << quat << endl;

	cout << "done" << endl;
}*/