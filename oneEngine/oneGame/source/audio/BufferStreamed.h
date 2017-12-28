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
		bool Sample ( ALuint source, double& rawtime, bool loop = false );
		bool Stream ( ALuint buffer, double& rawtime, bool loop = false );

		void FreeBuffers ( ALuint source );

		virtual double GetLength ( void );

		ALuint buffers [8];
		bool buffer_usage [8];
	#endif

	protected:
	#ifndef _AUDIO_FMOD_
		FILE*			oggFile;
		OggVorbis_File	oggStream;
		vorbis_info*	vorbisInfo;
		vorbis_comment*	vorbisComment;
		ALenum			format;
	#endif

		void InitStream ( const char* filename );
		void FreeStream ( void );
	};
}

#endif//AUDIO_BUFFER_STREAMED_H_