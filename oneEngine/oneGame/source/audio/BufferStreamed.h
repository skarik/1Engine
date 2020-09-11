#ifndef AUDIO_BUFFER_STREAMED_H_
#define AUDIO_BUFFER_STREAMED_H_

#include "audio/Buffer.h"

#include "ogg/ogg.h"
#include "vorbis/codec.h"
#include "vorbis/vorbisenc.h"
#include "vorbis/vorbisfile.h"

namespace audio
{
	class BufferStreamed : public Buffer
	{
	private:
		explicit BufferStreamed ( const char* filename, const int );
				~BufferStreamed ( void );

		friend	BufferManager;

	public:
	#ifndef _AUDIO_FMOD_
		bool Sample ( arSourceHandle source, double& rawtime, bool loop = false );
		bool Stream ( arBufferHandle buffer, double& rawtime, bool loop = false );

		void FreeBuffers ( arSourceHandle source );

		virtual double GetLength ( void );

		arBufferHandle	m_buffers [8];
		bool			m_buffer_usage [8];
	#endif

	protected:
	#ifndef _AUDIO_FMOD_
		FILE*			m_file;
		OggVorbis_File	m_oggStream;
		vorbis_info*	m_vorbisInfo;
		vorbis_comment*	m_vorbisComment;

		uint32_t		m_format;
	#endif

		void InitStream ( const char* filename );
		void FreeStream ( void );
	};
}

#endif//AUDIO_BUFFER_STREAMED_H_