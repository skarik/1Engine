#ifndef AUDIO_AUDIOTHREAD_WASAPI_H_
#define AUDIO_AUDIOTHREAD_WASAPI_H_

#include "core/types.h"
#include <thread>
#include <atomic>
#include "AudioBackend.h"

#ifdef PLATFORM_WINDOWS
namespace audio
{
	class WASAPIAudioWorkerThread;
	class WASAPIAudioBackend : public AudioBackend
	{
	public:
		WASAPIAudioBackend();
		~WASAPIAudioBackend();

		virtual void			Start ( void ) override;
		virtual void			Stop ( void ) override;

		virtual int32_t			SampleRate ( void ) override;
		virtual int32_t			ChannelCount ( void ) override;

		virtual uint32_t		AvailableFrames ( void ) override;
		virtual void			SubmitFrames ( uint32_t frameCount, float* pcmFrames ) override;

	public: // Interfaces specific for WASAPIAudioWorkerThread

		FORCE_INLINE uint32_t	GetBufferLength_ThreadSafe ( void );
		FORCE_INLINE float*		GetBuffer_ThreadSafe ( void );
		FORCE_INLINE uint32_t	GetBufferSize ( void );
		FORCE_INLINE uint32_t	GetBufferStart ( void );
		FORCE_INLINE void		SetBufferCursor_ThreadSafe ( uint32_t oldLength, uint32_t newLength, uint32_t newStart );

	private:
		std::thread				m_thread;
		WASAPIAudioWorkerThread*
								m_worker = NULL;

		// Ring-buffer of audio data
		float*					m_audioBuffer = NULL;
		// Length of the ring-buffer
		uint32_t				m_audioBufferSize = 0;
		// Current start position of the audio buffer: the first frame of unsent data.
		// It is only ever read and written by the worker thread.
		uint32_t				m_audioBufferStart = 0; 
		// Current length of the audio buffer: start + length = the last frame of unsent data.
		// It is written by both the submitting and working thread.
		std::atomic_uint32_t	m_audioBufferLength_ThreadSafe = 0;
		// Current end position of the audio buffer: the last frame of unsent data.
		// It is only ever written by the submitting thread.
		uint32_t				m_audioBufferEnd = 0;

		
	};
}
#endif//PLATFORM_WINDOWS

#endif//AUDIO_AUDIOTHREAD_WASAPI_H_