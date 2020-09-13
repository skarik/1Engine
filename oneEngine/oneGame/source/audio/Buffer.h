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

		//arBufferHandle			GetBuffer ( void )
		//	{ return m_sound; }

		AUDIO_API bool			IsStreamed ( void )
			{ return m_streamed; }

		//	GetSampleLength() : returns length of the audio buffer, in samples
		AUDIO_API virtual uint32_t
								GetSampleLength ( void );
		//	GetLength() : return length of the audio buffer, in seconds
		AUDIO_API virtual double
								GetLength ( void );

		AUDIO_API ChannelCount	GetChannelCount ( void );
		//	Data() : returns the underlying data of this buffer
		//AUDIO_API virtual arBufferHandle
		//						Data ( void );

		virtual void			Sample ( uint32_t& inout_sample_position, uint32_t sample_count, float* sample_output );

	protected:
		bool				m_streamed = false;
		arBufferHandle		m_sound = NULL;

		//Format				m_format = Format::kInvalid;
		//ChannelCount		m_channels = ChannelCount::kInvalid;

		std::atomic_uint64_t
							m_framesAndSampleRate_Packed;
	
		void					Init ( const char* filename );
		void					Free ( void );
	};	
}

#endif//AUDIO_SOUND_H_