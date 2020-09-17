#ifndef AUDIO_MIXING_OPERATIONS_H_
#define AUDIO_MIXING_OPERATIONS_H_

#include "core/types.h"

namespace audio
{
	namespace mixing
	{
		//	InterleavedStereoToChannels(input, result_left, result_right) : Splits an interleaved stereo buffer (frames in sequence) to two separate buffers.
		template <uint32_t Length>
		void InterleavedStereoToChannels ( const float* buffer_interleaved, float* out_buffer_left, float* out_buffer_right )
		{
			for (uint32_t i = 0; i < Length; ++i)
			{
				out_buffer_left[i] = buffer_interleaved[i * 2 + 0];
				out_buffer_right[i] = buffer_interleaved[i * 2 + 1];
			}
		}

		//	ChannelsToInterleavedStereo(input_left, input_right, result) : Combines two separate channels into interleaved stereo.
		template <uint32_t Length>
		void ChannelsToInterleavedStereo ( const float* buffer_left, const float* buffer_right, float* out_buffer_interleaved )
		{
			for (uint32_t i = 0; i < Length; ++i)
			{
				out_buffer_interleaved[i * 2 + 0] = buffer_left[i];
				out_buffer_interleaved[i * 2 + 1] = buffer_right[i];
			}
		}

		//	Acculmulate(input, input_output) : Combines the left buffer into the right buffer.
		template <uint32_t Length>
		void Acculmulate ( const float* buffer, float* inout_buffer_acculmulator )
		{
			for (uint32_t i = 0; i < Length; ++i)
			{
				inout_buffer_acculmulator[i] += buffer[i];
			}
		}

		//	Scale(input_output, scale) : Scales the entire buffer by the given value
		template <uint32_t Length>
		void Scale ( float* inout_buffer, const float scale )
		{
			for (uint32_t i = 0; i < Length; ++i)
			{
				inout_buffer[i] *= scale;
			}
		}
	}
}

#endif//AUDIO_MIXING_OPERATIONS_H_