//===============================================================================================//
//
//	audio::Master
//		Master class for managing audio interfaces, mixers, listeners, and sources.
//
//===============================================================================================//
#ifndef AUDIO_MASTER_H_
#define AUDIO_MASTER_H_

#include "core/types/types.h"
#include "audio/mixing/Channels.h"
#include "audio/backend/AudioBackend.h"

#include <vector>
#include <string>
#include <mutex>
#include <atomic>

namespace audio
{
	class Listener;
	class Source;
	class Buffer;
	class BufferManager;
	class Mixer;

	struct MixerObjectState
	{
		std::vector<Source*>*	m_sources;
		std::vector<Listener*>*	m_listeners;
		double					m_speedOfSound = 1125.0;
		Real					m_channelGain [(uint)audio::MixChannel::kMAX_COUNT];
	};

	class Manager
	{
	public:
		// Constructor and Destructor
		AUDIO_API				Manager ( void );
		AUDIO_API				~Manager ( void );

		//	Update() : Per-frame keep-alive.
		AUDIO_API void			Update ( float deltatime );

		// Adding and removing objects
		// TODO: This should be automatic:
		AUDIO_API void			AddListener ( Listener* );
		AUDIO_API void			RemoveListener ( Listener* );
		AUDIO_API unsigned int	AddSource ( Source* );
		//AUDIO_API void			RemoveSource ( Source* );

		// Public queries
		AUDIO_API bool			IsActive ( void );

		/*AUDIO_API const std::vector<Source*>*
								GetSources ( void ) const
		{
			return &sources;
		}*/

		//	GetObjectStateForMixerThread( ... ) : Gets the object lists for the mixer thread
		void					GetObjectStateForMixerThread ( MixerObjectState& object_state );

		AUDIO_API const uint32_t
								GetPreferredSampleRate ( void );

		// Currently set speed of sound
		double					m_speedOfSound = 1125.0;
		// Currently set channel volumes
		Real					m_channelGain [(uint)audio::MixChannel::kMAX_COUNT];

	private:
		//	InitSystem() : Starts up the audio system & mixer
		// If succeeds, then this sets active to true
		void					InitSystem ( void );
		//	FreeSystem() : Frees all components of the audio system
		void					FreeSystem ( void );

	private:
		// Is true if the audio device is valid and usable
		bool					active = false;	

		// Next ID to assign to sounds
		unsigned int			next_sound_id = 0;

		// Lists of sound objects
		std::vector<Listener*>	listeners;
		std::vector<Source*>	sources;
		std::atomic_bool		mixer_objects_synced = false;
		std::mutex				mixer_objects_lock;

		std::vector<Listener*>	listeners_to_delete;
		std::vector<Source*>	sources_to_delete;

		AudioBackend*			backend = NULL;
		Mixer*					mixer = NULL;
	};

	AUDIO_API Manager* getValidManager ( Manager* managerPointer = NULL );
}

#endif//AUDIO_MASTER_H_