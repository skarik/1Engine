#include "WaveformLoader.h"

#include "ogg/ogg.h"
#include "vorbis/codec.h"
#include "vorbis/vorbisenc.h"
#include "vorbis/vorbisfile.h"

ALuint audio::WaveformLoader::LoadOGG ( const char* sFilename )
{
	return Error( "Could not load OGG file" );
}
