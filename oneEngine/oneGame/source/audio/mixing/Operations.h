#ifndef AUDIO_MIXING_OPERATIONS_H_
#define AUDIO_MIXING_OPERATIONS_H_

#include "core/types.h"
#include "core/math/Math.h"
#include "muFFT/fft.h"
#include <complex>

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

		//	Saturate(input_output) : Saturates the values in the entire buffer
		template <uint32_t Length>
		void Saturate ( float* inout_buffer )
		{
			for (uint32_t i = 0; i < Length; ++i)
			{
				inout_buffer[i] = math::clamp(inout_buffer[i], -1.0F, +1.0F);
			}
		}

		//	Crossfade(inputFrom, inputTo, output) : Crossfades the two inputs to the output
		template <uint32_t Length>
		void Crossfade ( const float* bufferFrom, const float* bufferTo, float* out_buffer )
		{
			for (uint32_t i = 0; i < Length; ++i)
			{
				float fade = i / (float)(Length - 1);
				out_buffer[i] = math::lerp(fade, bufferFrom[i], bufferTo[i]);
			}
		}

		//	Interpolate(inputA, inputB, blend, output) : Linearly interpolates the two inputs to the output
		template <uint32_t Length>
		void Interpolate ( const float* bufferA, const float* bufferB, const float t, float* out_buffer )
		{
			for (uint32_t i = 0; i < Length; ++i)
			{
				out_buffer[i] = math::lerp(t, bufferA[i], bufferB[i]);
			}
		}

		//	Resample<LengthTo>(input, lengthFrom, output) : Resamples given input to the output.
		template <uint32_t LengthTo>
		void Resample ( const float* buffer, const uint32_t lengthFrom, float* out_buffer )
		{
			ResampleStride<LengthTo, 1>(buffer, lengthFrom, out_buffer);
		}

		//	ResampleStride<LengthTo,Stride>(input, lengthFrom, output) : Resamples given input with the given stride to the output.
		template <uint32_t LengthTo, uint32_t FrameStride>
		void ResampleStride ( const float* buffer, const uint32_t lengthFrom, float* out_buffer )
		{
			// No resampling needed. Just do a copy:
			if (LengthTo == lengthFrom)
			{
				for (uint32_t i = 0; i < LengthTo; ++i)
				{
					out_buffer[i] = buffer[i * FrameStride];
				}
			}
			// Low pitched, stretch
			else if (LengthTo > lengthFrom)
			{
				const uint32_t lengthFromSub1 = lengthFrom - 1;
				for (uint32_t i = 0; i < LengthTo; ++i)
				{					
					const float percentThruBuffer = i / (float)(LengthTo - 1);
					const float sourceFrame = lengthFromSub1 * percentThruBuffer;

					// Simple linear interpolation:
					const uint32_t sourceFrame0 = std::min((uint32_t)sourceFrame, lengthFromSub1);
					const uint32_t sourceFrame1 = std::min((uint32_t)sourceFrame + 1, lengthFromSub1);
					const float sourceFrameBlend = sourceFrame - sourceFrame0;

					const float wave0 = buffer[sourceFrame0 * FrameStride];
					const float wave1 = buffer[sourceFrame1 * FrameStride];
					out_buffer[i] = math::lerp(0.5F, math::lerp(sourceFrameBlend, wave0, wave1), math::largest(wave0, wave1)); // Bias it slightly towards the larger waveform to get crisper peaks.
				}
			}
			// High pitched, compress
			else if (LengthTo < lengthFrom)
			{
				const uint32_t lengthFromSub1 = lengthFrom - 1;
				for (uint32_t i = 0; i < LengthTo; ++i)
				{
					const float percentThruBufferStart = i / (float)(LengthTo);
					const float percentThruBufferEnd = (i + 1) / (float)(LengthTo);

					const float sourceFrameStart = lengthFrom * percentThruBufferStart;
					const float sourceFrameEnd = lengthFrom * percentThruBufferEnd;

					const uint32_t sourceFrameStart0 = std::min((uint32_t)sourceFrameStart, lengthFromSub1);
					const uint32_t sourceFrameStart1 = std::min((uint32_t)sourceFrameStart + 1, lengthFromSub1);
					const float sourceFrameStartBlend = sourceFrameStart - sourceFrameStart0;

					const uint32_t sourceFrameEnd0 = std::min((uint32_t)sourceFrameEnd, lengthFromSub1);
					const uint32_t sourceFrameEnd1 = std::min((uint32_t)sourceFrameEnd + 1, lengthFromSub1);
					const float sourceFrameEndBlend = sourceFrameEnd - sourceFrameEnd0;

					// We're going to do an average of the waveform sample we're compressing down:

					// Get the first interpolated sample
					const float waveStart0 = buffer[sourceFrameStart0 * FrameStride];
					const float waveStart1 = buffer[sourceFrameStart1 * FrameStride];
					const float waveStart = math::lerp(sourceFrameStartBlend, waveStart0, waveStart1);

					// Get the last interpolated sample
					const float waveEnd0 = buffer[sourceFrameEnd0 * FrameStride];
					const float waveEnd1 = buffer[sourceFrameEnd1 * FrameStride];
					const float waveEnd = math::lerp(sourceFrameEndBlend, waveEnd0, waveEnd1);

					// Get all the non-interpolated samples in-between
					float totalWave = 0.0;
					float largestWave = 0.0;
					const uint32_t numberOfSamples = 2 + std::max<int32_t>(0, (int32_t)sourceFrameEnd0 - (int32_t)sourceFrameStart1);
					for (uint32_t subsample = sourceFrameStart1; subsample < sourceFrameEnd0; ++subsample)
					{
						totalWave += buffer[subsample * FrameStride] / numberOfSamples;
						largestWave = math::largest(largestWave, buffer[subsample * FrameStride]);
					}
					totalWave += waveStart / numberOfSamples;
					totalWave += waveEnd / numberOfSamples;
					largestWave = math::largest(largestWave, math::largest(waveStart, waveEnd));

					out_buffer[i] = math::lerp(0.5F, totalWave, largestWave); // Bias it slightly towards the larger waveform to get crisper peaks.
				}
			}
		}

		//	ResampleStride<StrideIn, StrideOut>(input, lengthFrom, lengthTo, output) : Resamples given input with the given stride to the output.
		template <uint32_t FrameStrideIn, uint32_t FrameStrideOut>
		void ResampleStride ( const float* buffer, const uint32_t lengthFrom, const uint32_t lengthTo, float* out_buffer )
		{
			// No resampling needed. Just do a copy:
			if (lengthTo == lengthFrom)
			{
				for (uint32_t i = 0; i < lengthTo; ++i)
				{
					out_buffer[i * FrameStrideOut] = buffer[i * FrameStrideIn];
				}
			}
			// Low pitched, stretch
			else if (lengthTo > lengthFrom)
			{
				const uint32_t lengthFromSub1 = lengthFrom - 1;
				for (uint32_t i = 0; i < lengthTo; ++i)
				{					
					const float percentThruBuffer = i / (float)(lengthTo - 1);
					const float sourceFrame = lengthFromSub1 * percentThruBuffer;

					// Simple linear interpolation:
					const uint32_t sourceFrame0 = std::min((uint32_t)sourceFrame, lengthFromSub1);
					const uint32_t sourceFrame1 = std::min((uint32_t)sourceFrame + 1, lengthFromSub1);
					const float sourceFrameBlend = sourceFrame - sourceFrame0;

					const float wave0 = buffer[sourceFrame0 * FrameStrideIn];
					const float wave1 = buffer[sourceFrame1 * FrameStrideIn];
					out_buffer[i * FrameStrideOut] = math::lerp(0.3F, math::lerp(sourceFrameBlend, wave0, wave1), math::largest(wave0, wave1)); // Bias it slightly towards the larger waveform to get crisper peaks.
				}
			}
			// High pitched, compress
			else if (lengthTo < lengthFrom)
			{
				const uint32_t lengthFromSub1 = lengthFrom - 1;
				for (uint32_t i = 0; i < lengthTo; ++i)
				{
					const float percentThruBufferStart = i / (float)(lengthTo);
					const float percentThruBufferEnd = (i + 1) / (float)(lengthTo);

					const float sourceFrameStart = lengthFrom * percentThruBufferStart;
					const float sourceFrameEnd = lengthFrom * percentThruBufferEnd;

					const uint32_t sourceFrameStart0 = std::min((uint32_t)sourceFrameStart, lengthFromSub1);
					const uint32_t sourceFrameStart1 = std::min((uint32_t)sourceFrameStart + 1, lengthFromSub1);
					const float sourceFrameStartBlend = sourceFrameStart - sourceFrameStart0;

					const uint32_t sourceFrameEnd0 = std::min((uint32_t)sourceFrameEnd, lengthFromSub1);
					const uint32_t sourceFrameEnd1 = std::min((uint32_t)sourceFrameEnd + 1, lengthFromSub1);
					const float sourceFrameEndBlend = sourceFrameEnd - sourceFrameEnd0;

					// We're going to do an average of the waveform sample we're compressing down:

					// Get the first interpolated sample
					const float waveStart0 = buffer[sourceFrameStart0 * FrameStrideIn];
					const float waveStart1 = buffer[sourceFrameStart1 * FrameStrideIn];
					const float waveStart = math::lerp(sourceFrameStartBlend, waveStart0, waveStart1);

					// Get the last interpolated sample
					const float waveEnd0 = buffer[sourceFrameEnd0 * FrameStrideIn];
					const float waveEnd1 = buffer[sourceFrameEnd1 * FrameStrideIn];
					const float waveEnd = math::lerp(sourceFrameEndBlend, waveEnd0, waveEnd1);

					// Get all the non-interpolated samples in-between
					float totalWave = 0.0;
					float largestWave = 0.0;
					const uint32_t numberOfSamples = 2 + std::max<int32_t>(0, (int32_t)sourceFrameEnd0 - (int32_t)sourceFrameStart1);
					for (uint32_t subsample = sourceFrameStart1; subsample < sourceFrameEnd0; ++subsample)
					{
						totalWave += buffer[subsample * FrameStrideIn] / numberOfSamples;
						largestWave = math::largest(largestWave, buffer[subsample * FrameStrideIn]);
					}
					totalWave += waveStart / numberOfSamples;
					totalWave += waveEnd / numberOfSamples;
					largestWave = math::largest(largestWave, math::largest(waveStart, waveEnd));

					out_buffer[i * FrameStrideOut] = math::lerp(0.5F, totalWave, largestWave); // Bias it slightly towards the larger waveform to get crisper peaks.
				}
			}
		}

		template <uint32_t Length>
		void FFT (const float* buffer, float* out_buffer_real, float* out_buffer_imag)
		{
			float* input = (float*)mufft_alloc(Length * sizeof(float));
			std::complex<float>* output = (std::complex<float>*)mufft_alloc(Length * sizeof(std::complex<float>));

			for (uint32_t i = 0; i < Length; ++i)
			{
				input[i] = buffer[i];
			}

			mufft_plan_1d *muplan = mufft_create_plan_1d_r2c(Length, MUFFT_FLAG_CPU_ANY);
			mufft_execute_plan_1d(muplan, output, input);

			for (uint32_t i = 0; i < Length; ++i)
			{
				out_buffer_real[i] = output[i].real();
				out_buffer_imag[i] = output[i].imag();
			}

			mufft_free(input);
			mufft_free(output);
			mufft_free_plan_1d(muplan);
		}

		template <uint32_t Length>
		void InverseFFT (const float* buffer_real, const float* buffer_imag, float* out_buffer)
		{
			std::complex<float>* input = (std::complex<float>*)mufft_alloc(Length * sizeof(std::complex<float>));
			float* output = (float*)mufft_alloc(Length * sizeof(float));

			for (uint32_t i = 0; i < Length; ++i)
			{
				input[i].real(buffer_real[i]);
				input[i].imag(buffer_imag[i]);
			}

			mufft_plan_1d *muplan = mufft_create_plan_1d_c2r(Length, MUFFT_FLAG_CPU_ANY);
			mufft_execute_plan_1d(muplan, output, input);

			for (uint32_t i = 0; i < Length; ++i)
			{
				out_buffer[i] = output[i] / Length; // WHY IS IT SCALED UP???
			}

			mufft_free(input);
			mufft_free(output);
			mufft_free_plan_1d(muplan);
		}
	}
}

#endif//AUDIO_MIXING_OPERATIONS_H_