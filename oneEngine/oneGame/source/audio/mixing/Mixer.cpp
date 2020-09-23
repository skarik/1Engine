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
		static constexpr double kActualCoolSoundingAudioDelay = 16.0 / 1000.0;

		// Maxing out the pitch because the workbuffers are not that big
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

		// Pitched audio that resulted from the source sampling.
		WorkbufferStereo<kWorkbufferSize>
							m_workbuffer_sourceResult;
		// Result audio that resulted from the 3D DSP step
		WorkbufferStereo<kWorkbufferSize>
							m_workbuffer_3dDSPResult;

		// TODO: per-listener delay bank & pitch shift
		std::array<WorkbufferStereo<kWorkbufferSize + kWorkbufferSize>, 2>
							m_workbuffers_delay;
		uint32_t			m_delay_step = 0;

		SourceState			m_state;
		SourceState			m_statePrevious;
		float				m_distanceFromListener = 0.0F; // Calculated when sampling.
		MixChannel			m_tag = MixChannel::kDefault;

		bool				m_firstFrame = true;
		bool				m_newframe = false;
		bool				m_newmix = false;
	};
}

void audio::Mixer::CleanupSource ( uint source_id )
{
	std::lock_guard<std::mutex> lock(m_sourceRemovalRequestsLock);
	m_sourceRemovalRequests.push_back(source_id);
}

void audio::Mixer::AllocateSourceWorkbufferSet ( uint source_id )
{
	void* source_data = NULL;
	auto source_data_find_result = m_sourceStateMap.find(source_id);
	if (source_data_find_result == m_sourceStateMap.end())
	{
		source_data = new SourceWorkbufferSet();
		m_sourceStateMap.insert({source_id, source_data});
	}
}

