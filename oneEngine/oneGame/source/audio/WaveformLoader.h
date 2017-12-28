#ifndef AUDIO_WAVEFORM_LOADER_H_
#define AUDIO_WAVEFORM_LOADER_H_

#include "audio/AudioMaster.h"
#include "audio/types/handles.h"
#include "stdio.h"

namespace audio
{
	class WaveformLoader
	{

	public:
		arBufferHandle LoadFile ( const char* filename, bool positional );

	private:
		arReturnCode Error ( const char* );

		arBufferHandle LoadWave ( const char* );

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


		arBufferHandle LoadOGG ( const char* );
	};
}

#endif///AUDIO_WAVEFORM_LOADER_H_