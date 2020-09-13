#include "Mixer.h"
#include "audio/Manager.h"
#include "audio/backend/AudioBackend.h"
#include "audio/Source.h"
#include "audio/Buffer.h"
#include "audio/mixing/Workbuffer.h"
#include "audio/mixing/Channels.h"
#include "audio/mixing/Operations.h"

#include "core-ext/threads/Jobs.h"
#include "core/math/Math.h"

#include <chrono>
#include <algorithm>
#include <array>

namespace audio
{
	enum MixConstants : uint32_t
	{
		kWorkbufferSize = 1024,
	};

	struct Audio3DMixConstants
	{
		// Rough max left-right ear delay for 3D audio
		static constexpr double kMaxAudioDelay = 0.660 / 1000.0;
		static constexpr double kActualCoolSoundingAudioDelay = 8.0 / 1000.0;
	};
	
	struct SourceWorkbufferSet
	{
		// [0] will be pristine audio
		// [1] will be post-processed audio
		std::array<WorkbufferStereo<kWorkbufferSize>, 2>
							m_workbuffers;

		// TODO: per-listener delay bank & pitch shift
		WorkbufferStereo<kWorkbufferSize + kWorkbufferSize>
							m_workbuffer_delay;
			
		SourceState			m_state;
		MixChannel			m_tag = MixChannel::kDefault;

		bool				m_newframe = false;
		bool				m_newmix = false;
	};
}

void audio::Mixer::CleanupSource ( uint source_id )
{

}

