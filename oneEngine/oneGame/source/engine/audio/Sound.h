//===============================================================================================//
//
//		class engine::Sound
//
// Wraps the audio namespace sources.
// Should be created through engine::Audio rather than created manually.
// 
// Keeps track of itself with an internal list that can be accessed for read. This list can be
// used to manage AI and other sound-based effects.
//
//===============================================================================================//
#ifndef ENGINE_AUDIO_SOUND_H_
#define ENGINE_AUDIO_SOUND_H_

#include "core/math/Vector3.h"
#include "engine/behavior/CGameBehavior.h"
#include "audio/types/AudioStructs.h"
#include <vector>

namespace audio
{
	class Buffer;
	class Source;
}

namespace engine
{
	class Sound : public CGameBehavior
	{

	public:
		ENGINE_API Sound ( void );
		ENGINE_API ~Sound ( void );

		ENGINE_API void Update ( void );

		ENGINE_API void Play ( void );
		ENGINE_API void Stop ( void );
		ENGINE_API void SetLooped ( bool );

		ENGINE_API void SetGain ( float );
		ENGINE_API void SetPitch ( float );

		ENGINE_API const static std::vector<Sound*>& GetSoundList ( void );

	public:
		audio::Source*	mySource;
		bool			deleteWhenDone;
		Vector3f		position;
		Vector3f		velocity;

		audio::eSoundScriptAIAlert	ai_alert_amount;
		audio::eSoundScriptChannel	channel;

		Vector3f*		source_position;

	private:
		static std::vector<Sound*>	soundList;
	};
}

#endif//ENGINE_AUDIO_SOUND_H_