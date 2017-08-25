

#ifndef _C_AUDIO_SOURCE_H_
#define _C_AUDIO_SOURCE_H_

#include "CAudioMaster.h"
#include "core/math/Vector3d.h"

class CAudioSource
{
public:
	AUDIO_API CAudioSource ( CAudioSound* );
	AUDIO_API ~CAudioSource ( void );
	AUDIO_API void SetChannelProperties ( const int& n_channel );

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
	CAudioSound*	sound;
#ifndef _AUDIO_FMOD_
	ALuint			iSource;
#else
	FMOD::FMOD_CHANNEL*	m_instance;
#endif

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

#endif