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

#include "core/math/Vector3.h"
#include "core/math/Quaternion.h"
#include "core/math/matrix/CMatrix.h"

// Steam Include
#include "steam/steam_api.h"

// Program entry point
int ARUNIT_CALL ARUNIT_MAIN ( ARUNIT_ARGS )
{	ARUNIT_BUILD_CMDLINE

	// Load window settings
	CGameSettings gameSettings( (string)lpCmdLine, false );

	// Initialize input
	core::Input::Initialize();

	// Create debug console
	debug::ConsoleWindow::Init();

	// In core, need to ensure that the conversions are working properly.
	{	// Test matrix math
		Matrix4x4 mat;
		mat.setRotation( 10,20,30 );
		for ( int i = 0; i < 16; ++i )
		{
			std::cout << mat.pData[i] << ' ';
			if (i % 4 == 3)
				std::cout << std::endl;
		}

		Quaternion quat (mat.getEulerAngles());
		std::cout << quat << std::endl;

		mat.setRotation( quat );
		for ( int i = 0; i < 16; ++i )
		{
			std::cout << mat.pData[i] << ' ';
			if (i % 4 == 3)
				std::cout << std::endl;
		}

		quat = Quaternion (mat.getEulerAngles());
		std::cout << quat << std::endl;

		std::cout << "done" << std::endl;
	}

	// Set randomizer based on current CPU time
	{	// This gets reset as soon as a Perlin noice class is added.
		LARGE_INTEGER i_rand;
		QueryPerformanceCounter( &i_rand );
		srand( (uint32_t)i_rand.QuadPart );
		rand();

		Random.Seed( (uint32_t)i_rand.QuadPart );
	}

	// Create Window
	debug::Console->PrintMessage( "Main system initialized properly. I think.\n" );
	std::cout << "Win32 Build (" << __DATE__ << ") Prealpha" << std::endl;
	
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
		core::Input::Update();
		// Clear all inputs
		core::Input::PreUpdate();

		// Check for escape
#ifdef _WIN32
		int kbhit = _getch();
#else
		int kbhit = getc();
#endif
		if ( kbhit == core::kVkEscape ) {
			break;
		}
	}

	// Save all app data
	gameSettings.SaveSettings();
	// End Steam
	SteamAPI_Shutdown();

	// Free input
	core::Input::Free();

	return 0;
}