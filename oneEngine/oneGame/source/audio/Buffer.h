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
#include "audio/AudioMaster.h"
#include "audio/types/handles.h"

namespace audio
{
	class BufferManager;

	class Buffer : public arBaseObject
	{
	private:
		friend BufferManager;

	public:
		AUDIO_API explicit	Buffer ( const char* filename, const int channel_count = 1 );
		AUDIO_API explicit	Buffer ( void );
		AUDIO_API virtual	~Buffer ( void );

		arBufferHandle	GetBuffer ( void )
			{ return m_sound; }

		bool			IsStreamed ( void )
			{ return m_streamed; }
		bool			IsPositional ( void )
			{ return m_positional; }

		virtual double	GetLength ( void );

	protected:
		bool			m_streamed;
		bool			m_positional;
		arBufferHandle	m_sound;
	
		void Init ( const char* filename );
		void Free ( void );
	};	
}

#endif//AUDIO_SOUND_H_