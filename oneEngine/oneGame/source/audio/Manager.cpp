#include "audio/Manager.h"
#include "audio/Listener.h"
#include "audio/Source.h"
#include "audio/BufferManager.h"
#include "audio/Buffer.h"
#include "audio/Effect.h"

#include "audio/backend/WASAPIAudioBackend.h"
#include "audio/mixing/Mixer.h"
#include "core/debug/console.h"

#include <vector>
#include <string>
#include <algorithm>

static audio::Manager*	m_ActiveAudioManager = NULL;

audio::Manager* audio::getValidManager ( Manager* managerPointer )
{
	if (managerPointer == NULL || !managerPointer->IsActive())
	{
		managerPointer = m_ActiveAudioManager;
	}
	return managerPointer;
}

audio::Manager::Manager ( void )
{
	m_ActiveAudioManager = this;
	// Create sound master
	BufferManager::Active();

	// Output debug messages
	debug::Console->PrintMessage("Audio: start up the audio system...\n");

	InitSystem();
	if ( IsActive() )
	{
		debug::Console->PrintMessage("Creation of audio device successful!\n");
	}
	else
	{
		debug::Console->PrintError("Unable to created audio backend!\n");
	}

	// Set up default channel volumes
	for (uint i = 0; i < (uint)audio::MixChannel::kMAX_COUNT; ++i)
	{
		m_channelGain[i] = 1.0F;
	}
}

audio::Manager::~Manager ( void )
{
	FreeSystem();

	// Clear out all the managed audio-side objects we have
	for (Listener* listener : listeners_to_delete)
	{
		delete listener;
	}
	for (Source* source : sources_to_delete)
	{
		delete source;
	}
	for (auto& channel_effects : effects)
	{
		for (Effect* effect : channel_effects)
		{
			delete effect;
		}
	}

	delete BufferManager::m_Active;
	BufferManager::m_Active = NULL;

	if ( m_ActiveAudioManager == this )
		m_ActiveAudioManager = NULL;
}

void audio::Manager::Update ( float deltatime )
{
	if ( !IsActive() )
		return;

	// Update the buffer manager
	BufferManager::Active()->Tick( deltatime );

	// Go through the listeners and update them
	for ( std::vector<Listener*>::iterator it = listeners.begin(); it != listeners.end(); )
	{
		Listener* current = *it;
		if ( current && !current->queue_destruction )
		{
			current->Update();
			++it;
		}
		else
		{
			listeners_to_delete.push_back(*it);
			{
				std::lock_guard<std::mutex> lock(mixer_objects_lock);
				it = listeners.erase(it);
			}
			mixer_objects_synced = false;
		}
	}

	// Go through the sources and update them
	for ( std::vector<Source*>::iterator it = sources.begin(); it != sources.end(); )
	{
		Source* current = *it;
		if ( current && !current->queue_destruction )
		{
			current->GameTick( deltatime );
			++it;
		}
		else
		{
			sources_to_delete.push_back(*it);
			{
				std::lock_guard<std::mutex> lock(mixer_objects_lock);
				it = sources.erase(it);
			}
			mixer_objects_synced = false;
		}
	}

	// Go through the effects and update them
	for ( auto& channel_effects : effects )
	{
		for ( std::vector<Effect*>::iterator it = channel_effects.begin(); it != channel_effects.end(); )
		{
			Effect* current = *it;
			if ( current && !current->queue_destruction )
			{
				// Is there really an update for these?
				++it;
			}
			else
			{
				effects_to_delete.push_back(*it);
				{
					std::lock_guard<std::mutex> lock(mixer_objects_lock);
					it = channel_effects.erase(it);
				}
				mixer_objects_synced = false;
			}
		}
	}

	// If the objects are sync, we can safely delete items
	if (mixer_objects_synced)
	{
		for (Listener* listener : listeners_to_delete)
		{
			delete listener;
		}
		listeners_to_delete.clear();

		for (Source* source : sources_to_delete)
		{
			// Remove the source from the mixer (Mixer must take care of it in case it's still echoing or has reverb)
			mixer->CleanupSource(source->GetID());

			// Free up the source now that we're done with its info
			delete source;
		}
		sources_to_delete.clear();

		for (Effect* effect : effects_to_delete)
		{
			delete effect;
		}
		effects_to_delete.clear();
	}
}

void audio::Manager::GetObjectStateForMixerThread ( MixerObjectState& object_state )
{
	// Copy over the lists now
	{
		std::lock_guard<std::mutex> lock(mixer_objects_lock);
		*object_state.m_sources = sources;
		*object_state.m_listeners = listeners;
		object_state.m_speedOfSound = m_speedOfSound;
		for (uint i = 0; i < (uint)audio::MixChannel::kMAX_COUNT; ++i)
		{
			// Set channel game
			object_state.m_channel[i].m_gain = m_channelGain[i];
			// Copy over the current effects list
			object_state.m_channel[i].m_effects = effects[i];
		}
		object_state.m_channel[(uint)audio::MixChannel::kMAX_COUNT].m_gain = 1.0F;
		object_state.m_channel[(uint)audio::MixChannel::kMAX_COUNT].m_effects = effects[(uint)audio::MixChannel::kMAX_COUNT];
	}
	mixer_objects_synced = true;
}

void audio::Manager::InitSystem ( void )
{
#ifdef PLATFORM_WINDOWS
	backend = new WASAPIAudioBackend();
#else
	backend = NULL;
#endif

	if (backend != NULL)
	{
		backend->Start();

		if (backend->ChannelCount() != 0)
		{
			mixer = new Mixer(this, backend, 128);
			active = true;
		}
	}
}

void audio::Manager::FreeSystem ( void )
{
	if ( !IsActive() )
		return;

	delete mixer;

	backend->Stop();
	delete backend;
}

void audio::Manager::AddListener ( Listener* listener )
{
	listeners.push_back( listener );
}

unsigned int audio::Manager::AddSource ( Source* source )
{
	sources.push_back( source );
	return next_sound_id++;
}

void audio::Manager::AddEffect ( Effect* effect, audio::MixChannel channel )
{
	uint channel_index = (uint)channel;
	ARCORE_ASSERT(channel_index >= 0 && channel_index <= (uint)audio::MixChannel::kMAX_COUNT);

	effects[channel_index].push_back(effect);
}

bool audio::Manager::IsActive ( void ) 
{
	return active;
}

const uint32_t audio::Manager::GetPreferredSampleRate ( void )
{
	return (uint32_t)backend->SampleRate();
}