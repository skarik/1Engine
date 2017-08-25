
#include "CAudioMaster.h"
#include "CAudioListener.h"
#include "CAudioSource.h"
#include "CSoundManager.h"
#include "CAudioSound.h"

using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::find;

// Static variable defines
CAudioMaster*	CAudioMaster::pActiveAudioMaster = NULL;

// CAudioMaster function definitions
CAudioMaster::CAudioMaster ( void )
{
	pActiveAudioMaster = this;
	// Create sound master
	CSoundManager::Active = new CSoundManager();

	next_sound_id = 0;

	// Output debug messages
	//cout << "Audio: using Win32 build of OpenAL Soft 1.14 (DirectSound build)" << endl;
	cout << "Audio: start up the audio system..." << endl;
	InitSystem();
	if ( Active() )
		cout << "Creation of audio device successful!" << endl;
	else
		cout << "Error in creating OpenAL device..." << endl;
}

CAudioMaster::~CAudioMaster ( void )
{
	FreeSystem();

	delete CSoundManager::Active;
	CSoundManager::Active = NULL;

	if ( pActiveAudioMaster == this )
		pActiveAudioMaster = NULL;
}

// Update
void CAudioMaster::Update ( void )
{
	if ( !Active() )
		return;

	// Go through the listeners and update them
	for ( vector<CAudioListener*>::iterator it = listeners.begin(); it != listeners.end(); ++it )
	{
		if ( !(*it)->queue_destruction )
		{
			(*it)->Update();
		}
		else
		{
			delete (*it);
			break;
		}
	}


	// Go through the sources and update them
	for ( vector<CAudioSource*>::iterator it = sources.begin(); it != sources.end(); ++it )
	{
		if ( !(*it)->queue_destruction )
		{
			(*it)->Update();
		}
		else
		{
			delete (*it);
			break;
		}
	}

#ifdef _AUDIO_FMOD_
	// Update audio system
	FMOD::FMOD_System_Update( m_system );
#endif
}


