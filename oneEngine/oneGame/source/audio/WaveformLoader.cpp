#include "WaveformLoader.h"
#include "core/utils/string.h"
#include "core/debug/console.h"

audio::arBufferHandle audio::WaveformLoader::LoadFile ( const char* sFilename )
{
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
		debug::Console->PrintWarning(__FILE__ "(%d) don't recognize extension %s\n", __LINE__, sFileExtension.c_str());
	}
	return NULL;
}

audio::arBufferHandle audio::WaveformLoader::Error ( const char* message )
{
	debug::Console->PrintError("%s\n", message);
	return NULL;
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

	// Read in sections until we hit the data chunk:
	bool at_data = false;
	while (!at_data)
	{
		// Read in the the last byte of data before the sound file
		fread(&wave_data, sizeof(WAVE_Data), 1, soundFile);
		// Check for 'data' tag in memory
		if (wave_data.subChunkID[0] != 'd' ||
			wave_data.subChunkID[1] != 'a' ||
			wave_data.subChunkID[2] != 't' ||
			wave_data.subChunkID[3] != 'a')
		{
			fseek( soundFile, wave_data.subChunk2Size, SEEK_CUR );
			if (feof( soundFile ))
			{
				break;
			}
		}
		else
		{
			at_data = true;
		}
	}

	if (!at_data)
	{
		return Error( "CAudioSoundLoader::LoadWave: Invalid data header" );
	}

	// Read in the the last byte of data before the sound file
   /* fread(&wave_data, sizeof(WAVE_Data), 1, soundFile);
    // Check for 'data' tag in memory
    if (wave_data.subChunkID[0] != 'd' ||
        wave_data.subChunkID[1] != 'a' ||
        wave_data.subChunkID[2] != 't' ||
        wave_data.subChunkID[3] != 'a')
             return Error( "CAudioSoundLoader::LoadWave: Invalid data header" );*/

	// Allocate memory for data
	data = (unsigned char*) new uint64_t [ wave_data.subChunk2Size / sizeof(uint64_t) + 1 ]; // Allocate on the alignment of 8-byte for speed
    // Read in the sound data into the soundData variable
    if ( !fread( data, wave_data.subChunk2Size, 1, soundFile ) )
        return Error( "CAudioSoundLoader::LoadWave: error loading WAVE data into struct!" );

	arBufferHandle buffer = new arBufferData;
	{
		//// Now we set the variables that we passed in with the
		//// data from the structs
		//ALsizei size		= wave_data.subChunk2Size;
		//ALsizei frequency	= wave_format.sampleRate;
		//// The format is worked out by looking at the number of
		//// channels and the bits per sample.
		//ALenum format;
		//if (wave_format.numChannels == 1)
		//{
		//	if (wave_format.bitsPerSample == 8 )
		//		format = AL_FORMAT_MONO8;
		//	else if (wave_format.bitsPerSample == 16)
		//		format = AL_FORMAT_MONO16;
		//}
		//else if (wave_format.numChannels == 2)
		//{
		//	if (wave_format.bitsPerSample == 8 )
		//		format = AL_FORMAT_STEREO8;
		//	else if (wave_format.bitsPerSample == 16)
		//		format = AL_FORMAT_STEREO16;
		//}
		//// Create the OpenAL buffer
		//alGenBuffers( 1, &buffer );
		//// Send the data to OpenAL
		//alBufferData( buffer, format, (void*)data, size, frequency );

		buffer->channels = (audio::ChannelCount)wave_format.numChannels;
		buffer->sampleRate = wave_format.sampleRate;
		switch (wave_format.bitsPerSample)
		{
		case 8:
			buffer->format = audio::Format::kSignedInteger8;
			break;
		case 16:
			buffer->format = audio::Format::kSignedInteger16;
			break;
		case 32:
			buffer->format = audio::Format::kFloat32;
			break;
		default:
			buffer->format = audio::Format::kInvalid;
		}

		// Calculate the length of the buffer
		buffer->frames = (uint32_t)(((int64_t)wave_data.subChunk2Size * 8) / ((int64_t)wave_format.numChannels * wave_format.bitsPerSample));

		// Save the loaded data. This cast is safe as the data is aligned to 8 bytes
		buffer->data = (float*)data;
	}

	// We keep the data around until we resample the buffer in a separate thread in the buffer loader itself.

	// Close the file
	fclose(soundFile);
	// Return the buffer
	return buffer;
}