#ifndef AUDIO_MIXER_H_
#define AUDIO_MIXER_H_

#include "core/types.h"
#include <thread>
#include <atomic>
#include <map>

namespace audio
{
	class Manager;
	class AudioBackend;
	struct SourceWorkbufferSet;

	// Thread-destroying stereo mixer.
	class Mixer
	{
	public:
								Mixer ( Manager* object_state, AudioBackend* backend, uint32_t max_voices );
								~Mixer ( void );

		void					CleanupSource ( uint source_id );

	private:
		Manager*				m_objectState = NULL;
		AudioBackend*			m_backendTarget = NULL;
		std::thread				m_workerThread;

		std::atomic_bool		m_continueWork = true;

		uint32_t				m_maxVoices = 64;

		std::map<uint, void*>	m_sourceStateMap;

		SourceWorkbufferSet&	FindSourceWorkbufferSet ( uint source_id );
	};
}

#endif//AUDIO_MIXER_H_