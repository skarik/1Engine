
#ifndef _C_SOUND_MANAGER_H_
#define _C_SOUND_MANAGER_H_

class CAudioMaster;
class CAudioSound;
#include "core/types/types.h"
#include "core/containers/arstring.h"
#include <string>
#include <map>

// Sound Manager Class Definition
class CSoundManager
{
public:
	// Returns current active sound manager. It is created and destroyed by CAudioMaster.
	AUDIO_API static CSoundManager* GetActive ( void );

	// Finds the given sound. If it isn't loaded, it'll load the sound.
	// The pointer to the given sound is then returned.
	AUDIO_API CAudioSound*	GetSound ( const char*, const int n_positional=-1 );

	// Loads the given sound if not already loaded.
	AUDIO_API void	PrecacheSound ( const char* sndfn, const int n_positional=-1 ) { GetSound( sndfn, n_positional ); }
private:
	friend CAudioMaster; // Give AudioMaster access.

	// Map for the sound reference
	std::map<arstring<128>,CAudioSound*> soundmap;

	// Global instance
	static CSoundManager* Active;
};

#endif