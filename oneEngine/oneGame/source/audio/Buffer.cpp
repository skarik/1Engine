#include "Buffer.h"
#include "WaveformLoader.h"

#include "core/debug/console.h"

using std::string;

audio::Buffer::Buffer ( const char* n_filename, const int nPositional )
	: arBaseObject(), m_positional(nPositional>0)
{
	m_streamed = false;
	m_sound = BUFFER_NULL;
	Init( n_filename );
}
audio::Buffer::Buffer ( void )
	: arBaseObject(), m_positional(true)
{
	m_streamed = false;
	m_sound = BUFFER_NULL;
}

audio::Buffer::~Buffer ( void )
{
	Free();
}

void audio::Buffer::Init ( const char* n_filename )
{
	if ( !audio::Master::Active() )
		return;

	// Load in the file from the disk:
	audio::WaveformLoader loader;
	m_sound = loader.LoadFile( n_filename, IsPositional() );
	m_streamed = false;
}

void audio::Buffer::Free ( void )
{
	if ( !audio::Master::Active() )
		return;

	if ( m_sound )
	{
#	ifndef _AUDIO_FMOD_
		alDeleteBuffers( 1, &m_sound );
#	else
		FMOD::FMOD_Sound_Release( m_sound );
#	endif
	}

	m_sound = BUFFER_NULL;
}

double audio::Buffer::GetLength ( void )
{
#ifndef _AUDIO_FMOD_
	// First need length in samples
	ALint sizeInBytes;
	ALint channels;
	ALint bits;
	alGetBufferi(m_sound, AL_SIZE, &sizeInBytes);
	alGetBufferi(m_sound, AL_CHANNELS, &channels);
	alGetBufferi(m_sound, AL_BITS, &bits);
	double lengthInSamples = sizeInBytes * 8 / (channels * bits);
	// Then with length in samples, can get length in seconds
	ALint frequency;
	alGetBufferi(m_sound, AL_FREQUENCY, &frequency);
	double durationInSeconds = (double)lengthInSamples / (double)frequency;
	// Return length in seconds
	return durationInSeconds;
#else
	unsigned int ms;
	FMOD::FMOD_Sound_GetLength( m_sound, &ms, FMOD_TIMEUNIT_MS );
	return (ms/1000.0f);
#endif
}