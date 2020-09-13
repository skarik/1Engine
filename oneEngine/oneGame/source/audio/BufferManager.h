#ifndef AUDIO_BUFFER_MANAGER_H_
#define AUDIO_BUFFER_MANAGER_H_

#include "core/types/types.h"
#include "core/containers/arstring.h"
#include "core/containers/arsingleton.h"
#include "core-ext/containers/arresource.h"
#include <string>
#include <map>

namespace audio
{
	class Manager;
	class Buffer;

	typedef arresource<Buffer*> arResourceBuffer;

	// Time to wait after use to free a resource:
	const float kBufferUseTimeout = 120.0F;

	// Sound Manager Class Definition
	class BufferManager
	{
		ARSINGLETON_H_STORAGE(BufferManager, AUDIO_API);
		ARSINGLETON_H_ACCESS (BufferManager);
	public:
		//	GetSound(filename, 3d) : Finds the given sound, loading if needed.
		// Immediately releases the reference upon load.
		// The pointer to the given sound is then returned.
		AUDIO_API Buffer*		GetSound ( const char* filename, const int=-1 );

		//	PrecacheSound(filename, 3d) : Loads the given sound if not already loaded.
		// Uses GetSound() and immediately releases the reference.
		AUDIO_API void			PrecacheSound ( const char* filename, const int=-1 )
			{ GetSound( filename ); }

		//	Tick(delta_time) : Updates the resource management system.
		// When items get too old, they are unloaded.
		AUDIO_API void			Tick ( const float delta_time );
	private:
		friend Manager; // Give AudioMaster access.

		// Map for the sound reference
		std::map<arstring128,arResourceBuffer> soundmap;
	};
}

#endif