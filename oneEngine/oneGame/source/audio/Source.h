#ifndef AUDIO_SOURCE_H_
#define AUDIO_SOURCE_H_

#include "core/math/Vector3d.h"
#include "audio/AudioMaster.h"
#include "audio/types/handles.h"
#include "audio/types/AudioStructs.h"

namespace audio
{
	class Buffer;

	class Source
	{
	public:
		AUDIO_API explicit	Source ( Buffer* );
		AUDIO_API			~Source ( void );

		AUDIO_API void SetChannelProperties ( const audio::eSoundScriptChannel n_channel );

		AUDIO_API void Update ( void );

		AUDIO_API void Play ( bool reset = false );
		AUDIO_API void Pause ( void );
		AUDIO_API void Stop ( void );

		AUDIO_API void Destroy ( void );

		AUDIO_API bool IsPlaying ( void );
		AUDIO_API bool Played ( void );

		AUDIO_API void SetPlaybackTime ( double );
		AUDIO_API double GetPlaybackTime ( void );
		AUDIO_API double GetSoundLength ( void );

		AUDIO_API double GetCurrentMagnitude ( void );

		AUDIO_API unsigned int GetID ( void ) {
			return sound_id;
		}
	protected:
		Buffer*			sound;
		arSourceHandle	instance;

	public:
		Vector3d position;
		Vector3d velocity;

		struct sPlaybackOptions
		{
			bool looped;
			float pitch;
			float gain;
			//float max_dist;
			float rolloff;
		} options;

		bool queue_destruction;

		bool initial_setup;
		bool targets_playing;
		double playbacktime;

		unsigned int sound_id;
	};
}

#endif//AUDIO_SOURCE_H_