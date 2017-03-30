


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

#include "core/math/Vector3d.h"
#include "core/math/Quaternion.h"
#include "core/math/matrix/CMatrix.h"

// Steam Include
#include "steam/steam_api.h"

// Program entry point
/*#ifdef _WIN32
int WINAPI WinMain(	HINSTANCE	hInstance,			// Instance
					HINSTANCE	hPrevInstance,		// Previous Instance
					LPSTR		lpCmdLine,			// Command Line Parameters
					int			nCmdShow)			// Window Show State
#else
int main ( char** lpCmdLine )
#endif*/
DEPLOY_API int _ARUNIT_CALL Unit::Test_Core ( _ARUNIT_ARGS )
{	_ARUNIT_BUILD_CMDLINE

	// In core, need to ensure that the conversions are working properly.
	{ // Test matrix math
		Matrix4x4 mat;
		mat.setRotation( 10,20,30 );
		for ( int i = 0; i < 16; ++i ) {
			std::cout << mat.pData[i] << ' ';
			if ( i%4==3) std::cout << std::endl;
		}

		Quaternion quat (mat.getEulerAngles());
		std::cout << quat << std::endl;

		mat.setRotation( quat );
		for ( int i = 0; i < 16; ++i ) {
			std::cout << mat.pData[i] << ' ';
			if ( i%4==3) std::cout << std::endl;
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

	// Load window settings
	CGameSettings gameSettings;
	gameSettings.s_cmd = lpCmdLine;
	if ( CGameSettings::Active()->b_ro_Enable30Steroscopic ) {
		MessageBox( NULL, "Stereoscopic 3D mode either currently cascades into memory hell or isn't implemented.", "Invalid system setting", 0 );
		return 0;
	}

	// Initialize input
	CInput::Initialize();

	// Create debug console
	debug::CDebugConsole::Init();

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
		CInput::Update();
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