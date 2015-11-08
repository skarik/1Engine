// class CAudioSound
//  Only contains the data for the sound. Does not actually play a sound.
//  CAudioSource is what uses CAudioSound to play a sound.

#ifndef _C_AUDIO_SOUND_H_
#define _C_AUDIO_SOUND_H_

#include "CAudioMaster.h"

class CSoundManager;

class CAudioSound
{
private:
	friend CSoundManager;

	int referenceCount;
public:

	void AddReference ( void ) { ++referenceCount; }
	void RemoveReference ( void ) { --referenceCount; }

#ifndef _AUDIO_FMOD_
	ALuint GetBuffer ( void ) { return buffer; }
#else
	FMOD::FMOD_SOUND*	Handle ( void ) { return m_sound; }
#endif

	bool IsStreamed ( void ) { return streamed; }
	bool IsPositional ( void ) { return positional; }

	virtual double GetLength ( void );

protected:
	bool	streamed;
	bool	positional;
#ifndef _AUDIO_FMOD_
	ALuint	buffer;
#else
	FMOD::FMOD_SOUND*	m_sound;
#endif
	
	void Init ( const std::string& );
	void Free ( void );

	explicit CAudioSound ( const std::string&, const int );
	explicit CAudioSound ( void );
	virtual ~CAudioSound ( void );
};	

#endif