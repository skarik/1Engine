//---------------------------------------------
//	CAudioMaster.h
//		Master class for managing OpenAL interface
//
//

#ifndef _C_AUDIO_MASTER_H_
#define _C_AUDIO_MASTER_H_


// Includes
#include "core/types/types.h"
#include "AudioHeaders.h"
#include <cstdio>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

/*using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::find;*/

// Class prototypes
class CAudioListener;
class CAudioSource;
class CAudioSound;
class CAudioManager;

// Class Definition
class CAudioMaster
{
public:
	// Constructor and Destructor
	AUDIO_API CAudioMaster ( void );
	AUDIO_API ~CAudioMaster ( void );

	// Per-frame update
	AUDIO_API void Update ( void );

	// Adding and removing objects
	AUDIO_API void AddListener ( CAudioListener* );
	AUDIO_API void RemoveListener ( CAudioListener* );
	AUDIO_API unsigned int AddSource ( CAudioSource* );
	AUDIO_API void RemoveSource ( CAudioSource* );

	// Public queries
	AUDIO_API static	CAudioMaster* GetCurrent ( void );
	AUDIO_API static	bool	Active ( void );
#ifdef _AUDIO_FMOD_
	static	FMOD::FMOD_SYSTEM*	System ( void ) {
#ifdef _ENGINE_DEBUG
		if ( GetCurrent() == NULL ) {
			throw 0;
		}
#endif
		return GetCurrent()->m_system;
	}
#endif

	AUDIO_API const std::vector<CAudioSource*>* GetSources ( void ) const
	{
		return &sources;
	}
	
private:
	// Private member routines
	void	InitSystem ( void );	// If succeeds, then this sets active to true
	void	FreeSystem ( void );
private:
	// Static data
	static	CAudioMaster* pActiveAudioMaster;

private:
	// Private member data
	bool	active;		// Is true if the audio device is valid and usable

#ifndef _AUDIO_FMOD_
	// OpenAL device and context pointers
	ALCdevice*	device;
	ALCcontext*	context;
#else
	FMOD::FMOD_SYSTEM*	m_system;
#endif

	// Next ID to assign to sounds
	unsigned int next_sound_id;

	// Lists of sound objects
	std::vector<CAudioListener*>	listeners;
	std::vector<CAudioSource*>	sources;
	//vector<CAudioSound*>	sounds;
};


#endif 