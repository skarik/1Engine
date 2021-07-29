#include "core/os.h"
#ifdef PLATFORM_WINDOWS

#include "WASAPIAudioBackend.h"
#include "core/debug.h"
#include "core/mem.h"
#include "core/debug/console.h"
#include "core/math/Math.h"
#include <atomic>
#include <algorithm>
#include <mutex>

// Allows to enable use of the clock adjust. Shouldn't be needed, should just match your device and roll hard.
#define WASAPI_CONFIG_USE_CLOCK_ADJUST 0

// WASAPI & COM includes
#include <mmdeviceapi.h>
#include <Audioclient.h>
#include <windows.h>

// In WASAPI, each "RefTime" is 100 nanoseconds. Instead of dealing with that, we just define some easy constants.
static constexpr long long	kAudioRefTimes_Per_Second = 10000000;
static constexpr long long	kAudioRefTimes_Per_Millisecond = 10000;

// We want to define out buffer sizes based on the amount of latency we're willing to have.
static constexpr long long	kAudioMaxLatency_Milliseconds = 2;
static constexpr long long	kAudioBufferLength = kAudioMaxLatency_Milliseconds * kAudioRefTimes_Per_Millisecond;

static constexpr unsigned long kAudioDefaultSampleRate = 48000;

namespace audio
{
	class WASAPIAudioWorkerThread;

	// Notifier class. Used to track when the default audio device changes.
	class AudioChangedNotifier : public IMMNotificationClient 
	{
	public:
		WASAPIAudioWorkerThread*
								m_owner;

		AudioChangedNotifier ( WASAPIAudioWorkerThread* owner );

		// IUnknown implementation:
		ULONG STDMETHODCALLTYPE	AddRef()
		{
			return 1;
		}

		ULONG STDMETHODCALLTYPE	Release()
		{
			return 1;
		}

		HRESULT STDMETHODCALLTYPE
								QueryInterface(REFIID riid, VOID **ppvInterface)
		{
			if (IID_IUnknown == riid)
			{
				AddRef();
				*ppvInterface = (IUnknown*)this;
			}
			else if (__uuidof(IMMNotificationClient) == riid)
			{
				AddRef();
				*ppvInterface = (IMMNotificationClient*)this;
			}
			else
			{
				*ppvInterface = NULL;
				return E_NOINTERFACE;
			}
			return S_OK;
		}

		// IMMNotificationClient implementation:

