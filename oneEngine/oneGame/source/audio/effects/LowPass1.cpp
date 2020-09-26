#include "LowPass1.h"

#include "core/math/Math.h"
#include "audio/mixing/Operations.h"

audio::effect::LowPass1::LowPass1 ( MixChannel targetChannel )
	: audio::Effect( targetChannel )
{
	;
}

//	AllocateState() : Allocates delete-safe state for the mixer thread.
audio::EffectState* audio::effect::LowPass1::AllocateState ( void )
{
	return new audio::effect::LowPass1State;
}

//	UpdateStateForMixerThread( state ) : Grabs current game-side state and updates the given state in the mixer thread.
void audio::effect::LowPass1::UpdateStateForMixerThread ( EffectState* state )
{
	std::lock_guard<std::mutex> lock(mixer_read_lock);
}

//	Evaluate( input, output, state ) : Performs the actual work in the mixer thread.
void audio::effect::LowPass1::Evaluate ( WorkbufferStereo<audio::kWorkbufferSize>& input, WorkbufferStereo<audio::kWorkbufferSize>& output, EffectState* effectState )
{
	// shift current data down
	for (uint i = 0; i < kWorkbufferSize * 3; ++i)
	{
		m_waveform.m_data_left[i] = m_waveform.m_data_left[i + kWorkbufferSize];
		m_waveform.m_data_right[i] = m_waveform.m_data_right[i + kWorkbufferSize];
	}

	// copy new data to the end
	std::copy(input.m_data_left, input.m_data_left + kWorkbufferSize, m_waveform.m_data_left + kWorkbufferSize * 3);
	std::copy(input.m_data_right, input.m_data_right + kWorkbufferSize, m_waveform.m_data_right + kWorkbufferSize * 3);

	// copy full data to buffer
	std::copy(m_waveform.m_data, m_waveform.m_data + kWorkbufferSize * 8, m_waveformOutput.m_data);

	// fft on the new window of data
	audio::mixing::FFT<kWorkbufferSize * 4>(m_waveformOutput.m_data_left, m_fft.m_data_left, m_fft_imag.m_data_left);
	audio::mixing::FFT<kWorkbufferSize * 4>(m_waveformOutput.m_data_right, m_fft.m_data_right, m_fft_imag.m_data_right);

	// perform the fft fucking
	// let's fade out the back end of it
	for (uint32_t i = 0; i < kWorkbufferSize * 4; ++i)
	{
		float percent = i / (float)(kWorkbufferSize * 4 - 1);

		//percent = (float)pow(1.0 - percent, 100);
		//percent = (float)pow(math::saturate(percent + 0.95), 100);
		//percent  = (float)pow(math::saturate(1.0 - percent), 100);

		percent = (float)math::saturate((0.005 - percent) / 0.003);

		m_fft.m_data_left[i] *= percent;
		m_fft.m_data_right[i] *= percent;
		m_fft_imag.m_data_left[i] *= percent;
		m_fft_imag.m_data_right[i] *= percent;
	}

	// inv fft on the fucked frequence response
	audio::mixing::InverseFFT<kWorkbufferSize * 4>(m_fft.m_data_left, m_fft_imag.m_data_left, m_waveformOutput.m_data_left);
	audio::mixing::InverseFFT<kWorkbufferSize * 4>(m_fft.m_data_right, m_fft_imag.m_data_right, m_waveformOutput.m_data_right);

	// copy middle of window (previous sample) back out
	std::copy(m_waveformOutput.m_data_left + kWorkbufferSize * 2, m_waveformOutput.m_data_left + kWorkbufferSize * 3, output.m_data_left);
	std::copy(m_waveformOutput.m_data_right + kWorkbufferSize * 2, m_waveformOutput.m_data_right + kWorkbufferSize * 3, output.m_data_right);
}