//===============================================================================================//
//
//	CAudioMaster.h
//		Master class for managing OpenAL interface
//
//===============================================================================================//

#ifndef AUDIO_MASTER_H_
#define AUDIO_MASTER_H_

// Includes
#include "core/types/types.h"
#include "AudioHeaders.h"
#include <cstdio>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

namespace audio
{
	class Listener;
	class Source;
	class Buffer;
	class BufferManager;

	// Class Definition
	class Master
	{
	public:
		// Constructor and Destructor
		AUDIO_API Master ( void );
		AUDIO_API ~Master ( void );

		//	Update() : Per-frame keep-alive.
		AUDIO_API void Update ( void );

		// Adding and removing objects
		// TODO: This should be automatic:
		AUDIO_API void AddListener ( Listener* );
		AUDIO_API void RemoveListener ( Listener* );
		AUDIO_API unsigned int AddSource ( Source* );
		AUDIO_API void RemoveSource ( Source* );

		// Public queries
		AUDIO_API static	Master* GetCurrent ( void );
		AUDIO_API static	bool	Active ( void );
	/*#ifdef _AUDIO_FMOD_
		static	FMOD::FMOD_SYSTEM*	System ( void ) {
	#ifdef _ENGINE_DEBUG
			if ( GetCurrent() == NULL ) {
				throw 0;
			}
	#endif
			return GetCurrent()->m_system;
		}
	#endif*/

		AUDIO_API const std::vector<Source*>* GetSources ( void ) const
		{
			return &sources;
		}
	
	private:
		// Private member routines
		void	InitSystem ( void );	// If succeeds, then this sets active to true
		void	FreeSystem ( void );
	private:
		// Static data
		static	Master* pActiveAudioMaster;

	private:
		// Private member data
		bool	active;		// Is true if the audio device is valid and usable

	/*#ifndef _AUDIO_FMOD_
		// OpenAL device and context pointers
		ALCdevice*	device;
		ALCcontext*	context;
	#else
		FMOD::FMOD_SYSTEM*	m_system;
	#endif*/

		// Next ID to assign to sounds
		unsigned int next_sound_id;

		// Lists of sound objects
		std::vector<Listener*>	listeners;
		std::vector<Source*>	sources;
		//vector<CAudioSound*>	sounds;
	};
}

#endif//AUDIO_MASTER_H_