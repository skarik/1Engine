#ifndef AUDIO_MIXING_WORKBUFFER_H_
#define AUDIO_MIXING_WORKBUFFER_H_

#include "audio/mixing/Channels.h"

namespace audio
{
	template <int Length>
	struct WorkbufferStereo
	{
		union
		{
			struct
			{
				float				m_data [Length * 2];
			};
			struct
			{
				float				m_data_left [Length];
				float				m_data_right [Length];
			};
		};

		constexpr const int GetLength ( void ) const
			{ return Length; }
	};
}

#endif//AUDIO_MIXING_WORKBUFFER_H_
