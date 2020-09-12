#ifndef AUDIO_MIXER_H_
#define AUDIO_MIXER_H_

#include "core/types.h"
#include <thread>
#include <atomic>

namespace audio
{
	class AudioBackend;

	// Thread-destroying stereo mixer.
	class Mixer
	{
	public:
		Mixer ( AudioBackend* backend );
		~Mixer ( void );

	private:
		AudioBackend*			m_backendTarget = NULL;
		std::thread				m_workerThread;

		std::atomic_bool		m_continueWork = true;
	};
}

#endif//AUDIO_MIXER_H_