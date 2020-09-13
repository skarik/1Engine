#ifndef AUDIO_WAVEFORM_LOADER_H_
#define AUDIO_WAVEFORM_LOADER_H_

#include "audio/types/Handles.h"
#include "audio/types/BufferData.h"

namespace audio
{
	class WaveformLoader
	{
	public:
		//	LoadFile( filename ) : Loads the given file into a newly allocated PCM buffer.
		// Does not perform any resampling.
		arBufferHandle			LoadFile ( const char* filename );

	private:
		//	Error() : Spits the input string as an error and returns NULL.
		arBufferHandle			Error ( const char* );

		// -------------------- WAVE FILE FORMAT -------------------- //

		//	LoadWave( ... ) : Loads & allocates memory for new PCM buffer.
		// Does not perform any resampling.
		arBufferHandle			LoadWave ( const char* );
		
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

		// -------------------- OGG FILE FORMAT -------------------- //
		// Not actually handled here yet - typically OGGs are streamed.

		//	LoadOGG( ... ) : Loads & allocates memory for new PCM buffer.
		// Does not perform any resampling.
		arBufferHandle			LoadOGG ( const char* );
	};
}

#endif///AUDIO_WAVEFORM_LOADER_H_