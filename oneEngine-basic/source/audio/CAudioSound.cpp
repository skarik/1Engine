
#include "CAudioSound.h"
#include "CAudioSoundLoader.h"

#include "core/debug/console.h"

using std::string;

CAudioSound::CAudioSound ( const string& sFileName, const int nPositional )
	: referenceCount(0), positional(nPositional>0)
{
	streamed = false;
#ifndef _AUDIO_FMOD_
	buffer = 0;
#else
	m_sound = NULL;
#endif
	Init( sFileName );
}
CAudioSound::CAudioSound ( void )
	: referenceCount(0), positional(true)
{
	streamed = false;
#ifndef _AUDIO_FMOD_
	buffer = 0;
#else
	m_sound = NULL;
#endif
}

CAudioSound::~CAudioSound ( void )
{
	Free();
}

void CAudioSound::Init ( const string& sFileName )
{
	if ( !CAudioMaster::Active() )
		return;

#ifndef _AUDIO_FMOD_
	CAudioSoundLoader loader;

	streamed = false;
	buffer = loader.LoadFile( sFileName );
#else
	FMOD::FMOD_RESULT result = FMOD::FMOD_OK;

	if ( positional ) {
		result = FMOD::FMOD_System_CreateSound( CAudioMaster::System(), sFileName.c_str(), FMOD_DEFAULT | FMOD_3D, 0, &m_sound );
		//FMOD::FMOD_Sound_SetMode( m_sound, FMOD_3D_WORLDRELATIVE );
	}
	else {
		result = FMOD::FMOD_System_CreateSound( CAudioMaster::System(), sFileName.c_str(), FMOD_DEFAULT, 0, &m_sound );
	}

	// Check to see if it loaded properly
	if ( result != FMOD::FMOD_OK )
	{
		Debug::Console->PrintError("FMOD could not open the file \"" + sFileName + "\"");
	}

	streamed = false;
#endif
}

void CAudioSound::Free ( void )
{
	if ( !CAudioMaster::Active() )
		return;

#ifndef _AUDIO_FMOD_
	if ( buffer )
		alDeleteBuffers( 1, &buffer );
#else
	FMOD::FMOD_Sound_Release( m_sound );
#endif
}

double CAudioSound::GetLength ( void )
{
#ifndef _AUDIO_FMOD_
	// First need length in samples
	ALint sizeInBytes;
	ALint channels;
	ALint bits;
	alGetBufferi(buffer, AL_SIZE, &sizeInBytes);
	alGetBufferi(buffer, AL_CHANNELS, &channels);
	alGetBufferi(buffer, AL_BITS, &bits);
	double lengthInSamples = sizeInBytes * 8 / (channels * bits);
	// Then with length in samples, can get length in seconds
	ALint frequency;
	alGetBufferi(buffer, AL_FREQUENCY, &frequency);
	double durationInSeconds = (double)lengthInSamples / (double)frequency;
	// Return length in seconds
	return durationInSeconds;
#else
	unsigned int ms;
	FMOD::FMOD_Sound_GetLength( m_sound, &ms, FMOD_TIMEUNIT_MS );
	return (ms/1000.0f);
#endif
}