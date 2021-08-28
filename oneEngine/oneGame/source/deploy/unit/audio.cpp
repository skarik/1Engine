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
#include "core-ext/threads/Jobs.h"

// Include audio
#include "audio/Manager.h"

// Steam Include
#include "steam/steam_api.h"

// Classes for audio test
#include "audio/Listener.h"
#include "audio/BufferManager.h"
#include "audio/Source.h"
#include "audio/effects/LowPass1.h"

// Program entry point
int ARUNIT_CALL ARUNIT_MAIN ( ARUNIT_ARGS )
{	ARUNIT_BUILD_CMDLINE
	// Load window settings
	CGameSettings gameSettings ( (string)lpCmdLine, false );

	// Create jobs system
	core::jobs::System jobSystem (4);

	// Initialize input
	core::Input::Initialize();

	// Create debug console
	debug::ConsoleWindow::Init();

	// Create Window
	debug::Console->PrintMessage( "Main system initialized properly. I think.\n" );
	std::cout << "Win32 Build (" << __DATE__ << ") Prealpha" << std::endl;

	// Create Audio
	audio::Manager aManager;
	debug::Console->PrintMessage( "Audio manager created.\n" );

	// Inialize steam
	//bool bSteamy = SteamAPI_Init();
	//debug::Console->PrintMessage( "holy shit it's STEAMy!\n" );
	debug::Console->PrintWarning( "Press Escape to exit this module test.\n" );
	debug::Console->PrintWarning( "Press Space to play a click @ random position, WASD to control music direction.\n" );
	
	// Set up the thread priority
	{
		HANDLE currentThread = GetCurrentThread();
		SetThreadPriority( currentThread, THREAD_PRIORITY_HIGHEST );
	}

	// Create a listener
	audio::Listener* l_listener = new audio::Listener();
	l_listener->orient_forward = Vector3f(0, 1, 0);
	//l_listener->position = Vector3f(-140, 140, 0);

	// Create the music track to loop
	audio::Buffer* l_musicBuffer = audio::BufferManager::Active()->GetSound(".resbackup-0/sounds/music/princess-loop.ogg");
	//audio::Buffer* l_musicBuffer = audio::BufferManager::Active()->GetSound(".resbackup-0/sounds/music/Theme07-snippet.ogg");
	audio::Source* l_musicSource = new audio::Source(l_musicBuffer);
	l_musicSource->state.looped = true;
	l_musicSource->state.position = Vector3f(0, 1, 0);
	l_musicSource->state.pitch = 1.0F;
	l_musicSource->state.channel = audio::MixChannel::kMusic;
	l_musicSource->Play(true);

	audio::effect::LowPass1* l_lowPass = new audio::effect::LowPass1 (audio::MixChannel::kMusic);
	l_lowPass->m_state.m_cutoffFade = 400;
	l_lowPass->m_state.m_strength = 0.9F;

	audio::Source* l_soundSource = NULL;

	// Start off the clock timer
	Time::Init();
	// Run main loop
	while ( true )
	{
		// Update delta time since last step
		Time::Tick();
		// Update Steam's state
		//if ( bSteamy ) {
		//	SteamAPI_RunCallbacks();
		//}
		// Grab inputs
		core::Input::Update();
		// Update audio
		aManager.Update(Time::deltaTime);
		// Clear all inputs
		core::Input::PreUpdate();

		// Check for escape
		int kbkey = NIL;
		if (KBHIT())
		{
			kbkey = GETC();
			if ( kbkey == core::kVkEscape ) {
				break;
			}
		}

		//l_musicSource->state.position.x += Time::deltaTime * 3.0F;
		//l_musicSource->state.velocity.x = 300.0F;

		l_lowPass->m_state.m_strength = 0.5F + 0.5F * std::sin(Time::currentTime);

		// Check for other characters:

		switch (kbkey)
		{
		case ' ': {
				// Free previous sound
				if (l_soundSource != NULL)
				{
					l_soundSource->Destroy();
				}

				// Play a new sound
				audio::Buffer* sound = audio::BufferManager::Active()->GetSound(".resbackup-0/sounds/menu/click.wav");
				audio::Source* source = new audio::Source(sound);
				source->state.position = Random.PointInUnitSphere() * 400.0F;
				source->Play(true);

				// Save this source
				l_soundSource = source;
			}
			break;
		case 'a': {
				l_musicSource->state.position = Vector3f(-3, 0, 0);
			}
			break;
		case 'w': {
				l_musicSource->state.position = Vector3f(0, 1, 0);
		}
			break;
		case 'd': {
				l_musicSource->state.position = Vector3f(1, 0, 0);
			}
			break;
		case 's': {
				l_musicSource->state.position = Vector3f(0, -1, 0);
			}
			break;
		case 'z': {
				l_musicSource->state.position = Vector3f(-140, 140, 0);
			}
			break;
		case '1': {
				l_musicSource->state.spatial = 1.0F;
			}
			break;
		case '2': {
				l_musicSource->state.spatial = 0.75F;
			}
			break;
		case '3': {
				l_musicSource->state.spatial = 0.5F;
			}
			break;
		case '4': {
				l_musicSource->state.spatial = 0.25F;
			}
			break;
		case '5': {
				l_musicSource->state.spatial = 0.0F;
			}
			break;
		}
	}

	l_listener->Destroy();
	l_musicSource->Destroy();
	l_lowPass->Destroy();
	if (l_soundSource != NULL)
	{
		l_soundSource->Destroy();
	}

	// Save all app data
	gameSettings.SaveSettings();
	// End Steam
	//SteamAPI_Shutdown();

	// Free input
	core::Input::Free();

	// Free debug console
	debug::ConsoleWindow::Free();

	return 0;
}