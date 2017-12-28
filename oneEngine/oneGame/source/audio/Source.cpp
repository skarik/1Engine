#include "audio/Source.h"
#include "audio/Buffer.h"
#include "audio/BufferStreamed.h"
#include "audio/types/AudioStructs.h"

audio::Source::Source( Buffer* pInSound )
	:  sound(NULL), instance(HANDLE_NULL)
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
	sound_id = audio::Master::GetCurrent()->AddSource( this );
	if ( audio::Master::Active() )
	{
#ifndef _AUDIO_FMOD_
		alGenSources( 1, &instance );
		if ( !instance )
		{
			sound->RemoveReference();
			sound = NULL;
			std::cout << "Warning: source not created successfully!" << std::endl;
			return;
		}
		if ( !sound->IsStreamed() )
		{
			alSourcei( instance, AL_BUFFER, sound->GetBuffer() );
		}
		else
		{
			audio::BufferStreamed* ssound = (audio::BufferStreamed*)sound;
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
			alSourceQueueBuffers( instance, 1, &buffer0 );
			alSourceQueueBuffers( instance, 1, &buffer1 );
			// Set that did initial setup
			initial_setup = true;
		}
#else
		FMOD::FMOD_System_PlaySound( CAudioMaster::System(), FMOD::FMOD_CHANNEL_FREE, sound->Handle(), true, &m_instance );
		if ( m_instance ) {
			initial_setup = true;
		}
		FMOD::FMOD_Channel_SetLoopCount( instance, 0 ); 

		FMOD::FMOD_Channel_Set3DPanLevel( instance, 1 ); 
		//FMOD::FMOD_Channel_Set3DPanLevel( m_instance, 1 ); 
		// Channels are groups.
#endif
	}

	// Perform initial options update
	Update();
}

audio::Source::~Source ( void )
{
	if ( sound ) {
		sound->RemoveReference();
	}
	else {
		return;
	}
	audio::Master::GetCurrent()->RemoveSource( this );
	if ( audio::Master::Active() )
	{
#ifndef _AUDIO_FMOD_
		if ( instance ) {
			alDeleteSources( 1, &instance );
		}
#else
		FMOD::FMOD_Channel_Stop( m_instance );
#endif
	}
}

void audio::Source::SetChannelProperties ( const audio::eSoundScriptChannel n_channel )
{
	audio::eSoundScriptChannel channel = (audio::eSoundScriptChannel)n_channel;
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
	default:
		// 4.5 is the normal rolloff amount
		float rolloff_amount = 4.5F / options.rolloff;
		// Change the default distance based on actual rolloff
		FMOD::FMOD_Channel_Set3DMinMaxDistance( m_instance, 3.0f * rolloff_amount, 5000.0f * rolloff_amount );
		break;
	}
#endif
}

void audio::Source::Update ( void )
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
				alSourcePlay( instance );
			}
		}
		// Otherwise update the sound playing
		if ( targets_playing )
		{
			// Update the buffer
			BufferStreamed* ssound = (BufferStreamed*)sound;
			bool sound_valid = ssound->Sample( instance, playbacktime, options.looped );
			if ( !sound_valid ) {
				alSourceStop( instance );
				targets_playing = false;
			}

			// Update the source
			alSourcef( instance, AL_PITCH, options.pitch);
			alSourcef( instance, AL_GAIN, options.gain);
			alSource3f(instance, AL_POSITION, position.x, position.y, position.z);
			alSource3f(instance, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
			alSourcei( instance, AL_LOOPING, false );
			alSourcef( instance, AL_ROLLOFF_FACTOR, options.rolloff );
		}
	}
	else
	{
		if ( IsPlaying() ) {
			// Get playback position
			ALfloat sample_time;
			alGetSourcef( instance, AL_SEC_OFFSET, &sample_time );
			playbacktime = sample_time;
		}
		else {
			playbacktime = GetSoundLength();
		}

		// Update the source
		alSourcef( instance, AL_PITCH, options.pitch);
		alSourcef( instance, AL_GAIN, options.gain);
		alSource3f(instance, AL_POSITION, position.x, position.y, position.z);
		alSource3f(instance, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
		alSourcei( instance, AL_LOOPING, options.looped);
		alSourcef( instance, AL_ROLLOFF_FACTOR, options.rolloff );
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

void audio::Source::Play ( bool reset )
{
	if ( !sound ) {
		return;
	}

#ifndef _AUDIO_FMOD_
	if ( reset )
	{
		targets_playing = true;
		if ( !sound->IsStreamed() ) {
			alSourcePlay( instance );
		}
		else {
			if ( !IsPlaying() ) {
				if ( !initial_setup ) {
					playbacktime = 0;
				}
				alSourcePlay( instance );
			}
		}
		initial_setup = false;
	}
	else
	{
		targets_playing = true;
		alSourcePlay( instance );
		initial_setup = false;
	}
#else
	FMOD::FMOD_Channel_SetPaused( m_instance, false );
#endif
}
void audio::Source::Pause ( void )
{
#ifndef _AUDIO_FMOD_
	targets_playing = false;
	alSourcePause( instance );
#else
	FMOD::FMOD_Channel_SetPaused( m_instance, true );
#endif
}
void audio::Source::Stop ( void )
{
#ifndef _AUDIO_FMOD_
	targets_playing = false;
	alSourceStop( instance );
#else
	FMOD::FMOD_Channel_SetPaused( m_instance, true );
	FMOD::FMOD_Channel_SetPosition( m_instance, 0, FMOD_TIMEUNIT_MS );
#endif
}

void audio::Source::Destroy ( void )
{
	queue_destruction = true;
}

bool audio::Source::IsPlaying ( void )
{
#ifndef _AUDIO_FMOD_
	ALint state;
	alGetSourcei( instance, AL_SOURCE_STATE, &state );
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
bool audio::Source::Played ( void )
{
#ifndef _AUDIO_FMOD_
	ALint state;
	alGetSourcei( instance, AL_SOURCE_STATE, &state );
	return (state == AL_STOPPED)&&(playbacktime>=sound->GetLength());
#else
	int isPlaying;
	FMOD::FMOD_Channel_IsPlaying( m_instance, &isPlaying );
	return !isPlaying;
#endif
}

void audio::Source::SetPlaybackTime ( double target_time )
{
#ifndef _AUDIO_FMOD_
	if ( !sound->IsStreamed() ) {
		alSourcef( instance, AL_SEC_OFFSET, (ALfloat)target_time );
		playbacktime = target_time;
	}
	else {
		playbacktime = target_time;
	}
#else
	FMOD::FMOD_Channel_SetPosition( m_instance, (unsigned int)(target_time*1000), FMOD_TIMEUNIT_MS );
#endif
}
double audio::Source::GetPlaybackTime ( void )
{
#ifndef _AUDIO_FMOD_
	return playbacktime;
#else
	unsigned int result_time;
	FMOD::FMOD_Channel_GetPosition( m_instance, &result_time, FMOD_TIMEUNIT_MS );
	return result_time/1000.0;
#endif
}
double audio::Source::GetSoundLength ( void )
{
	return sound->GetLength();
}

double audio::Source::GetCurrentMagnitude ( void )
{
#ifndef _AUDIO_FMOD_
	return 0.0;
#else
	float wavearray[8];
	FMOD::FMOD_Channel_GetWaveData( instance, wavearray, 8, 0 );
	double result = 0;
	for ( int i = 0; i < 8; ++i ) {
		result += fabs(wavearray[i]);
	}
	return result/8.0;
#endif
}