		HRESULT STDMETHODCALLTYPE
								OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR pwstrDeviceId);
		HRESULT STDMETHODCALLTYPE
								OnDeviceAdded(LPCWSTR pwstrDeviceId);
		HRESULT STDMETHODCALLTYPE
								OnDeviceRemoved(LPCWSTR pwstrDeviceId);
		HRESULT STDMETHODCALLTYPE
								OnDeviceStateChanged(LPCWSTR pwstrDeviceId, DWORD dwNewState);
		HRESULT STDMETHODCALLTYPE
								OnPropertyValueChanged(LPCWSTR pwstrDeviceId, const PROPERTYKEY key);
	};

	class WASAPIAudioWorkerThread
	{
	public:
		WASAPIAudioBackend*		m_bufferLayer = NULL;
		std::atomic_bool		m_isReady = false;
		std::atomic_bool		m_continueRun = true;
		std::atomic_bool		m_requestNewDevice = false;

		IMMDeviceEnumerator*	m_deviceEnumerator = NULL;
		IMMDevice*				m_device = NULL;
		IAudioClient*			m_audioClient = NULL;
		IAudioRenderClient*		m_renderClient = NULL;
#	if WASAPI_CONFIG_USE_CLOCK_ADJUST
		IAudioClockAdjustment*	m_audioClockAdjuster = NULL;
#	endif

		AudioChangedNotifier*	m_audioChangedNotifier = NULL;

		// Event used to notify when the audio buffer has emptied.
		HANDLE					m_processEvent = NULL;

		int						m_channelCount = 2;
		int						m_sampleRate = 48000;
		int						m_bitsPerSample = 16;
		uint32_t				m_bufferFrameSize = 0;

		//	InitializeAudio() : Creates the WASAPI items that are used to recreate the audio device.
		// These are created once and reused between different devices.
		void					InitializeAudio ( void )
		{
			// Set up WASAPI system
			HRESULT hr;

			// Initialize COM for this thread...
			CoInitialize(NULL);

			// Need the COM MM Enumerator
			hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&m_deviceEnumerator);
			if (hr != S_OK || m_deviceEnumerator == NULL)
			{
				ARCORE_ERROR("BAD");
			}

			// Set up the callback for the enumerator now
			hr = m_deviceEnumerator->RegisterEndpointNotificationCallback(m_audioChangedNotifier);
			if (hr != S_OK)
			{
				ARCORE_ERROR("BAD");
			}

			// Create a process event
			m_processEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

			// And we're ready!
			m_isReady = true;
		}
		
		//	RecreateDevice() : Created audio device on the current device enumerator.
		// Will free up already-created devices and clients, and start fresh.
		void					RecreateDevice ( void )
		{
			HRESULT hr;

			// Free existing audio handles
			if (m_renderClient != NULL)
			{
				m_renderClient->Release();
				m_renderClient = NULL;
			}
#		if WASAPI_CONFIG_USE_CLOCK_ADJUST
			if (m_audioClockAdjuster != NULL)
			{
				m_audioClockAdjuster->Release();
				m_audioClockAdjuster = NULL;
			}
#		endif
			if (m_audioClient != NULL)
			{
				m_audioClient->Stop();
				m_audioClient->Release();
				m_audioClient = NULL;
			}
			if (m_device != NULL)
			{
				m_device->Release();
				m_device = NULL;
			}

			debug::Console->PrintMessage("Creating new audio device.\n");

			// We need to get the current default device. If we're recreating, this can change
			hr = m_deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &m_device);
			if (hr != S_OK || m_device == NULL)
			{
				ARCORE_ERROR("BAD");
			}

			// Activate the device & get the client....
			hr = m_device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&m_audioClient);
			if (hr != S_OK || m_audioClient == NULL)
			{
				ARCORE_ERROR("BAD");
			}

			// Get the current desired mix format
			WAVEFORMATEX* pwfx = NULL;
			m_audioClient->GetMixFormat(&pwfx);
			//pwfx->wFormatTag = WAVE_FORMAT_PCM;
			pwfx->nSamplesPerSec = kAudioDefaultSampleRate;
			pwfx->nChannels = 2;
			pwfx->wBitsPerSample = 32;
			pwfx->nAvgBytesPerSec = pwfx->nSamplesPerSec * (pwfx->wBitsPerSample / 8) * pwfx->nChannels;

			// Set up the client...
			hr = m_audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_RATEADJUST /*| AUDCLNT_STREAMFLAGS_EVENTCALLBACK*/, kAudioBufferLength, 0, pwfx, NULL);
			if (hr == AUDCLNT_E_UNSUPPORTED_FORMAT)
			{	// Fallback to defaults
				m_audioClient->GetMixFormat(&pwfx);
				hr = m_audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_RATEADJUST /*| AUDCLNT_STREAMFLAGS_EVENTCALLBACK*/, kAudioBufferLength, 0, pwfx, NULL);
			}
			if (hr != S_OK)
			{
				ARCORE_ERROR("BAD");
			}

#		if WASAPI_CONFIG_USE_CLOCK_ADJUST
			m_audioClient->GetService(__uuidof(IAudioClockAdjustment), (void**)&m_audioClockAdjuster);
#		endif

			// Save current sample rate & channel count:
			m_channelCount = pwfx->nChannels;
			m_sampleRate = pwfx->nSamplesPerSec;
			m_bitsPerSample = pwfx->wBitsPerSample;

			// We don't support higher channel count just yet so let's assert for now (we just want to fill in channels 0 and 1 in future)
			ARCORE_ASSERT(m_channelCount <= 2);

			// Also get the size of the buffer
			hr = m_audioClient->GetBufferSize(&m_bufferFrameSize);
			if (hr != S_OK)
			{
				ARCORE_ERROR("BAD");
			}

#		if WASAPI_CONFIG_USE_CLOCK_ADJUST
			// Force the sample rate
			m_audioClockAdjuster->SetSampleRate((float)m_sampleRate);
