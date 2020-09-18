#include "audio/Source.h"
#include "audio/Buffer.h"
#include "audio/BufferStreamed.h"
#include "audio/types/BufferData.h"

#include "audio/Manager.h"

audio::Source::Source( Buffer* inSound )
	:  sound(NULL), instance(NIL)
{
	// Set sound
	sound = inSound;
	if ( sound == NULL )
	{
		std::cout << "Warning: source with no sound created!" << std::endl;
		return;
	}

	auto auc = getValidManager();

	// Create the source and perform initialization
	sound->AddReference();
	sound_id = auc->AddSource( this );
	if ( auc->IsActive() )
	{
		; // ??? why is this needed anymore
	}

	// Perform initial options update
	GameTick(0.0F);
}

audio::Source::~Source ( void )
{
	auto auc = getValidManager();

	if ( sound )
	{
		sound->RemoveReference();
	}

	/*auc->RemoveSource( this );
	if ( auc->IsActive() )
	{
		// ???
	}*/
}

//void audio::Source::SetChannelProperties ( const audio::eSoundScriptChannel n_channel )
//{
//	audio::eSoundScriptChannel channel = (audio::eSoundScriptChannel)n_channel;
//#ifndef _AUDIO_FMOD_
//
//#else
//	switch ( channel )
//	{
//	case AudioStructs::CHAN_SPEECH:
//		// Set min distance so that max gain isn't too high when getting closer. (and isn't too low at close distances)
//		// Set max distance so that the attenuation stops soon enough.
//		FMOD::FMOD_Channel_Set3DMinMaxDistance( instance, 5.0f, 500.0f );
//		// Reduce the gain
//		options.gain *= 0.4f;
//		break;
//	case AudioStructs::CHAN_DEFAULT:
//	default:
//		// 4.5 is the normal rolloff amount
//		float rolloff_amount = 4.5F / options.rolloff;
//		// Change the default distance based on actual rolloff
//		FMOD::FMOD_Channel_Set3DMinMaxDistance( instance, 3.0f * rolloff_amount, 5000.0f * rolloff_amount );
//		break;
//	}
//#endif
//}

void audio::Source::GameTick ( const float delta_time )
{
	if ( !sound ) {
		return;
	}

	{	// Lock and push the mixer state
		std::lock_guard<std::mutex> lock(mixer_state_lock);
		mixer_state = state;
	}
}

void audio::Source::MixerSampleAndAdvance ( const uint32_t delta_samples, float* work_buffer )
{
	if ( !sound ) {
		return;
	}

	// Grab the looped from mixer state (only have one bit here, so shouldn't be any race-cond issues)
	bool isLooped = mixer_state.looped;

	{
		// Now we do another scoped lock for the other states that shouldn't change
		std::lock_guard<std::mutex> lock(playing_state_lock);

		//arBufferHandle	sound_data = sound->Data();
		uint channel_count = (uint)sound->GetChannelCount();
		uint32_t actual_samples_processed = delta_samples;

		// Copy into the work buffer
		uint32_t current_sample_io = current_sample;
		if (!isLooped)
		{
			if (current_sample + actual_samples_processed >= sound->GetSampleLength())
			{
				actual_samples_processed = sound->GetSampleLength() - current_sample;
				// Fill the part of the buffer that will be empty with 0's
				std::fill(&work_buffer[actual_samples_processed * channel_count], &work_buffer[delta_samples * channel_count], 0.0F);
			}
			sound->Sample(current_sample_io, actual_samples_processed, work_buffer);
		}
		// Copy into the work buffer but with looping
		else
		{
			if (current_sample + actual_samples_processed >= sound->GetSampleLength())
			{
				// Copy the end of the buffer
				sound->Sample(current_sample_io, sound->GetSampleLength() - current_sample, work_buffer);
				// Copy the remainder at the start
				current_sample_io = 0;
				uint32_t copy_offset = sound->GetSampleLength() - current_sample;
				sound->Sample(current_sample_io, actual_samples_processed - copy_offset, &work_buffer[copy_offset * channel_count]);
			}
			else
			{
				sound->Sample(current_sample_io, actual_samples_processed, work_buffer);
			}
		}

		// Advance at the end. Will always be delta_samples unless we're a non-looped audio file.
		if (sound->IsStreamed())
		{	
			// If it's a streamed file, we want the stream to advance the cursor, not the system.
			current_sample = current_sample_io;
			actual_samples_processed = 0;
		}
		if (!isLooped)
		{
			current_sample += actual_samples_processed;
			// If we're at the end of the audio, mark as no longer playing
			if (current_sample >= sound->GetSampleLength())
			{
				is_playing = false;
			}
		}
		else
		{
			// Loop the sample cursor around when looping (obvs)
			current_sample = (current_sample + actual_samples_processed) % sound->GetSampleLength();
		}
	}

	// Now that we sampled it, we don't need to be locking anymore...
}

//	MixerGetSourceState : Get the current source state for the audio
void audio::Source::MixerGetSourceState ( SourceState& output_state )
{
	std::lock_guard<std::mutex> lock(mixer_state_lock);
	output_state = mixer_state;
}

void audio::Source::Play ( bool reset )
{
	if ( !sound ) {
		return;
	}

	if (!is_playing)
	{
		if ( reset )
		{
			std::lock_guard<std::mutex> lock(playing_state_lock);
			current_sample = 0;
		}

		is_playing = true;
	}
}

void audio::Source::Pause ( void )
{
	is_playing = false;
}

void audio::Source::Stop ( void )
{
	is_playing = false;
	std::lock_guard<std::mutex> lock(playing_state_lock);
	current_sample = 0;
}

void audio::Source::Destroy ( void )
{
	queue_destruction = true;
}

bool audio::Source::IsPlaying ( void ) const
{
	return is_playing;
}
bool audio::Source::Played ( void )
{
	return !is_playing && (current_sample >= sound->GetSampleLength());
}

void audio::Source::SetPlaybackTime ( double target_time )
{
	std::lock_guard<std::mutex> lock(playing_state_lock);
	current_sample = (uint32_t)(sound->GetSampleLength() * (target_time / sound->GetLength()));
}
double audio::Source::GetPlaybackTime ( void ) const
{
	auto auc = getValidManager();
	return current_sample / (double)auc->GetPreferredSampleRate();
}
double audio::Source::GetSoundLength ( void ) const
{
	return sound->GetLength();
}

double audio::Source::GetCurrentMagnitude ( void )
{
//	float wavearray[8];
//	FMOD::FMOD_Channel_GetWaveData( instance, wavearray, 8, 0 );
//	double result = 0;
//	for ( int i = 0; i < 8; ++i ) {
//		result += fabs(wavearray[i]);
//	}
//	return result/8.0;
	return 0.0; // TODO. Could store 8 pieces of the last PCM processed...
}

// Playing sound information
const audio::Buffer* audio::Source::GetBuffer ( void ) const
{
	return sound;
}