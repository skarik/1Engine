#ifndef AUDIO_MIXER_H_
#define AUDIO_MIXER_H_

#include "core/types.h"
#include <thread>
#include <atomic>

namespace audio
{
	class Manager;
	class AudioBackend;

	// Thread-destroying stereo mixer.
	class Mixer
	{
	public:
		Mixer ( Manager* object_state, AudioBackend* backend, uint32_t max_voices );
		~Mixer ( void );

	private:
		Manager*				m_objectState = NULL;
		AudioBackend*			m_backendTarget = NULL;
		std::thread				m_workerThread;

		std::atomic_bool		m_continueWork = true;

		uint32_t				m_maxVoices = 64;
	};
}

#endif//AUDIO_MIXER_H_