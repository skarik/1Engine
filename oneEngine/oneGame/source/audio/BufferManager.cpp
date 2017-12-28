#include "core/utils/StringUtils.h"
#include "core/debug/console.h"

#include "AudioMaster.h"
#include "BufferManager.h"
#include "Buffer.h"
#include "BufferStreamed.h"

using std::string;
using std::map;

ARSINGLETON_CPP_DEF(audio::BufferManager);

//	GetSound(filename, 3d) : Finds the given sound, loading if needed.
// Immediately releases the reference upon load.
// The pointer to the given sound is then returned.
audio::Buffer* audio::BufferManager::GetSound ( const char* filename, const int n_positional )
{
	if ( !Master::Active() )
		return NULL;	// Do not load buffers if the device has failed

	// First look for it
	arstring<128> ar_soundName(filename);
	auto it = soundmap.find( ar_soundName );
	
	// It's not here
	if ( it == soundmap.end() )
	{
		arResourceBuffer resource = {0};

		// So create a new reference
		//  (choose between streamed and buffered based on extension)
		string sFileExtension = StringUtils::ToLower( StringUtils::GetFileExtension( filename ) );
		if ( sFileExtension == "ogg" || sFileExtension == "mp3" || sFileExtension == "mp2" )
		{	
			// TODO: Get the sound's length. If it's small enough, we may not need to stream it.
			resource.m_value = new BufferStreamed( filename, (n_positional == -1) ? false : n_positional );
			printf("New sound reference \"%s\" [streamed]\n", filename);
		}
		else
		{
			resource.m_value = new Buffer( filename, (n_positional == -1) ? true : n_positional );
			printf("New sound reference \"%s\" [normal]\n", filename);
		}
		resource.m_value->RemoveReference();
		
		resource.m_time_last_accessed = Time::currentTime;
		soundmap[ar_soundName] = resource;
		
		return soundmap[ar_soundName].m_value;
	}
	else
	{
		// Return found value
		it->second.m_time_last_accessed = Time::currentTime;
		return it->second.m_value;
	}
}


//	Tick(delta_time) : Updates the resource management system.
// When items get too old, they are unloaded.
void audio::BufferManager::Tick ( const float delta_time )
{
	const float referenceTime = Time::currentTime - kBufferUseTimeout;

	for (auto resource = soundmap.begin(); resource != soundmap.end(); )
	{
		if (resource->second.m_value->HasReference())
		{	// Update time reference.
			resource->second.m_time_last_accessed = Time::currentTime;
			++resource;
		}
		else if ( resource->second.m_time_last_accessed < referenceTime)
		{	// Free the resource.
			delete resource->second.m_value;
			resource = soundmap.erase(resource);
		}
		else
		{	// Skip.
			++resource;
		}
	}
}