#include "Mixer.h"
#include "audio/Manager.h"
#include "audio/backend/AudioBackend.h"
#include "audio/Source.h"
#include "audio/Buffer.h"
#include "audio/Listener.h"
#include "audio/mixing/Workbuffer.h"
#include "audio/mixing/Channels.h"
#include "audio/mixing/Operations.h"

#include "core-ext/threads/Jobs.h"
#include "core/math/Math.h"
#include "core/math/Math3d.h"

#include <chrono>
#include <algorithm>
#include <array>

namespace audio
{
	enum MixConstants : uint32_t
	{
		kWorkbufferSize = 1024,
		kWorkingMaxPitch = 4,
	};

	struct Audio3DMixConstants
	{
		// Rough max left-right ear delay for 3D audio
		static constexpr double kMaxAudioDelay = 0.660 / 1000.0;
		static constexpr double kActualCoolSoundingAudioDelay = 8.0 / 1000.0;

		// Maxing out the pitch because the workbuffers are not that bit
		static constexpr double kMaxPitch = (double)kWorkingMaxPitch;
	};
	
	struct SourceWorkbufferSet
	{
		// Pristine audio sampled from the source
		WorkbufferStereo<kWorkbufferSize * kWorkingMaxPitch>
							m_workbufferRaw;
		// [0] is pitched audio
		// [1] is post-processed audio
		std::array<WorkbufferStereo<kWorkbufferSize>, 2>
							m_workbuffers;

		// TODO: per-listener delay bank & pitch shift
		WorkbufferStereo<kWorkbufferSize + kWorkbufferSize>
							m_workbuffer_delay;
			
		SourceState			m_state;
		float				m_distanceFromListener; // Calculated when sampling.
		MixChannel			m_tag = MixChannel::kDefault;
		uint32_t			m_previousDelayLeft = 0;
		uint32_t			m_previousDelayRight = 0;

		bool				m_newframe = false;
		bool				m_newmix = false;
	};
}

