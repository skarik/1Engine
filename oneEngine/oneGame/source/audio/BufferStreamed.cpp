#include "BufferStreamed.h"
#include "WaveformLoader.h"

#include "core/debug/console.h"
#include "core/math/Math.h"
#include "audio/Manager.h"

#include <algorithm>

audio::BufferStreamed::BufferStreamed ( const char* filename )
	: Buffer()
{
	m_streamed = true;

	InitStream( filename );
}

audio::BufferStreamed::~BufferStreamed ( void )
{
	FreeStream();
}

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

void audio::BufferStreamed::InitStream ( const char* filename )
{
	// We create a (mostly) dummy sounds for streamed buffers
	m_sound = new arBufferData;

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
	{
		m_sound->format = Format::kSignedInteger16;
		m_sound->channels = ChannelCount::kMono;
		m_sound->sampleRate = m_vorbisInfo->rate;
	}
    else if ( m_vorbisInfo->channels == 2 )
	{
		m_sound->format = Format::kSignedInteger16;
		m_sound->channels = ChannelCount::kStereo;
		m_sound->sampleRate = m_vorbisInfo->rate;
	}
	else
	{
		ov_clear(&m_oggStream);
		m_file = NULL;
		debug::Console->PrintError("Invalid channel count of `%d` on Ogg: \"%s\"\n", m_vorbisInfo->channels, filename);
	}

	// Calculate the number of virtual frames the system wants to sample.
	auto auc = getValidManager();
	uint32_t resampledRate = auc->GetPreferredSampleRate();
	const double frameScale = (double)m_sound->sampleRate / (double)resampledRate;
	m_sound->frames = (uint32_t)(ov_pcm_total( &m_oggStream, -1 ) / frameScale);

	// Store length of the audio as well
	m_vorbisLength = ov_time_total( &m_oggStream, -1 );
}

void audio::BufferStreamed::FreeStream ( void )
{
	if (m_sound)
	{
		delete m_sound;
	}
	m_sound = NULL;

	// Clear decoder buffer & close FILE
    ov_clear( &m_oggStream ); 
}

void audio::BufferStreamed::Sample ( uint32_t& inout_sample_position, uint32_t sample_count, float* sample_output )
{
	auto auc = getValidManager();
	// First grab the scaled sample rate
	uint32_t resampledRate = auc->GetPreferredSampleRate();

	// We need the frame scale because we need to size up or down the samples read based on the rate
	const double frameScale = (double)m_sound->sampleRate / (double)resampledRate;

	// Create the buffer we use to read in
	const int read_buffer_size = (int)(frameScale * sizeof(int16_t) * sample_count * (uint)m_sound->channels);
	char* read_buffer = (char*)new uint64_t[read_buffer_size / sizeof(uint64_t) + 2];

	std::fill(read_buffer, read_buffer + read_buffer_size, 0);
	std::fill(sample_output, sample_output + sample_count * (uint)m_sound->channels, 0.0F);

	int size = 0;
	int out_bitstream = 0;
	int result = 0;

	int64_t current_sample = ov_pcm_tell( &m_oggStream );
	int64_t seeking_sample = (int64_t)std::round(inout_sample_position * frameScale);
	if (abs(current_sample - seeking_sample) > 2)
	{
		ov_pcm_seek( &m_oggStream, seeking_sample );
		debug::Console->PrintWarning("OGG Reader: Samples got a little bit too out of sync!\n");
	}

	// Read in remaining data info buffer
	while ( size < read_buffer_size - 4 )
	{
		result = ov_read( &m_oggStream, read_buffer + size, read_buffer_size - size, 0, 2, 1, &out_bitstream );

		if ( result > 0 )
		{
			size += result;
		}
		else if ( result == 0 )
		{
			// Set the rest of the data to zeros and quit
			std::fill(read_buffer + size, read_buffer + read_buffer_size, 0);
			break;
		}
		else if ( result == OV_HOLE || result == OV_EBADLINK || result == OV_EINVAL )
		{
			debug::Console->PrintError("Bad data in OGG stream!\n");
			break;
		}
	}

	// Output new rawtime position
	inout_sample_position = (uint32_t)std::round(ov_pcm_tell( &m_oggStream ) / frameScale);

	{	// Now, we resample the data to the correct format:
		const uint8_t channelCount = (uint8_t)m_sound->channels;

		const double frameScaleToNewData = frameScale * ((double)size / (double)read_buffer_size);
		
		// TODO: do different scaling depending on if frameScaleToNewData < 1, ==0, or > 1.

		// Resample it:
		for (uint32_t frame = 0; frame < sample_count; ++frame)
		{
			double originalFrame_Loose = frame * frameScaleToNewData;
			float originalFrame_Blend = (float)(originalFrame_Loose - (uint64_t)originalFrame_Loose);
			uint32_t originalFrame_0 = (uint32_t)originalFrame_Loose;
			uint32_t originalFrame_1 = std::min((uint32_t)(size / (sizeof(int16_t) * (uint)m_sound->channels)) - 1, originalFrame_0 + 1); // Safe since padding in buffer.

			for (uint8_t channel = 0; channel < channelCount; ++channel)
			{
				if (m_sound->format == audio::Format::kSignedInteger16)
				{
					int16_t* sourceData = (int16_t*)read_buffer;
					float value0 = sourceData[originalFrame_0 * channelCount + channel] / (float)INT16_MAX;
					float value1 = sourceData[originalFrame_1 * channelCount + channel] / (float)INT16_MAX;
					sample_output[frame * channelCount + channel] = math::lerp(originalFrame_Blend, value0, value1);
				}
			}
		}
	}

	// Free the temp buffer
	delete[] read_buffer;
}

double audio::BufferStreamed::GetLength ( void ) const
{
	return m_vorbisLength;
}

uint32_t audio::BufferStreamed::GetSampleLength ( void ) const
{
	return m_sound->frames;
}