#		endif

			// Make the client use the event
			m_audioClient->SetEventHandle(m_processEvent);

			// Get the render client used for generating data
			hr = m_audioClient->GetService(__uuidof(IAudioRenderClient), (void**)&m_renderClient);
			if (hr != S_OK)
			{
				ARCORE_ERROR("BAD");
			}

			// Buffer with empty data for the first pass
			{
				BYTE* next_buffer = NULL;
				m_renderClient->GetBuffer(m_bufferFrameSize, &next_buffer);
				// Clear out the buffer
				memset(next_buffer, 0, m_bufferFrameSize * m_channelCount * (m_bitsPerSample / 8));
				// Submit the buffer
				m_renderClient->ReleaseBuffer(m_bufferFrameSize, 0);
			}

			// Kick the audio on!
			m_audioClient->Start();
		}

		//	Run() : Main audio manager & loop
		void					Run ( void )
		{
			// Create notifier instance before starting audio systems
			m_audioChangedNotifier = new AudioChangedNotifier(this);
			m_audioChangedNotifier->AddRef();

			InitializeAudio();
			RecreateDevice();

			while (m_continueRun)
			{
				if (m_requestNewDevice)
				{
					RecreateDevice();
					m_requestNewDevice = false;
				}

				//// Wait for next buffer event to be signaled.
				//DWORD retval = WaitForSingleObject(m_processEvent, 2000);
				//if (retval != WAIT_OBJECT_0)
				//{
				//	// Event handle timed out, recreate device:
				//	m_requestNewDevice = true;
				//	debug::Console->PrintMessage("Audio client timed out.\n");
				//	continue;
				//}

				// Get the number of frames available to fill:
				uint32_t bufferFramePadding = m_bufferFrameSize;
				m_audioClient->GetCurrentPadding(&bufferFramePadding);
				uint32_t framesAvailableToWrite = m_bufferFrameSize - bufferFramePadding;
				// No space to keep going, skip for now.
				if (framesAvailableToWrite == 0)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(kAudioMaxLatency_Milliseconds - 1));
					continue;
				}

				// Get the number of frames available to upload:
				uint32_t framesAvailableToUpload = m_bufferLayer->GetBufferLength_ThreadSafe();
				// No data to upload, skip for now.
				if (framesAvailableToUpload == 0)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(kAudioMaxLatency_Milliseconds));
					continue;
				}

				// Take the lower number for items to upload
				uint32_t framesAvailable = std::min(framesAvailableToWrite, framesAvailableToUpload);

				BYTE* next_buffer = NULL;
				m_renderClient->GetBuffer(framesAvailable, &next_buffer);
				if (next_buffer != NULL)
				{
					/*static double time = 0.0;

					// Fill up the buffer
					for (uint32_t i = 0; i < framesAvailable; ++i)
					{
						time += 1.0 / m_sampleRate;

						float amplitude = (float)sin(time * 2 * M_PI * 440) * 0.2f;

						// apply to both channels
						if (m_bitsPerSample == 32)
						{
							float* next_buffer_float = (float*)next_buffer;

							if (m_channelCount == 1)
							{
								next_buffer_float[i] = amplitude;
							}
							else if (m_channelCount == 2)
							{
								next_buffer_float[i * 2 + 0] = amplitude;
								next_buffer_float[i * 2 + 1] = next_buffer_float[i * 2 + 0];
							}
						}
					}*/

					// Fill up the buffer with items from the ring buffer
					float* ringBuffer = m_bufferLayer->GetBuffer_ThreadSafe();
					uint32_t ringBufferSize = m_bufferLayer->GetBufferSize();
					uint32_t ringBufferLength = framesAvailableToUpload;
					uint32_t ringBufferStart = m_bufferLayer->GetBufferStart();
					float* next_buffer_float = (float*)next_buffer;

					switch (m_channelCount)
					{
						// Mono:
					case 1:
						{
							uint32_t ringBufferNewStart = ringBufferStart + framesAvailable;
							if (ringBufferNewStart <= ringBufferSize)
							{
								std::copy(&ringBuffer[ringBufferStart], &ringBuffer[ringBufferNewStart], next_buffer_float);
								ringBufferNewStart = ringBufferNewStart % ringBufferSize;
							}
							else
							{	// Wrapping around
								ringBufferNewStart = ringBufferNewStart % ringBufferSize;
								std::copy(&ringBuffer[ringBufferStart], &ringBuffer[ringBufferSize], next_buffer_float);
								std::copy(&ringBuffer[0], &ringBuffer[ringBufferNewStart], next_buffer_float + (ringBufferSize - ringBufferStart));
							}
							ringBufferStart = ringBufferNewStart;
						}
						break;

						// Stereo:
					case 2:
						{
							uint32_t ringBufferNewStart = ringBufferStart + framesAvailable;
							if (ringBufferNewStart <= ringBufferSize)
							{
								std::copy(&ringBuffer[ringBufferStart * 2], &ringBuffer[ringBufferNewStart * 2], next_buffer_float);
								ringBufferNewStart = ringBufferNewStart % ringBufferSize;
							}
							else
							{	// Wrapping around
								ringBufferNewStart = ringBufferNewStart % ringBufferSize;
								std::copy(&ringBuffer[ringBufferStart * 2], &ringBuffer[ringBufferSize * 2], next_buffer_float);
								std::copy(&ringBuffer[0], &ringBuffer[ringBufferNewStart * 2], next_buffer_float + (ringBufferSize - ringBufferStart) * 2);
							}
							ringBufferStart = ringBufferNewStart;
						}
						break;

					default:
						// Higher is not supported at the moment:
						ARCORE_ERROR("More than 2 channels of audio not yet supported.");
					}

					// Set the new ringbuffer state at the end.
					ringBufferLength -= framesAvailable;
					m_bufferLayer->SetBufferCursor_ThreadSafe(framesAvailableToUpload, ringBufferLength, ringBufferStart);

					// Submit the buffer
					m_renderClient->ReleaseBuffer(framesAvailable, 0);
				}
			}
			// End of the work loop, time to clean up...

			// Free audio systems
			m_renderClient->Release();
			m_renderClient = NULL;

			m_audioClient->Stop();
			m_audioClient->Release();
			m_audioClient = NULL;
			
			m_device->Release();
			m_device = NULL;
			
			m_deviceEnumerator->Release();
			m_deviceEnumerator = NULL;

			// Clean up notifier instance
			//delete_safe(m_audioChangedNotifier);
			m_audioChangedNotifier->Release();
		}
	};


	AudioChangedNotifier::AudioChangedNotifier ( WASAPIAudioWorkerThread* owner )
	{
		m_owner = owner;
	}

	HRESULT STDMETHODCALLTYPE
	AudioChangedNotifier::OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR pwstrDeviceId)
	{
		char  *pszFlow = "?????";
		char  *pszRole = "?????";

		switch (flow)
		{
		case eRender:
			pszFlow = "eRender";
			break;
		case eCapture:
			pszFlow = "eCapture";
			break;
		}

		switch (role)
		{
		case eConsole:
			pszRole = "eConsole";
			break;
		case eMultimedia:
			pszRole = "eMultimedia";
			break;
		case eCommunications:
			pszRole = "eCommunications";
			break;
		}

		printf("  -->New default device: flow = %s, role = %s\n", pszFlow, pszRole);

		// Mark to owner that we want a new device
		m_owner->m_requestNewDevice = true;

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE AudioChangedNotifier::OnDeviceAdded(LPCWSTR pwstrDeviceId)
	{
		printf("  -->Added device\n");
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE AudioChangedNotifier::OnDeviceRemoved(LPCWSTR pwstrDeviceId)
	{
		printf("  -->Removed device\n");
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE AudioChangedNotifier::OnDeviceStateChanged(LPCWSTR pwstrDeviceId, DWORD dwNewState)
	{
		char  *pszState = "?????";

		switch (dwNewState)
		{
		case DEVICE_STATE_ACTIVE:
			pszState = "ACTIVE";
			break;
		case DEVICE_STATE_DISABLED:
			pszState = "DISABLED";
			break;
		case DEVICE_STATE_NOTPRESENT:
			pszState = "NOTPRESENT";
			break;
		case DEVICE_STATE_UNPLUGGED:
			pszState = "UNPLUGGED";
			break;
		}

		printf("  -->New device state is DEVICE_STATE_%s (0x%8.8x)\n", pszState, dwNewState);

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE AudioChangedNotifier::OnPropertyValueChanged(LPCWSTR pwstrDeviceId, const PROPERTYKEY key)
	{
		/*printf("  -->Changed device property "
				"{%8.8x-%4.4x-%4.4x-%2.2x%2.2x-%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x}#%d\n",
				key.fmtid.Data1, key.fmtid.Data2, key.fmtid.Data3,
				key.fmtid.Data4[0], key.fmtid.Data4[1],
				key.fmtid.Data4[2], key.fmtid.Data4[3],
				key.fmtid.Data4[4], key.fmtid.Data4[5],
				key.fmtid.Data4[6], key.fmtid.Data4[7],
				key.pid);*/
		return S_OK;
	}
}

audio::WASAPIAudioBackend::WASAPIAudioBackend ( void )
{
	;
}
audio::WASAPIAudioBackend::~WASAPIAudioBackend ( void )
{
	if (m_worker != NULL)
	{
		Stop();
	}
}

void audio::WASAPIAudioBackend::Start ( void )
{
	// Start up the WASAPI worker
	m_worker = new WASAPIAudioWorkerThread();
	m_worker->m_bufferLayer = this;
	m_thread = std::thread( [&]{ m_worker->Run(); } );

	// Stall until it's ready so can shortcut later in mixer...
	while (!m_worker->m_isReady) 
	{
		std::this_thread::yield();
	}

	// Allocate buffer for the data to send in
	m_audioBufferSize = std::max<uint32_t>(1024 * 4, 5 * (kAudioDefaultSampleRate / 1000)); // 5 ms of data to buffer in.
	m_audioBuffer = new float[m_audioBufferSize * ChannelCount()];
}

void audio::WASAPIAudioBackend::Stop ( void )
{
	m_worker->m_continueRun = false;
	
	m_thread.join();

	delete m_worker;
	m_worker = NULL;

	delete[] m_audioBuffer;
}

int32_t audio::WASAPIAudioBackend::SampleRate ( void )
{
	return m_worker->m_sampleRate;
}
int32_t audio::WASAPIAudioBackend::ChannelCount ( void ) 
{
	return m_worker->m_channelCount;
}

uint32_t audio::WASAPIAudioBackend::AvailableFrames ( void ) 
{
	return m_audioBufferSize - m_audioBufferLength_ThreadSafe;
}

void audio::WASAPIAudioBackend::SubmitFrames ( uint32_t frameCount, float* pcmFrames )
{
	ARCORE_ASSERT_MSG(frameCount <= (m_audioBufferSize - m_audioBufferLength_ThreadSafe), "frameCount was more than the space available in the buffer.");

	switch (ChannelCount())
	{
		// Mono:
	case 1:
		for (uint32_t frame = 0; frame < frameCount; ++frame)
		{
			m_audioBuffer[m_audioBufferEnd] = pcmFrames[frame];

			// Increment by one frame
			m_audioBufferEnd = (m_audioBufferEnd + 1) % m_audioBufferSize;
		}
		break;

		// Stereo:
	case 2:
		for (uint32_t frame = 0; frame < frameCount; ++frame)
		{
			m_audioBuffer[m_audioBufferEnd * 2 + 0] = pcmFrames[frame * 2 + 0];
			m_audioBuffer[m_audioBufferEnd * 2 + 1] = pcmFrames[frame * 2 + 1];

			// Increment by one frame
			m_audioBufferEnd = (m_audioBufferEnd + 1) % m_audioBufferSize;
		}
		break;

	default:
		// Higher is not supported at the moment:
		ARCORE_ERROR("More than 2 channels of audio not yet supported.");
	}

	// Set the buffer length after the data is added
	m_audioBufferLength_ThreadSafe += frameCount;
}

uint32_t audio::WASAPIAudioBackend::GetBufferLength_ThreadSafe ( void )
{
	return m_audioBufferLength_ThreadSafe;
}

float* audio::WASAPIAudioBackend::GetBuffer_ThreadSafe ( void )
{
	return m_audioBuffer;
}

uint32_t audio::WASAPIAudioBackend::GetBufferSize ( void )
{
	return m_audioBufferSize;
}

uint32_t audio::WASAPIAudioBackend::GetBufferStart ( void )
{
	return m_audioBufferStart;
}

void audio::WASAPIAudioBackend::SetBufferCursor_ThreadSafe ( uint32_t oldLength, uint32_t newLength, uint32_t newStart )
{
	m_audioBufferStart = newStart;
	m_audioBufferLength_ThreadSafe -= oldLength - newLength;
}
#endif//PLATFORM_WINDOWS