void audio::Mixer::CleanupSource ( uint source_id )
{
	std::lock_guard<std::mutex> lock(m_sourceRemovalRequestsLock);
	m_sourceRemovalRequests.push_back(source_id);
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

		WorkbufferStereo<kWorkbufferSize>* workbuffer_channel_out = new WorkbufferStereo<kWorkbufferSize>[(uint)MixChannel::kMAX_COUNT];

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
				MixerObjectState mixObjectState;
				mixObjectState.m_sources = &l_sources;
				mixObjectState.m_listeners = &l_listeners;
				m_objectState->GetObjectStateForMixerThread(mixObjectState);

				audio::Listener* listener = NULL;
				if (l_listeners.size() > 0)
				{
					listener = l_listeners[0];
				}

				// Get listener info
				Vector3f listenerPosition = listener ? listener->position : Vector3f::zero;
				Vector3f listenerVelocity = listener ? listener->velocity : Vector3f::zero;
				Matrix3x3 listenerRotation = listener
					? Matrix3x3(listener->orient_forward.raw, listener->orient_forward.cross(listener->orient_up).raw, listener->orient_up.raw)
					: Matrix3x3();

				Matrix4x4 listenerTransform;
				{
					Matrix4x4 translation, rotation;
					translation.setTranslation(listenerPosition);
					rotation.setRotation(listenerRotation);
					listenerTransform = !translation * rotation;
				}
				Matrix4x4 listenerInverseTransform = listenerTransform.inverse();

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

							// Calculate the pitch
							double base_pitch = workbufferSet.m_state.pitch;

							Vector3f relative_velocity = workbufferSet.m_state.velocity - listenerVelocity;
							Vector3f relative_position = workbufferSet.m_state.position - listenerPosition;
							workbufferSet.m_distanceFromListener = relative_velocity.magnitude(); // Store since it's reused later
							float velocity_total = relative_velocity.magnitude();
							float velocity_pitch_multiplier = (relative_velocity / std::max<float>(FLOAT_PRECISION, velocity_total)).dot(relative_position / std::max<float>(FLOAT_PRECISION, workbufferSet.m_distanceFromListener));

							double final_pitch = math::clamp(base_pitch + base_pitch * (velocity_total / mixObjectState.m_speedOfSound * velocity_pitch_multiplier), 0.01, audio::Audio3DMixConstants::kMaxPitch);

							// From the pitch, calculate number of samples needed
							uint32_t samplesNeeded = (uint32_t)std::ceil(workingFrames * final_pitch);

							// First sample into the workbuffer
							source->MixerSampleAndAdvance(samplesNeeded, workbufferSet.m_workbufferRaw.m_data);
							// We now have raw interleaved audio. During the re-pitching, we split it into correct channels.

							// Now we re-pitch the audio:

							// No repitching. Just copy-paste.
							if (samplesNeeded == workingFrames)
							{	
								// Copy and paste if no scaling needed
								if (source->GetBuffer()->GetChannelCount() == ChannelCount::kMono)
								{
									// Duplicate into both left & right channels
									std::copy(workbufferSet.m_workbufferRaw.m_data, workbufferSet.m_workbufferRaw.m_data + kWorkbufferSize, workbufferSet.m_workbuffers[0].m_data_left);
									std::copy(workbufferSet.m_workbufferRaw.m_data, workbufferSet.m_workbufferRaw.m_data + kWorkbufferSize, workbufferSet.m_workbuffers[0].m_data_right);
								}
								else if (source->GetBuffer()->GetChannelCount() == ChannelCount::kStereo)
								{
									audio::mixing::InterleavedStereoToChannels<kWorkbufferSize>(
										workbufferSet.m_workbufferRaw.m_data,
										workbufferSet.m_workbuffers[0].m_data_left,
										workbufferSet.m_workbuffers[0].m_data_right
										);
								}
							}
							// Low pitched, stretch
							else if (samplesNeeded < workingFrames)
							{
								// TODO: actual interpolation to get rid of some of the crust
								if (source->GetBuffer()->GetChannelCount() == ChannelCount::kMono)
								{
									for (uint32_t frame = 0; frame < workingFrames; ++frame)
									{
										double percentThruBuffer = frame / (double)workingFrames;
										double sourceFrame = samplesNeeded * percentThruBuffer;
										uint32_t sourceFrame0 = (uint32_t)sourceFrame;

										workbufferSet.m_workbuffers[0].m_data_left[frame] = workbufferSet.m_workbufferRaw.m_data[sourceFrame0];
										workbufferSet.m_workbuffers[0].m_data_right[frame] = workbufferSet.m_workbufferRaw.m_data[sourceFrame0];
									}
								}
								else if (source->GetBuffer()->GetChannelCount() == ChannelCount::kStereo)
								{
									for (uint32_t frame = 0; frame < workingFrames; ++frame)
									{
										double percentThruBuffer = frame / (double)workingFrames;
										double sourceFrame = samplesNeeded * percentThruBuffer;
										uint32_t sourceFrame0 = (uint32_t)sourceFrame;

										workbufferSet.m_workbuffers[0].m_data_left[frame] = workbufferSet.m_workbufferRaw.m_data[sourceFrame0 * 2 + 0];
										workbufferSet.m_workbuffers[0].m_data_right[frame] = workbufferSet.m_workbufferRaw.m_data[sourceFrame0 * 2 + 1];
									}
								}
							}
							// High pitched, compress
							else if (samplesNeeded > workingFrames)
							{
								// TODO: actual interpolation to get rid of some of the crust
								if (source->GetBuffer()->GetChannelCount() == ChannelCount::kMono)
								{
									for (uint32_t frame = 0; frame < workingFrames; ++frame)
									{
										double percentThruBufferStart = frame / (double)workingFrames;
										double percentThruBufferEnd = (frame + 1) / (double)workingFrames;
										double sourceFrameStart = samplesNeeded * percentThruBufferStart;
										double sourceFrameEnd = samplesNeeded * percentThruBufferEnd;
										uint32_t sourceFrame0 = (uint32_t)sourceFrameStart;
										uint32_t sourceFrame1 = (uint32_t)std::ceil(sourceFrameEnd);

										workbufferSet.m_workbuffers[0].m_data_left[frame] = workbufferSet.m_workbufferRaw.m_data[sourceFrame0];
										workbufferSet.m_workbuffers[0].m_data_right[frame] = workbufferSet.m_workbufferRaw.m_data[sourceFrame0];
									}
								}
								else if (source->GetBuffer()->GetChannelCount() == ChannelCount::kStereo)
								{
									for (uint32_t frame = 0; frame < workingFrames; ++frame)
									{
										double percentThruBufferStart = frame / (double)workingFrames;
										double percentThruBufferEnd = (frame + 1) / (double)workingFrames;
										double sourceFrameStart = samplesNeeded * percentThruBufferStart;
										double sourceFrameEnd = samplesNeeded * percentThruBufferEnd;
										uint32_t sourceFrame0 = (uint32_t)sourceFrameStart;
										uint32_t sourceFrame1 = (uint32_t)std::ceil(sourceFrameEnd);

										workbufferSet.m_workbuffers[0].m_data_left[frame] = workbufferSet.m_workbufferRaw.m_data[sourceFrame0 * 2 + 0];
										workbufferSet.m_workbuffers[0].m_data_right[frame] = workbufferSet.m_workbufferRaw.m_data[sourceFrame0 * 2 + 1];
									}
								}
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
				
				// Perform per-source work to make them 3D:

				for (auto& setPair : m_sourceStateMap)
				{
					SourceWorkbufferSet* setRef = ((audio::SourceWorkbufferSet*)setPair.second);

					if (setRef->m_newframe)
					{
						// Mix in the new frame:
						auto process3dAudio = [&, setRef]
						{
							SourceWorkbufferSet& set = *setRef;

							// Eventually we'll do the actual per-listener & per-source 3D effects here

							// Let's transform the audio's position
							Vector3f localPosition = listenerInverseTransform * set.m_state.position;
							Real localPositionLength = localPosition.magnitude();
							Vector3f localPositionNormalized = localPosition / localPositionLength;

							// get the gain and the distance fade
							float base_gain = set.m_state.gain;
							float final_gain = base_gain;

							switch (set.m_state.falloffStyle)
							{
								case audio::Falloff::kLinear:
								{
									float distance_gain = 1.0F - math::saturate((localPositionLength - set.m_state.min_dist) / (set.m_state.max_dist - set.m_state.min_dist));
									final_gain = base_gain * distance_gain;
								}
								break;

								case audio::Falloff::kPower:
								{
									float distance_gain_linear = 1.0F - math::saturate((localPositionLength - set.m_state.min_dist) / (set.m_state.max_dist - set.m_state.min_dist));
									float distance_gain = std::powf(distance_gain_linear, set.m_state.falloff);
									final_gain = base_gain * distance_gain;
								}
								break;

								case audio::Falloff::kInverse:
								{
									float distance_gain_linear = std::max(0.0F, (localPositionLength - set.m_state.min_dist) / (set.m_state.max_dist - set.m_state.min_dist));
									float distance_gain = 1.0F / (1.0F + distance_gain_linear * set.m_state.falloff * 4.0F);
									final_gain = base_gain * distance_gain;
								}
								break;

								case audio::Falloff::kExponential:
								{
									float distance_gain_linear = std::max(0.0F, (localPositionLength - set.m_state.min_dist) / (set.m_state.max_dist - set.m_state.min_dist));
									float distance_gain = std::powf(0.5F, distance_gain_linear * set.m_state.falloff * 4.0F);
									final_gain = base_gain * distance_gain;
								}
								break;
							}

							// get the angle to the audio
							float cosine = localPositionNormalized.y;
							float distance_blend = math::clamp(localPositionLength, 0.0F, 1.0F);
						 
							// do spatial blend disable
							cosine = math::lerp(set.m_state.spatial, 0.0F, cosine);
							final_gain = math::lerp(set.m_state.spatial, base_gain, final_gain);

							// for now, we just delay the left the most amount of time
							uint32_t delayAmountLeft = 0;//(uint32_t)(distance_blend * std::max(0.0F, +cosine) * audio::Audio3DMixConstants::kActualCoolSoundingAudioDelay / sampleStep);
							uint32_t delayAmountRight = 0;//(uint32_t)(distance_blend * std::max(0.0F, -cosine) * audio::Audio3DMixConstants::kActualCoolSoundingAudioDelay / sampleStep);

							// TODO: We're just going to nix delay until we can actually figure out how to properly do a variable delay without a ton of clicks.
							// Honestly, the gain changing has clicks too. Are there two samples generated, the prev & current state, and the two just blended together?
							// ...that's probably it, isn't it.

							// stretch the current delay to the new delay
							//copy from m_workbuffer_delay to buffer1
							//rescale from buffer1 to m_workbuffer_delay
							std::copy(set.m_workbuffer_delay.m_data_left, set.m_workbuffer_delay.m_data_left + set.m_previousDelayLeft, set.m_workbuffers[1].m_data_left);
							std::copy(set.m_workbuffer_delay.m_data_right, set.m_workbuffer_delay.m_data_right + set.m_previousDelayRight, set.m_workbuffers[1].m_data_right);
							// rescale the data
							for (uint32_t frame = 0; frame < delayAmountLeft; ++frame)
							{
								uint32_t frameSource = (uint32_t)(frame * (set.m_previousDelayLeft / (double)delayAmountLeft));
								set.m_workbuffer_delay.m_data_left[frame] = set.m_workbuffers[1].m_data_left[frameSource];
							}
							for (uint32_t frame = 0; frame < delayAmountRight; ++frame)
							{
								uint32_t frameSource = (uint32_t)(frame * (set.m_previousDelayRight / (double)delayAmountRight));
								set.m_workbuffer_delay.m_data_right[frame] = set.m_workbuffers[1].m_data_right[frameSource];
							}

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
							// clear the now-unused part of the bank
							std::fill(set.m_workbuffer_delay.m_data_left + kWorkbufferSize, set.m_workbuffer_delay.m_data_left + kWorkbufferSize + kWorkbufferSize, 0.0F);
							std::fill(set.m_workbuffer_delay.m_data_right + kWorkbufferSize, set.m_workbuffer_delay.m_data_right + kWorkbufferSize + kWorkbufferSize, 0.0F);

							// scale left and right
							const float gain_left = math::lerp(set.m_state.spatial, 1.0F, 1.0F - std::max(0.0F, +cosine) * 0.7F);
							const float gain_right = math::lerp(set.m_state.spatial, 1.0F, 1.0F - std::max(0.0F, -cosine) * 0.7F);
							audio::mixing::Scale<kWorkbufferSize>(set.m_workbuffers[1].m_data_left, final_gain * gain_left);
							audio::mixing::Scale<kWorkbufferSize>(set.m_workbuffers[1].m_data_right, final_gain * gain_right);

							// do fades if we have a new delay and no previous data in the buffer (gotta fix the clicks!)
							/*if (delayAmountLeft != 0 && set.m_previousDelayLeft == 0)
							{
								uint32_t fadeLength = std::min<uint32_t>(kWorkbufferSize - delayAmountLeft, 32);
								for (uint32_t frame = 0; frame < fadeLength; ++frame)
								{
									float fade_in_gain = frame / (float)fadeLength;
									set.m_workbuffers[1].m_data_left[frame + delayAmountLeft] *= fade_in_gain;
								}
							}
							if (delayAmountRight != 0 && set.m_previousDelayRight == 0)
							{
								uint32_t fadeLength = std::min<uint32_t>(kWorkbufferSize - delayAmountRight, 32);
								for (uint32_t frame = 0; frame < fadeLength; ++frame)
								{
									float fade_in_gain = frame / (float)fadeLength;
									set.m_workbuffers[1].m_data_right[frame + delayAmountRight] *= fade_in_gain;
								}
							}*/

							// save delay so we can stretch the current delay if things change
							set.m_previousDelayLeft = delayAmountLeft;
							set.m_previousDelayRight = delayAmountRight;

							// The fucked-up sampled audio now lives in workbuffer 1
						};

						// Push the new job
						l_pendingJobs.push_back(core::jobs::System::Current::AddJobRequest(process3dAudio));

						// Set new state
						setRef->m_newframe = false; // Data consumed
						setRef->m_newmix = true; // New mix
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

				// Check for sources we can remove now
				{
					std::lock_guard<std::mutex> lock(m_sourceRemovalRequestsLock);
					for (auto iterSourceId = m_sourceRemovalRequests.begin(); iterSourceId != m_sourceRemovalRequests.end(); )
					{
						uint sourceId = *iterSourceId;
						SourceWorkbufferSet& workbufferSet = FindSourceWorkbufferSet(sourceId);
						// If no new mix, we can remove it from the list.
						if (!workbufferSet.m_newmix)
						{
							// Deallocate the item
							auto pair = m_sourceStateMap.find(sourceId);
							ARCORE_ASSERT(pair != m_sourceStateMap.end());
							delete pair->second;
							m_sourceStateMap.erase(pair);

							// Remove from list
							iterSourceId = m_sourceRemovalRequests.erase(iterSourceId);
						}
						else
						{
							++iterSourceId;
						}
					}
				}

				// Now push all source data into their relevant channels:

				// Clear all channels
				for (uint32_t mixchannel = 0; mixchannel < (uint32_t)MixChannel::kMAX_COUNT; ++mixchannel)
				{
					std::fill(workbuffer_channel_out[mixchannel].m_data, workbuffer_channel_out[mixchannel].m_data + kWorkbufferSize * channelCount, 0.0F);
				}
				// Acculmulate all audio into their channels
				for (auto& setPair : m_sourceStateMap)
				{
					SourceWorkbufferSet& set = *((audio::SourceWorkbufferSet*)setPair.second);

					if (set.m_newmix)
					{
						WorkbufferStereo<kWorkbufferSize>& workbuffer_channel = workbuffer_channel_out[(uint32_t)set.m_state.channel];

						audio::mixing::ChannelsToInterleavedStereo<kWorkbufferSize>(set.m_workbuffers[1].m_data_left, set.m_workbuffers[1].m_data_right, workbuffer_mix);
						audio::mixing::Acculmulate<kWorkbufferSize * 2>(workbuffer_mix, workbuffer_channel.m_data);

						// Done mixing here
						set.m_newmix = false;
					}
				}
				// Scale channels & acculmulate channels data into the main buffer
				std::fill(workbuffer_out, workbuffer_out + kWorkbufferSize * channelCount, 0.0F);
				for (uint32_t mixchannel = 0; mixchannel < (uint32_t)MixChannel::kMAX_COUNT; ++mixchannel)
				{
					audio::mixing::Scale<kWorkbufferSize * 2>(workbuffer_channel_out[mixchannel].m_data, mixObjectState.m_channelGain[mixchannel]);
					audio::mixing::Acculmulate<kWorkbufferSize * 2>(workbuffer_channel_out[mixchannel].m_data, workbuffer_out);
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

		delete[] workbuffer_channel_out;
		delete[] workbuffer_out;
		delete[] workbuffer_mix;
	});
}

audio::Mixer::~Mixer ( void )
{
	m_continueWork = false;
	m_workerThread.join();

	// Free all the mixing states
	for (auto& setPair : m_sourceStateMap)
	{
		delete setPair.second;
	}
}