audio::SourceWorkbufferSet& audio::Mixer::FindSourceWorkbufferSet ( uint source_id )
{
	void* source_data = NULL;//m_sourceStateMap.fin
	auto source_data_find_result = m_sourceStateMap.find(source_id);
	if (source_data_find_result == m_sourceStateMap.end())
	{
		source_data = new SourceWorkbufferSet();
		m_sourceStateMap.insert({source_id, source_data});
	}
	else
	{
		source_data = source_data_find_result->second;
	}

	return *((audio::SourceWorkbufferSet*)source_data);
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
		const double sampleRate = m_backendTarget->SampleRate();
		const uint32_t channelCount = 2;//m_backendTarget->ChannelCount(); // Always assume stereo for now
		const double sampleStep = 1.0 / sampleRate;

		float* workbuffer_out = new float [kWorkbufferSize * channelCount];
		float* workbuffer_mix = new float [kWorkbufferSize * channelCount];

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

				audio::Listener* listener = NULL;
				if (l_listeners.size() > 0)
				{
					listener = l_listeners[0];
				}

				// Sample all the sources, but do not yet process them
				for (audio::Source* source : l_sources)
				{
					if (source->IsPlaying())
					{
						auto sampleAndDemux = [&, source]
						{
							// Grab the workbuffer set for this source:
							SourceWorkbufferSet& workbufferSet = FindSourceWorkbufferSet(source->GetID());

							// Get the source state
							source->MixerGetSourceState(workbufferSet.m_state);

							// First sample into the workbuffer
							source->MixerSampleAndAdvance(workingFrames, workbufferSet.m_workbuffers[1].m_data);

							// System works in stereo, so create the stereo channels for this sound:
							if (source->GetBuffer()->GetChannelCount() == ChannelCount::kMono)
							{
								// Duplicate into both left & right channels
								std::copy(workbufferSet.m_workbuffers[1].m_data, workbufferSet.m_workbuffers[1].m_data + kWorkbufferSize, workbufferSet.m_workbuffers[0].m_data_left);
								std::copy(workbufferSet.m_workbuffers[1].m_data, workbufferSet.m_workbuffers[1].m_data + kWorkbufferSize, workbufferSet.m_workbuffers[0].m_data_right);
							}
							else if (source->GetBuffer()->GetChannelCount() == ChannelCount::kStereo)
							{
								audio::mixing::InterleavedStereoToChannels<kWorkbufferSize>(
									workbufferSet.m_workbuffers[1].m_data,
									workbufferSet.m_workbuffers[0].m_data_left,
									workbufferSet.m_workbuffers[0].m_data_right
									);
							}

							workbufferSet.m_tag = workbufferSet.m_state.channel;
							workbufferSet.m_newframe = true;

							// The pristine sampled audio now lives in workbuffer 0
						};

						l_pendingJobs.push_back(core::jobs::System::Current::AddJobRequest(sampleAndDemux));
					}
				}

				// Wait for all the per-source sampling to finish
				for (auto job : l_pendingJobs)
				{
					core::jobs::System::Current::WaitForJob(job);
				}
				l_pendingJobs.clear();
				
				// Perform per-source work to make them 3D
				for (auto& setPair : m_sourceStateMap)
				{
					SourceWorkbufferSet& set = *((audio::SourceWorkbufferSet*)setPair.second);

					if (set.m_newframe)
					{
						// Mix in the new frame:

						// For now we just copy, but eventually we'll do the actual per-listener & per-source 3D effects here
						//std::copy(set.m_workbuffers[0].m_data, set.m_workbuffers[0].m_data + kWorkbufferSize * 2, set.m_workbuffers[1].m_data);

						// get the angle to the audio
						
						float angle = atan2(set.m_state.position.y, set.m_state.position.x);
						float cosine = cos(angle);
						float distance_blend = math::clamp(set.m_state.position.sqrMagnitude(), 0.0F, 1.0F);
						 
						// for now, we just delay the left the most amount of time
						uint32_t delayAmountLeft = (uint32_t)(distance_blend * std::max(0.0F, cosine) * audio::Audio3DMixConstants::kActualCoolSoundingAudioDelay / sampleStep);
						uint32_t delayAmountRight = (uint32_t)(distance_blend * std::max(0.0F, -cosine) * audio::Audio3DMixConstants::kActualCoolSoundingAudioDelay / sampleStep);

						// let's do left & right

						// copy the data offset into the buffer
						std::copy(set.m_workbuffers[0].m_data_left, set.m_workbuffers[0].m_data_left + kWorkbufferSize, set.m_workbuffer_delay.m_data_left + delayAmountLeft);
						std::copy(set.m_workbuffers[0].m_data_right, set.m_workbuffers[0].m_data_right + kWorkbufferSize, set.m_workbuffer_delay.m_data_right + delayAmountRight);
						// copy the first half into target
						std::copy(set.m_workbuffer_delay.m_data_left, set.m_workbuffer_delay.m_data_left + kWorkbufferSize, set.m_workbuffers[1].m_data_left);
						std::copy(set.m_workbuffer_delay.m_data_right, set.m_workbuffer_delay.m_data_right + kWorkbufferSize, set.m_workbuffers[1].m_data_right);
						// shift the bank downward
						std::copy(set.m_workbuffer_delay.m_data_left + kWorkbufferSize, set.m_workbuffer_delay.m_data_left + kWorkbufferSize + kWorkbufferSize, set.m_workbuffer_delay.m_data_left);
						std::copy(set.m_workbuffer_delay.m_data_right + kWorkbufferSize, set.m_workbuffer_delay.m_data_right + kWorkbufferSize + kWorkbufferSize, set.m_workbuffer_delay.m_data_right);

						// Set new state
						set.m_newframe = false; // Data consumed
						set.m_newmix = true; // New mix

						 // The fucked-up sampled audio now lives in workbuffer 0
					}
					else
					{
						// Otherwise continue the old working
					}

				}

				// Wait for all the per-source sampling to finish
				for (auto job : l_pendingJobs)
				{
					core::jobs::System::Current::WaitForJob(job);
				}
				l_pendingJobs.clear();

				// Now push all source data into the main buffer
				std::fill(workbuffer_out, workbuffer_out + kWorkbufferSize * channelCount, 0.0F);
				for (auto& setPair : m_sourceStateMap)
				{
					SourceWorkbufferSet& set = *((audio::SourceWorkbufferSet*)setPair.second);

					if (set.m_newmix)
					{
						audio::mixing::ChannelsToInterleavedStereo<kWorkbufferSize>(set.m_workbuffers[1].m_data_left, set.m_workbuffers[1].m_data_right, workbuffer_mix);
						audio::mixing::Acculmulate<kWorkbufferSize * 2>(workbuffer_mix, workbuffer_out);

						// Done mixing here
						set.m_newmix = false;
					}
				}

				//// Generate sine waves to verify device working properly
				//for (uint32_t frame = 0; frame < workingFrames; ++frame)
				//{
				//	// Get the amplitudes set up
				//	float amplitudeLeft = (float)sin(time * 2 * M_PI * 440) * 0.2f;
				//	float amplitudeRight = (float)sin(time * 2 * M_PI * 523.251) * 0.2f;
				//	
				//	// TODO: here we would mix our internal stereo into the correct channel number
				//	if (channelCount == 2)
				//	{
				//		workbuffer[frame * 2 + 0] = amplitudeLeft;
				//		workbuffer[frame * 2 + 1] = amplitudeRight;
				//	}
				//	// Go to next step
				//	time += sampleStep;
				//}

				// Submit our mixed buffer
				m_backendTarget->SubmitFrames(workingFrames, workbuffer_out);
			}

			// Sleep 1ms to relax the CPU as little as we can
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		delete[] workbuffer_out;
		delete[] workbuffer_mix;
	});
}

audio::Mixer::~Mixer ( void )
{
	m_continueWork = false;
	m_workerThread.join();
}