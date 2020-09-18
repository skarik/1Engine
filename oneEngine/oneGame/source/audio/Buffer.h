//===============================================================================================//
//
//		class audio::Buffer
//
//	Contains audio data. Can be referenced by multiple audio sources.
//	Due to limitations of many sound engines, a buffer can only be used in either 2D or 3D mode.
//	In order to use the same sound in both 2D and 3D mode, the sound must be loaded twice.
//
//	Please use BufferManager or AudioMaster to load buffers with proper management.
//
//===============================================================================================//
#ifndef AUDIO_SOUND_H_
#define AUDIO_SOUND_H_

#include "core/types/arBaseObject.h"
#include "audio/types/Formats.h"
#include "audio/types/handles.h"
#include <atomic>

namespace audio
{
	class BufferManager;

	class Buffer : public arBaseObject
	{
	private:
		friend BufferManager;

	public:
		AUDIO_API explicit		Buffer ( const char* filename );
		AUDIO_API explicit		Buffer ( void );
		AUDIO_API virtual		~Buffer ( void );

		//	IsStreamed() : is this a streamed sound?
		// Streamed audio needs slightly different sampling handling on the audio::Source end.
		AUDIO_API bool			IsStreamed ( void ) const
			{ return m_streamed; }

		//	GetSampleLength() : returns length of the audio buffer, in samples
		AUDIO_API virtual uint32_t
								GetSampleLength ( void ) const;
		//	GetSampleRate() : returns sample rate of the audio buffer
		AUDIO_API virtual uint32_t
								GetSampleRate ( void ) const;
		//	GetLength() : return length of the audio buffer, in seconds
		AUDIO_API virtual double
								GetLength ( void ) const;
		//	GetReadyToSample() : return if the sample is loaded and ready to sample
		AUDIO_API virtual bool
								GetReadyToSample ( void ) const;

		//	GetChannelCount() : return number of channels the audio buffer has
		AUDIO_API ChannelCount	GetChannelCount ( void ) const;

		virtual void			Sample ( uint32_t& inout_sample_position, uint32_t sample_count, float* sample_output );

	protected:
		bool				m_streamed = false;
		arBufferHandle		m_sound = NULL;

		std::atomic_bool
							m_readyToSample = false;
		std::atomic_uint64_t
							m_framesAndSampleRate_Packed = 0;
	
		void					Init ( const char* filename );
		void					Free ( void );
	};	
}

#endif//AUDIO_SOUND_H_