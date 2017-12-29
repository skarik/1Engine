#include "deploy/unit/unit.h"

#ifdef _WIN32
#	include "conio.h"
#	define GETC _getch
#	define KBHIT _kbhit
#else
#	define GETC getc
#	define KBHIT kbhit
#endif

// System Includes
#include "core/math/random/Random.h"
#include "core/time.h"
#include "core/settings/CGameSettings.h"
#include "core/input/CInput.h"
#include "core/debug/console.h"

// Include audio
#include "audio/AudioMaster.h"

// Steam Include
#include "steam/steam_api.h"

// Classes for audio test
#include "audio/Listener.h"
#include "audio/BufferManager.h"
#include "audio/Source.h"

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
	audio::Master aMaster;
	debug::Console->PrintMessage( "Audio master created.\n" );

	// Inialize steam
	bool bSteamy = SteamAPI_Init();
	debug::Console->PrintMessage( "holy shit it's STEAMy!\n" );
	debug::Console->PrintWarning( "Press Escape to exit this module test.\n" );
	
	// Set up the thread priority
	{
		HANDLE currentThread = GetCurrentThread();
		SetThreadPriority( currentThread, THREAD_PRIORITY_HIGHEST );
	}

	// Create a listener
	audio::Listener* l_listener = new audio::Listener();

	// Create the music track to loop
	audio::Buffer* l_musicBuffer = audio::BufferManager::Active()->GetSound(".resbackup-0/sounds/music/princess-loop.ogg");
	//audio::Buffer* l_musicBuffer = audio::BufferManager::Active()->GetSound(".resbackup-0/sounds/music/Theme07-snippet.ogg");
	audio::Source* l_musicSource = new audio::Source(l_musicBuffer);
	l_musicSource->options.looped = true;
	l_musicSource->Play(true);

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
		int kbkey = NIL;
		if (KBHIT())
		{
			kbkey = GETC();
			if ( kbkey == CKeys::Escape ) {
				break;
			}
		}

		// Check for other characters:

		switch (kbkey)
		{
		case ' ': {
				// Play a sound
				audio::Buffer* sound = audio::BufferManager::Active()->GetSound(".resbackup-0/sounds/menu/click.wav");
				audio::Source* source = new audio::Source(sound);
				source->Play(true);
			}
			break;
		}
	}

	// Clean up audio
	delete l_listener;

	// Save all app data
	gameSettings.SaveSettings();
	// End Steam
	SteamAPI_Shutdown();

	// Free input
	CInput::Free();

	return 0;
}