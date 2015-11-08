// class CAudioSoundStreamed
//  for streaming OGG files


#ifndef _C_AUDIO_SOUND_STREAMED_H_
#define _C_AUDIO_SOUND_STREAMED_H_

#include "CAudioSound.h"

#include "ogg/ogg.h"
#include "vorbis/codec.h"
#include "vorbis/vorbisenc.h"
#include "vorbis/vorbisfile.h"

class CAudioSoundStreamed : public CAudioSound
{
private:
	explicit CAudioSoundStreamed ( const std::string&, const int );
			~CAudioSoundStreamed ( void );

	friend CSoundManager;

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

	void InitStream ( const std::string& );
	void FreeStream ( void );
};

#endif