#ifndef AUDIO_MIXER_H_
#define AUDIO_MIXER_H_

#include "core/types.h"
#include <thread>
#include <atomic>
#include <map>
#include <list>
#include <mutex>

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

		//	CleanupSource( source_id ) : Requests the given source be removed from the source state map.
		// Mixer has no idea when a source shouldn't be considered anymore, so this helps it along - checking items in the removal list until their data is emptied out.
		void					CleanupSource ( uint source_id );

	private:
		// Object state we sample for audio state
		Manager*				m_objectState = NULL;
		// Backend we output our audio to
		AudioBackend*			m_backendTarget = NULL;
		// Mixer thread
		std::thread				m_workerThread;

		// Should the mixer continue the work?
		std::atomic_bool		m_continueWork = true;

		// Max number of sources to sample at a time
		uint32_t				m_maxVoices = 64;
		double					m_speedOfSound = 1125.0;

		// Source states. Used to track buffers between audio frames for DSP.
		std::map<uint, void*>	m_sourceStateMap;

		//	FindSourceWorkbufferSet( source_id ) : Finds existing or allocates new workbuffers for the given source.
		SourceWorkbufferSet&	FindSourceWorkbufferSet ( uint source_id );

		std::mutex				m_sourceRemovalRequestsLock;
		// Sources requested destruction
		std::list<uint>			m_sourceRemovalRequests;
	};
}

#endif//AUDIO_MIXER_H_