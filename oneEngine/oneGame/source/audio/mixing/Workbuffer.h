#ifndef AUDIO_MIXING_WORKBUFFER_H_
#define AUDIO_MIXING_WORKBUFFER_H_

#include "audio/mixing/Channels.h"

namespace audio
{
	template <int Length, int Channels>
	class Workbuffer
	{
		float				m_data [Length * Channels];
		MixChannel			m_tag = MixChannel::kDefault;
	};
}

#endif//AUDIO_MIXING_WORKBUFFER_H_
