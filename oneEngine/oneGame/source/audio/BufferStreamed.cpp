#include "BufferStreamed.h"
#include "WaveformLoader.h"

#include "core/debug/console.h"

using std::string;

audio::BufferStreamed::BufferStreamed ( const char* filename, const int nPositional )
	: Buffer()
{
	m_positional = (nPositional>0);
#ifndef _AUDIO_FMOD_
	memset(m_buffers, 0, sizeof(m_buffers));
#endif
	m_streamed = true;
	InitStream( filename );
}

audio::BufferStreamed::~BufferStreamed ( void )
{
	FreeStream();
}

#ifndef _AUDIO_FMOD_
static const char* VorbisErrorString(int code)
{
    switch(code)
    {
        case OV_EREAD:
            return "Read from media.";
        case OV_ENOTVORBIS:
            return "Not Vorbis data.";
        case OV_EVERSION:
            return "Vorbis version mismatch.";
        case OV_EBADHEADER:
            return "Invalid Vorbis header.";
        case OV_EFAULT:
            return "Internal logic fault (bug or heap/stack corruption.";
        default:
            return "Unknown Ogg error.";
    }
}
#endif

void audio::BufferStreamed::InitStream ( const char* filename )
{
#ifndef _AUDIO_FMOD_
	int result;

	// Open OGG file
	m_file = fopen( filename, "rb" );
	if ( !m_file )
	{
		debug::Console->PrintError("Could not open Ogg file: \"%s\"\n", filename);
		return;
	}

	// Load file into vorbis loader
	result = ov_open( m_file, &m_oggStream, NULL, 0 );
	if ( result < 0 )
	{
		fclose( m_file );
		debug::Console->PrintError("Could not open Ogg stream: %s\n", VorbisErrorString(result));
		return;
	}

	// Get vorbis info and comment blocks
	m_vorbisInfo = ov_info(&m_oggStream, -1);
	m_vorbisComment = ov_comment(&m_oggStream, -1);

	// Get channel info from vorbis info block
    if ( m_vorbisInfo->channels == 1 )
		m_format = AL_FORMAT_MONO16;
    else if ( m_vorbisInfo->channels == 2 )
		m_format = AL_FORMAT_STEREO16;
	else
	{
		ov_clear(&m_oggStream);
		m_file = NULL;
		debug::Console->PrintError("Invalid channel count of `%d` on Ogg: \"%s\"\n", m_vorbisInfo->channels, filename);
	}


	// Create buffers
	alGenBuffers( 8, m_buffers );
	memset(m_buffer_usage, 0, sizeof(m_buffer_usage));

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

void audio::BufferStreamed::FreeStream ( void )
{
#ifndef _AUDIO_FMOD_
	// Free buffers
    alDeleteBuffers( 8, m_buffers );
	// Clear decoder buffer & close FILE
    ov_clear( &m_oggStream ); 
#else
	FMOD::FMOD_Sound_Release( m_sound );
#endif
}

#ifndef _AUDIO_FMOD_
void audio::BufferStreamed::FreeBuffers ( ALuint source )
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
			if ( buffer == m_buffers[i] ) {
				m_buffer_usage[i] = false; // If it's in the list, mark it as free
			}
		}
    }
}

bool audio::BufferStreamed::Sample ( arSourceHandle source, double& rawtime, bool loop )
{
    ALint processed_count;
    arBufferHandle buffer;
    bool active = true;

	// Get amount of buffers that have been used
    alGetSourcei( source, AL_BUFFERS_PROCESSED, &processed_count );
 
	// Pop off all those buffers and stream new data into them
    while ( processed_count-- )
    {
        alSourceUnqueueBuffers(source, 1, &buffer); // Unqueue dead buffer
        active = Stream( buffer, rawtime, loop );  // Perform vodoo magic
        alSourceQueueBuffers(source, 1, &buffer); // Queue up resurrect buffer
    }
	
    return active;
}

bool audio::BufferStreamed::Stream ( arBufferHandle buffer, double& rawtime, bool loop )
{
	const int kStreamBufferSize = 4096 * 16;
    char data[kStreamBufferSize];
    int size = 0;
    int out_bitstream = 0;
    int result = 0;

	// Set current rawtime position
	double start_time = ov_time_tell( &m_oggStream );
	if ( fabs(start_time - rawtime) > 0.001F )
	{
		debug::Console->PrintMessage("Time differential too large. Seeking from %f to %f.\n", start_time, rawtime);
		ov_time_seek( &m_oggStream, rawtime );
	}

	// Read in remaining data info buffer
    while ( size < kStreamBufferSize )
    {
        result = ov_read( &m_oggStream, data + size, kStreamBufferSize - size, 0, 2, 1, &out_bitstream );
    
		if ( result > 0 ) {
            size += result;
		}
		else if ( result == 0 ) {
			// We run out of data!
			if ( loop ) { // Go back to start if looping
				ov_time_seek( &m_oggStream, 0.0 );
			}
			else { // Or just quit otherwise
				break;
			}
		}
		else if ( result == OV_HOLE || result == OV_EBADLINK || result == OV_EINVAL ) {
			debug::Console->PrintError("Bad data in OGG stream!\n");
			return false;
		}
    }

	// Output new rawtime position
	rawtime = ov_time_tell( &m_oggStream );
    
	// If no data read, then no longer active
	if ( size == 0 ) {
        return loop;
	}
 
	// Push the data to the buffer!
    alBufferData( buffer, m_format, data, size, m_vorbisInfo->rate );
 
    return true;
}

double audio::BufferStreamed::GetLength ( void )
{
	return ov_time_total( &m_oggStream, -1 );
}
#endif