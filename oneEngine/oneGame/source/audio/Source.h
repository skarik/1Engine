#ifndef AUDIO_SOURCE_H_
#define AUDIO_SOURCE_H_

#include "core/math/Vector3.h"
#include "audio/types/Handles.h"
#include "audio/mixing/Channels.h"
#include <mutex>
#include <atomic>

namespace audio
{
	class Manager;
	class Buffer;

	enum class Falloff
	{
		// 1.0 - (x / L)
		kLinear,
		// (1.0 - (x / L)) ^ factor
		kPower,
		// 1 / (1.0 + x / L * 4 * factor)
		kInverse,
		// 0.5 ^ (x / L * 4 * factor)
		kExponential,
	};

	struct SourceState
	{
		Vector3f			position = Vector3f(0, 0, 0);
		Vector3f			velocity = Vector3f(0, 0, 0);

		bool				looped = false;
		float				pitch = 1.0F;
		float				gain = 1.0F;
		MixChannel			channel = MixChannel::kDefault;

		// 0.0 for 3D bypass, direct output. 1.0 for full 3D mixing.
		// Blends in and out by controlling the % of left-right delay and panning.
		float				spatial = 1.0F;

		float				min_dist = 20.0F;
		float				max_dist = 300.0F;
		float				falloff = 1.0F;
		Falloff				falloffStyle = Falloff::kPower;
	};

	class Source
	{
	private:
		friend Manager;
		AUDIO_API				~Source ( void ); // Cannot be deleted by outsiders

	public:
		AUDIO_API explicit		Source ( Buffer* inSound );

		//AUDIO_API void			SetChannelProperties ( const audio::eSoundScriptChannel n_channel );

		//	GameTick( delta_time ) : Push current state to the mixer
		void					GameTick ( const float delta_time );
		//	MixerSampleAndAdvance( samples, buffer ) : Sample Advance playback cursor by given time
		void					MixerSampleAndAdvance ( const uint32_t delta_samples, float* work_buffer );
		//	MixerGetSourceState( out_state ) : Get the current source state for the audio
		void					MixerGetSourceState ( SourceState& output_state );

		AUDIO_API void			Play ( bool reset = false );
		AUDIO_API void			Pause ( void );
		AUDIO_API void			Stop ( void );

		AUDIO_API void			Destroy ( void );

		AUDIO_API bool			IsPlaying ( void ) const;
		AUDIO_API bool			Played ( void );

		AUDIO_API void			SetPlaybackTime ( double target_time );
		AUDIO_API double		GetPlaybackTime ( void ) const;
		AUDIO_API double		GetSoundLength ( void ) const;

		// Playing sound information
		AUDIO_API const Buffer*	GetBuffer ( void ) const;

		// Current playing state information
		AUDIO_API double		GetCurrentMagnitude ( void );

		AUDIO_API unsigned int	GetID ( void )
			{ return sound_id; }
	protected:
		Buffer*				sound = NULL;
		arSourceHandle		instance = NIL;

	public:
		SourceState			state;
		bool				queue_destruction = false;

	private:
		SourceState			mixer_state;
		// Mutex for mixer read state & game write state
		std::mutex			mixer_state_lock;
		
		// Mutex for safer interactions between Mixer and Play/Pause/Stop
		std::mutex			playing_state_lock;
		// Is this source playing
		std::atomic_bool	is_playing;
		// The current sample we're to play back next
		uint32_t			current_sample = 0;
		//bool				initial_setup = false;
		//bool				targets_playing = false;

		unsigned int		sound_id;
	};
}

#endif//AUDIO_SOURCE_H_