audio::SourceWorkbufferSet& audio::Mixer::FindSourceWorkbufferSet ( uint source_id )
{
	void* source_data = NULL;
	auto source_data_find_result = m_sourceStateMap.find(source_id);
	if (source_data_find_result == m_sourceStateMap.end())
	{
		source_data = m_sourceStateMap.find(0xFFFFFFFF)->second;
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

	m_workerThread = std::thread([&]
	{
		const double sampleRate = m_backendTarget->SampleRate();
		const uint32_t channelCount = 2;//m_backendTarget->ChannelCount(); // Always assume stereo for now
		const double sampleStep = 1.0 / sampleRate;

		WorkbufferStereo<kWorkbufferSize>* workbuffer_channel_out = new WorkbufferStereo<kWorkbufferSize>[(uint)MixChannel::kMAX_COUNT];

		float* workbuffer_out = new float [kWorkbufferSize * channelCount];
		float* workbuffer_mix = new float [kWorkbufferSize * channelCount];

		WorkbufferStereo<kWorkbufferSize>* workbuffer_fft_real = new WorkbufferStereo<kWorkbufferSize>;
		WorkbufferStereo<kWorkbufferSize>* workbuffer_fft_imag = new WorkbufferStereo<kWorkbufferSize>;

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
					listenerTransform = translation * rotation;
				}
				Matrix4x4 listenerInverseTransform = listenerTransform.inverse();

				// Allocate data for all the sources
				AllocateSourceWorkbufferSet(0xFFFFFFFF); // Allocate dummy set for if we have a bad state.
				for (audio::Source* source : l_sources)
				{
					AllocateSourceWorkbufferSet(source->GetID());
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

							// Update previous state if first frame
							if (workbufferSet.m_firstFrame)
							{
								workbufferSet.m_statePrevious = workbufferSet.m_state;
								workbufferSet.m_firstFrame = false;
							}

							// Calculate the pitch
							double base_pitch = workbufferSet.m_state.pitch;

							Vector3f relative_velocity = workbufferSet.m_state.velocity - listenerVelocity;
							Vector3f relative_position = workbufferSet.m_state.position - listenerPosition;
							workbufferSet.m_distanceFromListener = relative_position.magnitude(); // Store since it's reused later
							float velocity_total = relative_velocity.magnitude();
							float velocity_pitch_multiplier = 
								-(relative_velocity / std::max<float>(FLOAT_PRECISION, velocity_total)).dot(
									relative_position / std::max<float>(FLOAT_PRECISION, workbufferSet.m_distanceFromListener)
								);

							double final_pitch = math::clamp(base_pitch + base_pitch * (velocity_total / mixObjectState.m_speedOfSound * velocity_pitch_multiplier), 0.01, audio::Audio3DMixConstants::kMaxPitch);

							// Disable the pitch shifting if in 2D mode
							final_pitch = math::lerp<double>(workbufferSet.m_state.spatial, base_pitch, final_pitch);

							// From the pitch, calculate number of samples needed
							uint32_t samplesNeeded = (uint32_t)std::ceil(workingFrames * final_pitch);

							// First sample into the workbuffer
							source->MixerSampleAndAdvance(samplesNeeded, workbufferSet.m_workbufferRaw.m_data);
							// We now have raw interleaved audio. During the re-pitching, we split it into correct channels.

							// Now we re-pitch the audio:
							if (source->GetBuffer()->GetChannelCount() == ChannelCount::kMono)
							{
								audio::mixing::Resample<kWorkbufferSize>(workbufferSet.m_workbufferRaw.m_data, samplesNeeded, workbufferSet.m_workbuffer_sourceResult.m_data_left);
								std::copy(workbufferSet.m_workbuffer_sourceResult.m_data_left, workbufferSet.m_workbuffer_sourceResult.m_data_left + workingFrames, workbufferSet.m_workbuffer_sourceResult.m_data_right);
							}
							else
							{
								audio::mixing::ResampleStride<kWorkbufferSize, 2>(workbufferSet.m_workbufferRaw.m_data, samplesNeeded, workbufferSet.m_workbuffer_sourceResult.m_data_left);
								audio::mixing::ResampleStride<kWorkbufferSize, 2>(workbufferSet.m_workbufferRaw.m_data + 1, samplesNeeded, workbufferSet.m_workbuffer_sourceResult.m_data_right);
							}

							workbufferSet.m_tag = workbufferSet.m_state.channel;
							workbufferSet.m_newframe = true;
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

							auto processForSourceState = [&listenerInverseTransform, &sampleStep](
								const SourceState& in_state,
								WorkbufferStereo<kWorkbufferSize>& in_workbuffer,
								WorkbufferStereo<kWorkbufferSize>& scratch_workbuffer,
								WorkbufferStereo<kWorkbufferSize + kWorkbufferSize>& delay_workbuffer,
								WorkbufferStereo<kWorkbufferSize>& out_workbuffer)
							{
								// Let's transform the audio's position
								Vector3f localPosition = listenerInverseTransform * in_state.position;
								Real localPositionLength = localPosition.magnitude();
								Vector3f localPositionNormalized = localPosition / localPositionLength;

								// get the gain and the distance fade
								float base_gain = in_state.gain;
								float final_gain = base_gain;

								switch (in_state.falloffStyle)
								{
									case audio::Falloff::kLinear:
									{
										float distance_gain = 1.0F - math::saturate((localPositionLength - in_state.min_dist) / (in_state.max_dist - in_state.min_dist));
										final_gain = base_gain * distance_gain;
									}
									break;

									case audio::Falloff::kPower:
									{
										float distance_gain_linear = 1.0F - math::saturate((localPositionLength - in_state.min_dist) / (in_state.max_dist - in_state.min_dist));
										float distance_gain = std::powf(distance_gain_linear, in_state.falloff);
										final_gain = base_gain * distance_gain;
									}
									break;

									case audio::Falloff::kInverse:
									{
										float distance_gain_linear = std::max(0.0F, (localPositionLength - in_state.min_dist) / (in_state.max_dist - in_state.min_dist));
										float distance_gain = 1.0F / (1.0F + distance_gain_linear * in_state.falloff * 4.0F);
										final_gain = base_gain * distance_gain;
									}
									break;

									case audio::Falloff::kExponential:
									{
										float distance_gain_linear = std::max(0.0F, (localPositionLength - in_state.min_dist) / (in_state.max_dist - in_state.min_dist));
										float distance_gain = std::powf(0.5F, distance_gain_linear * in_state.falloff * 4.0F);
										final_gain = base_gain * distance_gain;
									}
									break;
								}

								// get the angle to the audio
								float cosine = localPositionNormalized.y;
								float distance_blend = math::clamp(localPositionLength, 0.0F, 1.0F);

								// do spatial blend disable
								cosine = math::lerp(in_state.spatial, 0.0F, cosine);
								final_gain = math::lerp(in_state.spatial, base_gain, final_gain);

								// for now, we just delay the left the most amount of time
								uint32_t delayAmountLeft = (uint32_t)(distance_blend * std::max(0.0F, +cosine) * audio::Audio3DMixConstants::kActualCoolSoundingAudioDelay / sampleStep);
								uint32_t delayAmountRight = (uint32_t)(distance_blend * std::max(0.0F, -cosine) * audio::Audio3DMixConstants::kActualCoolSoundingAudioDelay / sampleStep);

								// copy the data offset into the buffer
								std::copy(in_workbuffer.m_data_left, in_workbuffer.m_data_left + kWorkbufferSize, delay_workbuffer.m_data_left + delayAmountLeft);
								std::copy(in_workbuffer.m_data_right, in_workbuffer.m_data_right + kWorkbufferSize, delay_workbuffer.m_data_right + delayAmountRight);
								// copy the first half into target
								std::copy(delay_workbuffer.m_data_left, delay_workbuffer.m_data_left + kWorkbufferSize, scratch_workbuffer.m_data_left);
								std::copy(delay_workbuffer.m_data_right, delay_workbuffer.m_data_right + kWorkbufferSize, scratch_workbuffer.m_data_right);
								// shift the bank downward
								std::copy(delay_workbuffer.m_data_left + kWorkbufferSize, delay_workbuffer.m_data_left + kWorkbufferSize + kWorkbufferSize, delay_workbuffer.m_data_left);
								std::copy(delay_workbuffer.m_data_right + kWorkbufferSize, delay_workbuffer.m_data_right + kWorkbufferSize + kWorkbufferSize, delay_workbuffer.m_data_right);
								// clear the now-unused part of the bank
								std::fill(delay_workbuffer.m_data_left + kWorkbufferSize, delay_workbuffer.m_data_left + kWorkbufferSize + kWorkbufferSize, 0.0F);
								std::fill(delay_workbuffer.m_data_right + kWorkbufferSize, delay_workbuffer.m_data_right + kWorkbufferSize + kWorkbufferSize, 0.0F);

								// scale left and right
								const float gain_left = math::lerp(in_state.spatial, 1.0F, 1.0F - std::max(0.0F, +cosine) * 0.7F);
								const float gain_right = math::lerp(in_state.spatial, 1.0F, 1.0F - std::max(0.0F, -cosine) * 0.7F);
								audio::mixing::Scale<kWorkbufferSize>(scratch_workbuffer.m_data_left, final_gain * gain_left);
								audio::mixing::Scale<kWorkbufferSize>(scratch_workbuffer.m_data_right, final_gain * gain_right);

								// now output
								std::copy(scratch_workbuffer.m_data_left, scratch_workbuffer.m_data_left + kWorkbufferSize, out_workbuffer.m_data_left);
								std::copy(scratch_workbuffer.m_data_right, scratch_workbuffer.m_data_right + kWorkbufferSize, out_workbuffer.m_data_right);
							};

							// To prevent clicks with the delay bank, we must blend with the previous frame's delay state - thus we need to mix the 3D audio twice:
							if (set.m_delay_step == 0)
							{
								processForSourceState(
									set.m_state,
									set.m_workbuffer_sourceResult, set.m_workbuffers[0], set.m_workbuffers_delay[0], set.m_workbuffer_3dDSPResult);
								processForSourceState(
									set.m_statePrevious,
									set.m_workbuffer_sourceResult, set.m_workbuffers[0], set.m_workbuffers_delay[1], set.m_workbuffers[1]);
							}
							else
							{
								processForSourceState(
									set.m_state,
									set.m_workbuffer_sourceResult, set.m_workbuffers[0], set.m_workbuffers_delay[1], set.m_workbuffer_3dDSPResult);
								processForSourceState(
									set.m_statePrevious,
									set.m_workbuffer_sourceResult, set.m_workbuffers[0], set.m_workbuffers_delay[0], set.m_workbuffers[1]);
							}

							// Crossfade the old state with the new state.
							audio::mixing::Crossfade<kWorkbufferSize>(set.m_workbuffers[1].m_data_left, set.m_workbuffer_3dDSPResult.m_data_left, set.m_workbuffer_3dDSPResult.m_data_left);
							audio::mixing::Crossfade<kWorkbufferSize>(set.m_workbuffers[1].m_data_right, set.m_workbuffer_3dDSPResult.m_data_right, set.m_workbuffer_3dDSPResult.m_data_right);

							// Go to the next step
							set.m_delay_step = (set.m_delay_step + 1) % 2;

							// Save previous state so we can blend to next sample set
							set.m_statePrevious = set.m_state; 
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

						audio::mixing::Acculmulate<kWorkbufferSize>(set.m_workbuffer_3dDSPResult.m_data_left, workbuffer_channel.m_data_left);
						audio::mixing::Acculmulate<kWorkbufferSize>(set.m_workbuffer_3dDSPResult.m_data_right, workbuffer_channel.m_data_right);

						// Done mixing here
						set.m_newmix = false;
					}
				}
				// Scale channels & acculmulate channels data into the main buffer
				std::fill(workbuffer_out, workbuffer_out + kWorkbufferSize * channelCount, 0.0F);
				for (uint32_t mixchannel = 0; mixchannel < (uint32_t)MixChannel::kMAX_COUNT; ++mixchannel)
				{
					// let's do some fucky dsp 
					// This actually works! We just have too much other noise in our mixing still...
					/*if (mixchannel == (uint32_t)MixChannel::kMusic)
					{
						audio::mixing::FFT<kWorkbufferSize>(workbuffer_channel_out[mixchannel].m_data_left, workbuffer_fft_real->m_data_left, workbuffer_fft_imag->m_data_left);
						audio::mixing::FFT<kWorkbufferSize>(workbuffer_channel_out[mixchannel].m_data_right, workbuffer_fft_real->m_data_right, workbuffer_fft_imag->m_data_right);

						// let's fade out the back end of it
						for (uint32_t i = 0; i < kWorkbufferSize; ++i)
						{
							float percent = i / (float)(kWorkbufferSize - 1);

							percent = pow(1.0 - percent, 100);

							workbuffer_fft_real->m_data_left[i] *= percent;
							workbuffer_fft_real->m_data_right[i] *= percent;
							workbuffer_fft_imag->m_data_left[i] *= percent;
							workbuffer_fft_imag->m_data_right[i] *= percent;
						}

						//audio::mixing::InverseFFT<kWorkbufferSize>(workbuffer_fft_real->m_data_left, workbuffer_fft_imag->m_data_left, workbuffer_channel_out[mixchannel].m_data_left);
						//audio::mixing::InverseFFT<kWorkbufferSize>(workbuffer_fft_real->m_data_right, workbuffer_fft_imag->m_data_right, workbuffer_channel_out[mixchannel].m_data_right);
					}*/

					audio::mixing::ChannelsToInterleavedStereo<kWorkbufferSize>(workbuffer_channel_out[mixchannel].m_data_left, workbuffer_channel_out[mixchannel].m_data_right, workbuffer_mix);
					audio::mixing::Scale<kWorkbufferSize * 2>(workbuffer_mix, mixObjectState.m_channelGain[mixchannel]);
					audio::mixing::Acculmulate<kWorkbufferSize * 2>(workbuffer_mix, workbuffer_out);
				}

				audio::mixing::Saturate<kWorkbufferSize * 2>(workbuffer_out);

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
		delete workbuffer_fft_real;
		delete workbuffer_fft_imag;
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