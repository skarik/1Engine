#ifndef AUDIO_TYPES_BUFFER_DATA_H_
#define AUDIO_TYPES_BUFFER_DATA_H_

#include "core/types.h"
#include "audio/types/Formats.h"

namespace audio
{
	struct arBufferData
	{
		float*			data;
		uint32_t		frames;
		ChannelCount	channels;
		SampleRate		sampleRate;
		Format			format;
	};
}

#endif//AUDIO_TYPES_BUFFER_DATA_H_