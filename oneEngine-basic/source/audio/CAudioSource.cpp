
#include "CAudioSource.h"
#include "CAudioSound.h"
#include "CAudioSoundStreamed.h"

#include "AudioStructs.h"

CAudioSource::CAudioSource( CAudioSound * pInSound )
	:  sound(NULL),
#ifndef _AUDIO_FMOD_
	iSource(0)
#else
	m_instance(NULL)
#endif
{
	queue_destruction = false;

	// Set initial options
	options.pitch	= 1;
	options.gain	= 1;
	options.looped	= false;
	options.rolloff = 1;
	position = Vector3d();
	velocity = Vector3d();
	// Set initial state
	playbacktime = 0;
	targets_playing = false;
	initial_setup = false;

	// Set sound
	sound = pInSound;
	if ( sound == NULL ) {
		std::cout << "Warning: source with no sound created!" << std::endl;
		return;
	}
	// Create the source and perform initialization
	sound->AddReference();
	sound_id = CAudioMaster::GetCurrent()->AddSource( this );
	if ( CAudioMaster::Active() )
	{
#ifndef _AUDIO_FMOD_
		alGenSources( 1, &iSource );
		if ( !iSource ) {
			sound->RemoveReference();
			sound = NULL;
			std::cout << "Warning: source not created successfully!" << std::endl;
			return;
		}
		if ( !sound->IsStreamed() ) {
			alSourcei( iSource, AL_BUFFER, sound->GetBuffer() );
		}
		else {
			CAudioSoundStreamed* ssound = (CAudioSoundStreamed*)sound;
			ALuint buffer0, buffer1;
			// Get the open buffers
			for ( int i = 0; i < 8; ++i ) {
				if ( !ssound->buffer_usage[i] ) {
					ssound->buffer_usage[i] = true;
					buffer0 = ssound->buffers[i];
					break;
				}
			}
			for ( int i = 0; i < 8; ++i ) {
				if ( !ssound->buffer_usage[i] ) {
					ssound->buffer_usage[i] = true;
					buffer1 = ssound->buffers[i];
					break;
				}
			}
			// Load in inital streaming data
			ssound->Stream( buffer0, playbacktime );
			ssound->Stream( buffer1, playbacktime );
			// Queue buffers to source
			alSourceQueueBuffers( iSource, 1, &buffer0 );
			alSourceQueueBuffers( iSource, 1, &buffer1 );
			// Set that did initial setup
			initial_setup = true;
		}
#else
		FMOD::FMOD_System_PlaySound( CAudioMaster::System(), FMOD::FMOD_CHANNEL_FREE, sound->Handle(), true, &m_instance );
		if ( m_instance ) {
			initial_setup = true;
		}
		FMOD::FMOD_Channel_SetLoopCount( m_instance, 0 ); 

		FMOD::FMOD_Channel_Set3DPanLevel( m_instance, 1 ); 
		//FMOD::FMOD_Channel_Set3DPanLevel( m_instance, 1 ); 
		// Channels are groups.
#endif
	}

	// Perform initial options update
	Update();
}

CAudioSource::~CAudioSource ( void )
{
	if ( sound ) {
		sound->RemoveReference();
	}
	else {
		return;
	}
	CAudioMaster::GetCurrent()->RemoveSource( this );
	if ( CAudioMaster::Active() )
	{
#ifndef _AUDIO_FMOD_
		if ( iSource ) {
			alDeleteSources( 1, &iSource );
		}
#else
		FMOD::FMOD_Channel_Stop( m_instance );
#endif
	}
}

void CAudioSource::SetChannelProperties ( const int& n_channel )
{
	AudioStructs::soundScript_channel channel = (AudioStructs::soundScript_channel)n_channel;
#ifndef _AUDIO_FMOD_

#else
	switch ( channel )
	{
	case AudioStructs::CHAN_SPEECH:
		// Set min distance so that max gain isn't too high when getting closer. (and isn't too low at close distances)
		// Set max distance so that the attenuation stops soon enough.
		FMOD::FMOD_Channel_Set3DMinMaxDistance( m_instance, 5.0f, 500.0f );
		// Reduce the gain
		options.gain *= 0.4f;
		break;
	case AudioStructs::CHAN_DEFAULT:
		FMOD::FMOD_Channel_Set3DMinMaxDistance( m_instance, 3.0f, 5000.0f );
		break;
	}
#endif
}

