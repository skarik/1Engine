#ifndef AUDIO_MIXING_CHANNELS_H_
#define AUDIO_MIXING_CHANNELS_H_

#include "core/types.h"

namespace audio
{
	enum class MixChannel : uint8_t
	{
		kDefault = 0,
		kPhysics,
		kHeavy,
		kSpeech,
		kBackground,
		kMusic,

		kMAX_COUNT,
		kNoChannel = kMAX_COUNT
	};
}

#endif//AUDIO_MIXING_CHANNELS_H_