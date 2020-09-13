#include "Mixer.h"
#include "audio/Manager.h"
#include "audio/backend/AudioBackend.h"
#include "audio/Source.h"
#include "audio/mixing/Workbuffer.h"
#include "audio/mixing/Channels.h"

#include "core-ext/threads/Jobs.h"

#include <chrono>
#include <algorithm>

namespace audio
{
	struct MixSource
	{
		SourceState state;
		Workbuffer<1024, 2> workbuffer;
	};
}

audio::Mixer::Mixer ( Manager* object_state, AudioBackend* backend, uint32_t max_voices )
	: m_objectState(object_state)
	, m_backendTarget(backend)
	, m_maxVoices(max_voices)
{
	m_continueWork = true;

	// we need:
	//		workbuffer for each voice
	//		MixChannel::kMAX_COUNT workbuffers for each listener
	//			we want to tag each workbuffer with a channel
	//		workbuffer for the final output

	m_workerThread = std::thread([&]
	{
		double sampleRate = m_backendTarget->SampleRate();
		uint32_t channelCount = m_backendTarget->ChannelCount();
		double sampleStep = 1.0 / sampleRate;

		constexpr uint32_t kWorkbufferSize = 1024;
		float* workbuffer = new float [kWorkbufferSize * channelCount];

		std::vector<Source*> l_sources;
		std::vector<Listener*> l_listeners;
		std::vector<core::jobs::JobId> l_pendingJobs;

		while (m_continueWork)
		{
			// If we have available frames, throw them in
			uint32_t workingFrames = std::min<uint32_t>(m_backendTarget->AvailableFrames(), kWorkbufferSize);
			if (workingFrames >= kWorkbufferSize)
			{
				m_objectState->GetObjectStateForMixerThread(l_sources, l_listeners);

				// TODO: Consider per listener, not just the first listener.
				audio::Listener* listener = NULL;
				if (l_listeners.size() > 0)
				{
					listener = l_listeners[0];
				}

				for (audio::Source* source : l_sources)
				{
					if (source->IsPlaying())
					{
						//l_pendingJobs.push_back(
						//	core::jobs::System::Current::AddJobRequest([&]
						//	{
								source->MixerSampleAndAdvance(workingFrames, workbuffer);
						//	})
						//);
					}
				}

				// Wait for all the per-source sampling to finish
				for (auto job : l_pendingJobs)
				{
					core::jobs::System::Current::WaitForJob(job);
				}
				l_pendingJobs.clear();

				/*for (uint32_t frame = 0; frame < workingFrames; ++frame)
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
				}*/

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
	m_continueWork = false;
	m_workerThread.join();
}