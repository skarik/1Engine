#include "WaveformLoader.h"
#include "core/utils/string.h"

using namespace std;

audio::arBufferHandle audio::WaveformLoader::LoadFile ( const char* sFilename, bool positional )
{
#ifndef _AUDIO_FMOD_
	string sFileRezname = sFilename;
	string sFileExtension = core::utils::string::GetLower( core::utils::string::GetFileExtension( sFileRezname ) );

	if ( sFileExtension == "wav" )
	{
		return LoadWave( sFilename );
	}
	else if ( sFileExtension == "ogg" )
	{
		return LoadOGG( sFilename );
	}
	else
	{
		cout << __FILE__ << "(" << __LINE__ << ") don't recognize extension " << sFileExtension << endl;
	}
	return BUFFER_NULL;
#else
	FMOD::FMOD_RESULT result = FMOD::FMOD_OK;
	arBufferHandle result = BUFFER_NULL;

	if ( positional ) {
		result = FMOD::FMOD_System_CreateSound( CAudioMaster::System(), sFilename, FMOD_DEFAULT | FMOD_3D, 0, &result );
	}
	else {
		result = FMOD::FMOD_System_CreateSound( CAudioMaster::System(), sFilename, FMOD_DEFAULT, 0, &result );
	}

	// Check to see if it loaded properly
	if ( result != FMOD::FMOD_OK ) {
		debug::Console->PrintError("FMOD could not open the file \"" + sFileName + "\"");
		result = BUFFER_NULL;
	}
	return result;
#endif
}

audio::arReturnCode audio::WaveformLoader::Error ( const char* message )
{
	cout << message << endl;
	return 0;
}


// Load WAV file
audio::arBufferHandle audio::WaveformLoader::LoadWave ( const char* sFilename )
{
	FILE* soundFile;
	WAVE_Format	wave_format;
	RIFF_Header	riff_header;
	WAVE_Data	wave_data;
	unsigned char* data;

	// Open file
	soundFile = fopen( sFilename, "rb" );
	if ( soundFile == NULL )
		return Error( "CAudioSoundLoader::LoadWave: Could not open file" );

	// Read in the RIFF header
	fread( &riff_header, sizeof(RIFF_Header), 1, soundFile );
	// Check the RIFF header before continuing.
	if ((riff_header.chunkID[0] != 'R' ||
         riff_header.chunkID[1] != 'I' ||
         riff_header.chunkID[2] != 'F' ||
         riff_header.chunkID[3] != 'F') &&
        (riff_header.format[0] != 'W' ||
         riff_header.format[1] != 'A' ||
         riff_header.format[2] != 'V' ||
         riff_header.format[3] != 'E'))
         return Error( "CAudioSoundLoader::LoadWave: Invalid RIFF header" );

	// Read in the WAVE format information
    fread(&wave_format, sizeof(WAVE_Format), 1, soundFile);
    // Check for 'fmt ' tag in memory
    if (wave_format.subChunkID[0] != 'f' ||
        wave_format.subChunkID[1] != 'm' ||
        wave_format.subChunkID[2] != 't' ||
        wave_format.subChunkID[3] != ' ')
             return Error( "CAudioSoundLoader::LoadWave: Invalid Wave Format" );

	// Check for extra parameters
    if (wave_format.subChunkSize > 16)
        fseek( soundFile, sizeof(short), SEEK_CUR );

	// Read in the the last byte of data before the sound file
    fread(&wave_data, sizeof(WAVE_Data), 1, soundFile);
    // Check for 'data' tag in memory
    if (wave_data.subChunkID[0] != 'd' ||
        wave_data.subChunkID[1] != 'a' ||
        wave_data.subChunkID[2] != 't' ||
        wave_data.subChunkID[3] != 'a')
             return Error( "CAudioSoundLoader::LoadWave: Invalid data header" );

	// Allocate memory for data
    data = new unsigned char [ wave_data.subChunk2Size ];
    // Read in the sound data into the soundData variable
    if ( !fread( data, wave_data.subChunk2Size, 1, soundFile ) )
        return Error( "CAudioSoundLoader::LoadWave: error loading WAVE data into struct!" );

	ALuint buffer = 0;
	{
		// Now we set the variables that we passed in with the
		// data from the structs
		ALsizei size		= wave_data.subChunk2Size;
		ALsizei frequency	= wave_format.sampleRate;
		// The format is worked out by looking at the number of
		// channels and the bits per sample.
		ALenum format;
		if (wave_format.numChannels == 1)
		{
			if (wave_format.bitsPerSample == 8 )
				format = AL_FORMAT_MONO8;
			else if (wave_format.bitsPerSample == 16)
				format = AL_FORMAT_MONO16;
		}
		else if (wave_format.numChannels == 2)
		{
			if (wave_format.bitsPerSample == 8 )
				format = AL_FORMAT_STEREO8;
			else if (wave_format.bitsPerSample == 16)
				format = AL_FORMAT_STEREO16;
		}
		// Create the OpenAL buffer
		alGenBuffers( 1, &buffer );
		// Send the data to OpenAL
		alBufferData( buffer, format, (void*)data, size, frequency );
	}

	// Free temp data
	delete [] data; // Though it may be possible we want this data down the road, so consider saving it
					// Effects need to be rendered in real time, causing this issue.

	// Close the file
	fclose(soundFile);
	// Return the buffer
	return buffer;
}