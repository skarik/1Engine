#include "Mixer.h"
#include "audio/backend/AudioBackend.h"

#include <chrono>
#include <algorithm>

audio::Mixer::Mixer ( AudioBackend* backend )
{
	m_backendTarget = backend;
	m_continueWork = true;

	m_workerThread = std::thread([&]
	{
		static double time = 0.0;

		double sampleRate = m_backendTarget->SampleRate();
		uint32_t channelCount = m_backendTarget->ChannelCount();
		double sampleStep = 1.0 / sampleRate;

		constexpr uint32_t kWorkbufferSize = 1024;
		float* workbuffer = new float [kWorkbufferSize * channelCount];

		while (m_continueWork)
		{
			// If we have available frames, throw them in
			uint32_t workingFrames = std::min<uint32_t>(m_backendTarget->AvailableFrames(), kWorkbufferSize);
			if (workingFrames > 0)
			{
				for (uint32_t frame = 0; frame < workingFrames; ++frame)
				{
					// Get the amplitudes set up
					float amplitudeLeft = (float)sin(time * 2 * M_PI * 440) * 0.2f;
					float amplitudeRight = (float)sin(time * 2 * M_PI * 523.251) * 0.2f;
					
					// TODO: here we would mix our internal stereo into the correct channel number

					if (channelCount == 2)
					{
						workbuffer[frame * 2 + 0] = amplitudeLeft;
						workbuffer[frame * 2 + 1] = amplitudeRight;
					}

					// Go to next step
					time += sampleStep;
				}

				// Submit our mixed buffer
				m_backendTarget->SubmitFrames(workingFrames, workbuffer);
			}

			// Sleep 1ms to relax the CPU as little as we can
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		delete[] workbuffer;
	});
}

audio::Mixer::~Mixer ( void )
{
	m_workerThread.join();
}