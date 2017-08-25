
#include "CAudioSoundLoader.h"

#ifndef _AUDIO_FMOD_

#include "ogg/ogg.h"
#include "vorbis/codec.h"
#include "vorbis/vorbisenc.h"
#include "vorbis/vorbisfile.h"


ALuint CAudioSoundLoader::LoadOGG ( const char* sFilename )
{
	return Error( "Could not load OGG file" );
}

#endif