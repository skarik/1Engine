
#include "CAudioSoundStreamed.h"
#include "CAudioSoundLoader.h"

#include "core/debug/console.h"

using std::string;

CAudioSoundStreamed::CAudioSoundStreamed ( const string& sFileName, const int nPositional )
	: CAudioSound()
{
	positional = (nPositional>0);
#ifndef _AUDIO_FMOD_
	for ( int i = 0; i < 8; ++i ) {
		buffers[i] = 0;
	}
#endif
	streamed = true;
	InitStream( sFileName );
}

CAudioSoundStreamed::~CAudioSoundStreamed ( void )
{
	FreeStream();
}

string errorString(int code)
{
    switch(code)
    {
        case OV_EREAD:
            return string("Read from media.");
        case OV_ENOTVORBIS:
            return string("Not Vorbis data.");
        case OV_EVERSION:
            return string("Vorbis version mismatch.");
        case OV_EBADHEADER:
            return string("Invalid Vorbis header.");
        case OV_EFAULT:
            return string("Internal logic fault (bug or heap/stack corruption.");
        default:
            return string("Unknown Ogg error.");
    }
}

void CAudioSoundStreamed::InitStream ( const string& sFileName )
{
#ifndef _AUDIO_FMOD_
	int result;

	// Open OGG file
	oggFile = fopen( sFileName.c_str(), "rb" );
	if ( !oggFile ) {
		throw string("Could not open Ogg file.");
	}

	// Load file into vorbis loader
	result = ov_open( oggFile, &oggStream, NULL, 0 ); /// CRASHES. REBUILD OGG LIBARY. ///
	if ( result < 0 )
	{
		fclose( oggFile );
		throw string("Could not open Ogg stream. ") + errorString(result);
	}

	// Get vorbis info and comment blocks
	vorbisInfo = ov_info(&oggStream, -1);
    vorbisComment = ov_comment(&oggStream, -1);
	// Get channel info from vorbis info block
    if ( vorbisInfo->channels == 1 )
        format = AL_FORMAT_MONO16;
    else
        format = AL_FORMAT_STEREO16;

	// Create buffers
	alGenBuffers( 8, buffers );
	for ( int i = 0; i < 8; ++i ) {
		buffer_usage[i] = false;
	}
#else
	FMOD::FMOD_RESULT result = FMOD::FMOD_OK;

	if ( positional ) {
		result = FMOD::FMOD_System_CreateStream( CAudioMaster::System(), sFileName.c_str(), FMOD_DEFAULT | FMOD_3D, 0, &m_sound );
	}
	else {
		result = FMOD::FMOD_System_CreateStream( CAudioMaster::System(), sFileName.c_str(), FMOD_DEFAULT /*| FMOD_3D*/, 0, &m_sound );
	}

	// Check to see if it loaded properly
	if ( result != FMOD::FMOD_OK )
	{
		debug::Console->PrintError("FMOD could not open the file \"" + sFileName + "\"");
	}

	streamed = true;
#endif
}

void CAudioSoundStreamed::FreeStream ( void )
{
#ifndef _AUDIO_FMOD_
    alDeleteBuffers( 8, buffers );
    ov_clear( &oggStream ); // Also will close the FILE handle
#else
	FMOD::FMOD_Sound_Release( m_sound );
#endif
}

#ifndef _AUDIO_FMOD_
void CAudioSoundStreamed::FreeBuffers ( ALuint source )
{
	ALuint buffer;
	ALint queued;
    
	// Get amount of buffers queued on this source
    alGetSourcei( source, AL_BUFFERS_QUEUED, &queued );
    
    while ( queued-- ) // Dequeue all buffers on this source
    {
        alSourceUnqueueBuffers( source, 1, &buffer );
		// Check for that buffer in the buffer list
		for ( int i = 0; i < 8; ++i ) {
			if ( buffer == buffers[i] ) {
				buffer_usage[i] = false; // If it's in the list, mark it as free
			}
		}
    }
}


bool CAudioSoundStreamed::Sample ( ALuint source, double& rawtime, bool loop )
{
    ALint processed_count;
    ALuint buffer;
    bool active = true;

	// Get amount of buffers that have been used
    alGetSourcei( source, AL_BUFFERS_PROCESSED, &processed_count );
 
	// Pop off all those buffers and stream new data into them
    while( processed_count-- )
    {
        alSourceUnqueueBuffers(source, 1, &buffer); // Unqueue dead buffer
        active = Stream( buffer, rawtime, loop );  // Perform vodoo magic
        alSourceQueueBuffers(source, 1, &buffer); // Queue up resurrect buffer
    }
	
    return active;
}

bool CAudioSoundStreamed::Stream ( ALuint buffer, double& rawtime, bool loop )
{
#define BUFFER_SIZE (4096 * 16)

    char data[BUFFER_SIZE];
    int  size = 0;
    int  section;
    int  result;

	// Set current rawtime position
	ov_time_seek( &oggStream, rawtime );

	// Read in remaining data info buffer
    while( size < BUFFER_SIZE )
    {
        result = ov_read( &oggStream, data + size, BUFFER_SIZE - size, 0, 2, 1, & section );
    
		if ( result > 0 ) {
            size += result;
		}
		else {
            if ( result < 0 )
                throw errorString(result);
            else
                break;
		}
    }

	// Output new rawtime position
	rawtime = ov_time_tell( &oggStream );
	if ( loop ) {
		if ( ov_raw_tell( &oggStream )+1 >= ov_raw_total( &oggStream, -1 ) ) {
			rawtime = 0;
		}
	}
    
	// If no data read, then no longer active
	if ( size == 0 ) {
        return loop;
	}
 
    alBufferData( buffer, format, data, size, vorbisInfo->rate );
 
    return true;
}

double CAudioSoundStreamed::GetLength ( void )
{
	return ov_time_total( &oggStream, -1 );
}
#endif