// OpenAL starting
void CAudioMaster::InitSystem ( void )
{
	// Number of channels
	const int c_voices = 128;

#ifndef _AUDIO_FMOD_
	// Create device and context
	device	= alcOpenDevice( NULL );
	context	= alcCreateContext( device, NULL );
	alcMakeContextCurrent( context );

	if (( device != NULL )&&( context != NULL ))
	{
		// Print out device version if successful
		int ver_maj, ver_min;
		alcGetIntegerv( device, ALC_MAJOR_VERSION, sizeof(int), &ver_maj );
		alcGetIntegerv( device, ALC_MINOR_VERSION, sizeof(int), &ver_min );

		cout << "  OpenAL version " << ver_maj << "." << ver_min;

		// Print out device using
		const char* str_dev;
		str_dev = alcGetString( device, ALC_DEFAULT_DEVICE_SPECIFIER );
		cout << " using " << str_dev << endl;

		// Assume success here
		active = true;
	}

	// Set options if active
	if ( Active() )
	{
		//alDistanceModel( 
	}
#else
	FMOD::FMOD_RESULT	result;
	unsigned int		version;
	int					numdrivers;
	FMOD::FMOD_SPEAKERMODE	speakermode;
	FMOD::FMOD_CAPS			caps;
	char				name [256];


	// Create a system object
	result = FMOD::FMOD_System_Create( &m_system );
	// Check the version
	FMOD::FMOD_System_GetVersion( m_system, &version );
	if ( version < FMOD_VERSION )
	{
		printf( "You are using an old version of FMOD %08x. This program requires %08x.\n", version, FMOD_VERSION );
		active = false;
		return;
	}
	// Get the drivers that we can use
	result = FMOD::FMOD_System_GetNumDrivers( m_system, &numdrivers );
	if ( numdrivers == 0 )
	{
		result = FMOD::FMOD_System_SetOutput( m_system, FMOD::FMOD_OUTPUTTYPE_NOSOUND );
		printf( "Could not find open audio driver. Sound output currently disabled.\n" );
	}
	else
	{
		// Get the driver caps
		result = FMOD::FMOD_System_GetDriverCaps( m_system, 0, &caps, 0, &speakermode );
		// User selected speaker mode
		result = FMOD::FMOD_System_SetSpeakerMode( m_system, speakermode );
		// Check for hardware emulation
		if ( caps & FMOD_CAPS_HARDWARE_EMULATED )
		{
			// User has the hardware acceleration slider set to off. This is really bad for latency.
			result = FMOD::FMOD_System_SetDSPBufferSize( m_system, 1024, 10 );
			printf( "  Windows hardware acceleration slider is set to off! Turn it on!\n" );
		}

		// Get driver name
		result = FMOD::FMOD_System_GetDriverInfo( m_system, 0, name, 256, 0 );
		// Check for specific type of driver that has issues
		if ( strstr( name, "SigmaTel" ) != NULL )
		{
			/*
			Sigmatel sound devices crackle for some reason if the format is PCM 16bit.
			PCM floating point output seems to solve it.
			*/
			result = FMOD::FMOD_System_SetSoftwareFormat( m_system, 48000, FMOD::FMOD_SOUND_FORMAT_PCMFLOAT, 0,0, FMOD::FMOD_DSP_RESAMPLER_LINEAR );
		}
		// Print out the name of the driver
		printf( "  FMOD started on devicename:'%s'\n", name );
	}

	// Initialize the system
	result = FMOD::FMOD_System_Init( m_system, 100, FMOD_INIT_NORMAL, 0);
	if ( result == FMOD::FMOD_ERR_OUTPUT_CREATEBUFFER )
	{
		/*
		Ok, the speaker mode selected isn't supported by this soundcard. Switch it
		back to stereo...
		*/
		result = FMOD::FMOD_System_SetSpeakerMode( m_system, FMOD::FMOD_SPEAKERMODE_STEREO );
		// ...and reinit
		result = FMOD::FMOD_System_Init( m_system, c_voices, FMOD_INIT_NORMAL, 0 );
	}

	active = true;

	// Now set options if active
	if ( Active() )
	{
		// Set 3D options
		FMOD_System_Set3DNumListeners( CAudioMaster::System(), 1 );
		FMOD::FMOD_System_Set3DSettings( m_system, 1.0f, 3.28f, 1.0f ); // Feet scale
	}

#endif
}

// OpenAL starting
void CAudioMaster::FreeSystem ( void )
{
	if ( !Active() )
		return;

#ifndef _AUDIO_FMOD_
	// Free context and device
	alcDestroyContext( context );
	alcCloseDevice   ( device  );
#else
	FMOD::FMOD_System_Release( m_system );
#endif
}


// Adding and removing objects
void CAudioMaster::AddListener ( CAudioListener* listener )
{
	listeners.push_back( listener );
}
void CAudioMaster::RemoveListener ( CAudioListener* listener )
{
	vector<CAudioListener*>::iterator it;
	it = find( listeners.begin(), listeners.end(), listener );
	if ( it == listeners.end() )
	{
		cout << __FILE__ << "(" << __LINE__ << ") Error destroying listener THAT DOESN'T EXIST (should never happen)" << endl;
	}
	else
	{
		listeners.erase( it );
	}
}
unsigned int CAudioMaster::AddSource ( CAudioSource* source )
{
	sources.push_back( source );
	return next_sound_id++;
}
void CAudioMaster::RemoveSource ( CAudioSource* source )
{
	vector<CAudioSource*>::iterator it;
	it = find( sources.begin(), sources.end(), source );
	if ( it == sources.end() )
	{
		cout << __FILE__ << "(" << __LINE__ << ") Error removing non-existant source from list (should never happen)" << endl;
	}
	else
	{
		sources.erase( it );
	}
}

CAudioMaster* CAudioMaster::GetCurrent ( void )
{
	return pActiveAudioMaster;
}
bool CAudioMaster::Active ( void ) 
{
	return pActiveAudioMaster->active;
}