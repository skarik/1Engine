
#include "deploy/unit/unit.h"
#ifdef _WIN32
#include "conio.h"
#endif

// System Includes
#include "core/math/random/Random.h"
#include "core/time.h"
#include "core/settings/CGameSettings.h"
#include "core/input/CInput.h"
#include "core/debug/console.h"

// Include audio
#include "audio/CAudioMaster.h"

// Steam Include
#include "steam/steam_api.h"

// Program entry point
int ARUNIT_CALL ARUNIT_MAIN ( ARUNIT_ARGS )
{	ARUNIT_BUILD_CMDLINE
	// Load window settings
	CGameSettings gameSettings ( (string)lpCmdLine );

	// Initialize input
	CInput::Initialize();

	// Create debug console
	debug::CDebugConsole::Init();

	// Create Window
	debug::Console->PrintMessage( "Main system initialized properly. I think.\n" );
	std::cout << "Win32 Build (" << __DATE__ << ") Prealpha" << std::endl;

	// Create Audio
	CAudioMaster aMaster;
	
	// Inialize steam
	bool bSteamy = SteamAPI_Init();
	debug::Console->PrintMessage( "holy shit it's STEAMy!\n" );
	debug::Console->PrintWarning( "Press Escape to exit this module test.\n" );
	
	// Set up the thread priority
	{
		HANDLE currentThread = GetCurrentThread();
		SetThreadPriority( currentThread, THREAD_PRIORITY_HIGHEST );
	}

	// Start off the clock timer
	Time::Init();
	// Run main loop
	while ( true )
	{
		// Update delta time since last step
		Time::Tick();
		// Update Steam's state
		if ( bSteamy ) {
			SteamAPI_RunCallbacks();
		}
		// Grab inputs
		CInput::Update();
		// Update audio
		aMaster.Update();
		// Clear all inputs
		CInput::PreUpdate();

		// Check for escape
#ifdef _WIN32
		int kbhit = _getch();
#else
		int kbhit = getc();
#endif
		if ( kbhit == CKeys::Escape ) {
			break;
		}
	}

	// Save all app data
	gameSettings.SaveSettings();
	// End Steam
	SteamAPI_Shutdown();

	// Free input
	CInput::Free();

	return 0;
}