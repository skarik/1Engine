#include "audio/Manager.h"
#include "audio/Listener.h"
#include "audio/Source.h"
#include "audio/BufferManager.h"
#include "audio/Buffer.h"

#include "audio/backend/WASAPIAudioBackend.h"
#include "audio/mixing/Mixer.h"
#include "core/debug/console.h"

#include <vector>
#include <string>
#include <algorithm>

/*using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::find;*/

// Static variable defines
static audio::Manager*	m_ActiveAudioManager = NULL;

audio::Manager* audio::getValidManager ( Manager* managerPointer )
{
	if (managerPointer == NULL || !managerPointer->IsActive())
	{
		managerPointer = m_ActiveAudioManager;
	}
	return managerPointer;
}

// CAudioMaster function definitions
audio::Manager::Manager ( void )
{
	m_ActiveAudioManager = this;
	// Create sound master
	BufferManager::Active();

	// Output debug messages
	//cout << "Audio: using Win32 build of OpenAL Soft 1.14 (DirectSound build)" << endl;
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

	delete BufferManager::m_Active;
	BufferManager::m_Active = NULL;

	if ( m_ActiveAudioManager == this )
		m_ActiveAudioManager = NULL;
}

// Update
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

	// If the objects are sync, we can safely delete items
	if (mixer_objects_synced)
	{
		for (Listener* listener : listeners_to_delete)
		{
			delete listener;
		}
		for (Source* source : sources_to_delete)
		{
			// Remove the source from the mixer (Mixer must take care of it in case it's still echoing or has reverb)
			mixer->CleanupSource(source->GetID());

			// Free up the source now that we're done with its info
			delete source;
		}

		listeners_to_delete.clear();
		sources_to_delete.clear();
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
		std::copy(m_channelGain, m_channelGain + (uint)audio::MixChannel::kMAX_COUNT, object_state.m_channelGain);
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

// Adding and removing objects
void audio::Manager::AddListener ( Listener* listener )
{
	listeners.push_back( listener );
}

void audio::Manager::RemoveListener ( Listener* listener )
{
	std::vector<Listener*>::iterator it;
	it = find( listeners.begin(), listeners.end(), listener );
	if ( it == listeners.end() )
	{
		debug::Console->PrintError(__FILE__ "(%d) Error destroying listener THAT DOESN'T EXIST (should never happen" "\n", __LINE__);
	}
	else
	{
		listeners.erase( it );
	}
}

unsigned int audio::Manager::AddSource ( Source* source )
{
	sources.push_back( source );
	return next_sound_id++;
}

/*void audio::Manager::RemoveSource ( Source* source )
{
	std::vector<Source*>::iterator it;
	it = find( sources.begin(), sources.end(), source );
	if ( it == sources.end() )
	{
		debug::Console->PrintError(__FILE__ "(%d) Error removing non-existant source from list (should never happen" "\n", __LINE__);
	}
	else
	{
		sources.erase( it );
	}
}*/

bool audio::Manager::IsActive ( void ) 
{
	return active;
}

const uint32_t audio::Manager::GetPreferredSampleRate ( void )
{
	return (uint32_t)backend->SampleRate();
}