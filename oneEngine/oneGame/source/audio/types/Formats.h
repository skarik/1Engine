#ifndef AUDIO_TYPES_FORMATS_H_
#define AUDIO_TYPES_FORMATS_H_

#include "core/types.h"

namespace audio
{
	enum class Format
	{
		kInvalid = 0,

		// This is the only valid format supported. All data is resampled to this format.
		// For future off-CPU audio processing, this is the most compatible format.
		kFloat32,

		// These are used only during loading and should not be used at other times
		kSignedInteger16,
		kSignedInteger8,
	};

	enum class ChannelCount : uint32_t
	{
		kInvalid = 0,

		kMono = 1,
		kStereo = 2,
	};

	typedef uint32_t SampleRate;
}

#endif//AUDIO_TYPES_FORMATS_H_