void CAudioSource::Update ( void )
{
	if ( !sound ) {
		return;
	}

#ifndef _AUDIO_FMOD_
	if ( sound->IsStreamed() )
	{
		// Check for an invalid stop
		if ( !IsPlaying() ) {
			if ( targets_playing ) {
				alSourcePlay( iSource );
			}
		}
		// Otherwise update the sound playing
		if ( targets_playing )
		{
			// Update the buffer
			CAudioSoundStreamed* ssound = (CAudioSoundStreamed*)sound;
			bool sound_valid = ssound->Sample( iSource, playbacktime, options.looped );
			if ( !sound_valid ) {
				alSourceStop( iSource );
				targets_playing = false;
			}

			// Update the source
			alSourcef( iSource, AL_PITCH, options.pitch);
			alSourcef( iSource, AL_GAIN, options.gain);
			alSource3f(iSource, AL_POSITION, position.x, position.y, position.z);
			alSource3f(iSource, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
			alSourcei( iSource, AL_LOOPING, false );
			alSourcef( iSource, AL_ROLLOFF_FACTOR, options.rolloff );
		}
	}
	else
	{
		if ( IsPlaying() ) {
			// Get playback position
			ALfloat sample_time;
			alGetSourcef( iSource, AL_SEC_OFFSET, &sample_time );
			playbacktime = sample_time;
		}
		else {
			playbacktime = GetSoundLength();
		}

		// Update the source
		alSourcef( iSource, AL_PITCH, options.pitch);
		alSourcef( iSource, AL_GAIN, options.gain);
		alSource3f(iSource, AL_POSITION, position.x, position.y, position.z);
		alSource3f(iSource, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
		alSourcei( iSource, AL_LOOPING, options.looped);
		alSourcef( iSource, AL_ROLLOFF_FACTOR, options.rolloff );
	}
#else
	// Update the instance options
	FMOD::FMOD_Channel_SetFrequency( m_instance, 44100 * options.pitch );
	FMOD::FMOD_Channel_SetVolume( m_instance, options.gain );
	if ( sound->IsStreamed() ) {
		if ( options.looped ) {
			FMOD::FMOD_Channel_SetLoopCount( m_instance, -1 ); 
		}
		else {
			FMOD::FMOD_Channel_SetLoopCount( m_instance, 0 ); 
		}
	}
	if ( sound->IsPositional() ) {
		FMOD::FMOD_VECTOR t_position = { position.x, position.y, position.z };
		FMOD::FMOD_VECTOR t_velocity = { velocity.x, velocity.y, velocity.z };
		FMOD::FMOD_Channel_Set3DAttributes( m_instance, &t_position, &t_velocity );
	}
#endif
}

void CAudioSource::Play ( bool reset )
{
	if ( !sound ) {
		return;
	}

#ifndef _AUDIO_FMOD_
	if ( reset )
	{
		targets_playing = true;
		if ( !sound->IsStreamed() ) {
			alSourcePlay( iSource );
		}
		else {
			if ( !IsPlaying() ) {
				if ( !initial_setup ) {
					playbacktime = 0;
				}
				alSourcePlay( iSource );
			}
		}
		initial_setup = false;
	}
	else
	{
		targets_playing = true;
		alSourcePlay( iSource );
		initial_setup = false;
	}
#else
	FMOD::FMOD_Channel_SetPaused( m_instance, false );
#endif
}
void CAudioSource::Pause ( void )
{
#ifndef _AUDIO_FMOD_
	targets_playing = false;
	alSourcePause( iSource );
#else
	FMOD::FMOD_Channel_SetPaused( m_instance, true );
#endif
}
void CAudioSource::Stop ( void )
{
#ifndef _AUDIO_FMOD_
	targets_playing = false;
	alSourceStop( iSource );
#else
	FMOD::FMOD_Channel_SetPaused( m_instance, true );
	FMOD::FMOD_Channel_SetPosition( m_instance, 0, FMOD_TIMEUNIT_MS );
#endif
}

void CAudioSource::Destroy ( void )
{
	queue_destruction = true;
}

bool CAudioSource::IsPlaying ( void )
{
#ifndef _AUDIO_FMOD_
	ALint state;
	alGetSourcei( iSource, AL_SOURCE_STATE, &state );
	return state == AL_PLAYING;
#else
	int isPlaying;
	FMOD::FMOD_Channel_IsPlaying( m_instance, &isPlaying );
	if ( isPlaying ) {
		FMOD::FMOD_Channel_GetPaused( m_instance, &isPlaying );
		isPlaying = !isPlaying;
	}
	return isPlaying!=0;
#endif
}
bool CAudioSource::Played ( void )
{
#ifndef _AUDIO_FMOD_
	ALint state;
	alGetSourcei( iSource, AL_SOURCE_STATE, &state );
	return (state == AL_STOPPED)&&(playbacktime>=sound->GetLength());
#else
	int isPlaying;
	FMOD::FMOD_Channel_IsPlaying( m_instance, &isPlaying );
	return !isPlaying;
#endif
}

void CAudioSource::SetPlaybackTime ( double target_time )
{
#ifndef _AUDIO_FMOD_
	if ( !sound->IsStreamed() ) {
		alSourcef( iSource, AL_SEC_OFFSET, (ALfloat)target_time );
		playbacktime = target_time;
	}
	else {
		playbacktime = target_time;
	}
#else
	FMOD::FMOD_Channel_SetPosition( m_instance, (unsigned int)(target_time*1000), FMOD_TIMEUNIT_MS );
#endif
}
double CAudioSource::GetPlaybackTime ( void )
{
#ifndef _AUDIO_FMOD_
	return playbacktime;
#else
	unsigned int result_time;
	FMOD::FMOD_Channel_GetPosition( m_instance, &result_time, FMOD_TIMEUNIT_MS );
	return result_time/1000.0;
#endif
}
double CAudioSource::GetSoundLength ( void )
{
	return sound->GetLength();
}

double CAudioSource::GetCurrentMagnitude ( void )
{
	float wavearray[8];
	FMOD::FMOD_Channel_GetWaveData( m_instance, wavearray, 8, 0 );
	double result = 0;
	for ( int i = 0; i < 8; ++i ) {
		result += fabs(wavearray[i]);
	}
	return result/8.0;
}