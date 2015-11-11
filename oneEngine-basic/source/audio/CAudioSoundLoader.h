
#ifndef _C_AUDIO_SOUND_LOADER_H_
#define _C_AUDIO_SOUND_LOADER_H_

#include "CAudioMaster.h"
#include "stdio.h"

#ifndef _AUDIO_FMOD_

class CAudioSoundLoader
{

public:
	ALuint LoadFile ( const std::string& );

private:
	ALuint Error ( const char* );

	ALuint LoadWave ( const char* );

	// -------------------- WAVE FILE FORMAT -------------------- //
	/*
	 * Struct that holds the RIFF data of the Wave file.
	 * The RIFF data is the meta data information that holds,
	 * the ID, size and format of the wave file
	 */
	struct RIFF_Header {
		char chunkID[4];
		long chunkSize;//size not including chunkSize or chunkID
		char format[4];
	};
	/*
	* Struct to hold fmt subchunk data for WAVE files.
	*/
	struct WAVE_Format {
		char subChunkID[4];
		long subChunkSize;
		short audioFormat;
		short numChannels;
		long sampleRate;
		long byteRate;
		short blockAlign;
		short bitsPerSample;
	};
	/*
	* Struct to hold the data of the wave file
	*/
	struct WAVE_Data {
		char subChunkID[4]; //should contain the word data
		long subChunk2Size; //Stores the size of the data block
	};


	ALuint LoadOGG ( const char* );
};

#endif

#endif