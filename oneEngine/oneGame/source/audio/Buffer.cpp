#include "Buffer.h"
#include "WaveformLoader.h"
#include "audio/Manager.h"

#include "core/debug/console.h"
#include "core-ext/threads/Jobs.h"

using std::string;

audio::Buffer::Buffer ( const char* filename )
	: arBaseObject()
{
	Init( filename );
}
audio::Buffer::Buffer ( void )
	: arBaseObject()
{
}

audio::Buffer::~Buffer ( void )
{
	Free();
}

void audio::Buffer::Init ( const char* n_filename )
{
	auto auc = getValidManager();
	if ( !auc->IsActive() )
		return;

	// Load in the file from the disk:
	audio::WaveformLoader loader;
	m_sound = loader.LoadFile( n_filename );
	m_streamed = false;

	// If we have no sound, then mark this audio as bad.
	if (m_sound == NULL)
	{
		printf("Error loading \"%s\" [normal]\n", n_filename);
		m_readyToSample = false;
		return;
	}

	// Pack in the frames & sample rate info now for the audio thread:
	uint64_t shiftableSampleRate = m_sound->sampleRate;
	m_framesAndSampleRate_Packed = (m_sound->frames) | (shiftableSampleRate << 32);

	// We need to resample the audio:
	auto resampleJob = core::jobs::System::Current::AddJobRequest(
		[&, auc]
	{
		// Prevent the job from working on a deleted object:
		this->AddReference();

		// First, need length, and the new resampled length:
		uint32_t resampledRate = auc->GetPreferredSampleRate();

		const double frameScale = (double)m_sound->sampleRate / (double)resampledRate;
		const uint8_t channelCount = (uint8_t)m_sound->channels;

		uint32_t resampledFrames = (uint32_t)(m_sound->frames / frameScale);

		// New data
		float* resampledData = (float*) new uint64_t [(resampledFrames / (sizeof(uint64_t) / sizeof(float)) + 1) * channelCount];

		// Resample it:
		for (uint32_t frame = 0; frame < resampledFrames; ++frame)
		{
			uint32_t originalFrame = (uint32_t)(frame * frameScale);
			for (uint8_t channel = 0; channel < channelCount; ++channel)
			{
				if (m_sound->format == audio::Format::kSignedInteger8)
				{
					int8_t* sourceData = (int8_t*)m_sound->data;
					resampledData[frame * channelCount + channel] = sourceData[originalFrame * channelCount + channel] / (float)INT8_MAX;
				}
				else if (m_sound->format == audio::Format::kSignedInteger16)
				{
					int16_t* sourceData = (int16_t*)m_sound->data;
					resampledData[frame * channelCount + channel] = sourceData[originalFrame * channelCount + channel] / (float)INT16_MAX;
				}
				else if (m_sound->format == audio::Format::kFloat32)
				{
					resampledData[frame * channelCount + channel] = m_sound->data[originalFrame * channelCount + channel];
				}
			}
		}

		// Save the new resampled data & free the old data
		delete[] m_sound->data;
		m_sound->data = resampledData;

		// Set the new parameters
		m_sound->sampleRate = resampledRate;
		m_sound->format = audio::Format::kFloat32;
		m_sound->frames = resampledFrames;

		// Now we can apply the correct sample rate when done
		uint64_t shiftableSampleRate = m_sound->sampleRate;
		m_framesAndSampleRate_Packed = (m_sound->frames) | (shiftableSampleRate << 32);
		
		// And we're ready to sample
		m_readyToSample = true;

		// Now we're done, we can remove the reference
		this->RemoveReference();
	});
}

void audio::Buffer::Free ( void )
{
	auto auc = getValidManager();
	if ( !auc->IsActive() )
		return;

	if (m_sound)
	{
		delete[] m_sound->data;
		delete m_sound;
	}
	m_sound = NULL;
}

double audio::Buffer::GetLength ( void ) const
{
	uint64_t packedData = m_framesAndSampleRate_Packed;
	uint32_t frames = 0xFFFFFFFF & packedData;
	uint32_t sampleRate = 0xFFFFFFFF & (packedData >> 32);

	return frames / (double)sampleRate;
}

uint32_t audio::Buffer::GetSampleLength ( void ) const
{
	uint64_t packedData = m_framesAndSampleRate_Packed;
	uint32_t frames = 0xFFFFFFFF & packedData;

	return frames;
}

uint32_t audio::Buffer::GetSampleRate ( void ) const
{
	uint64_t packedData = m_framesAndSampleRate_Packed;
	uint32_t sampleRate = 0xFFFFFFFF & (packedData >> 32);

	return sampleRate;
}

bool audio::Buffer::GetReadyToSample ( void ) const
{
	return m_readyToSample;
}

audio::ChannelCount audio::Buffer::GetChannelCount ( void ) const
{
	if (m_sound == NULL)
		return audio::ChannelCount::kInvalid;
	else
		return m_sound->channels;
}

void audio::Buffer::Sample ( uint32_t& inout_sample_position, uint32_t sample_count, float* sample_output )
{
	ARCORE_ASSERT(m_sound != NULL);
	ARCORE_ASSERT(inout_sample_position + sample_count <= m_sound->frames);
	uint32_t copy_start = inout_sample_position * (uint)m_sound->channels;
	uint32_t copy_end = copy_start + sample_count * (uint)m_sound->channels;

	std::copy(&m_sound->data[copy_start], &m_sound->data[copy_end], sample_output);

	inout_sample_position += sample_count;
}