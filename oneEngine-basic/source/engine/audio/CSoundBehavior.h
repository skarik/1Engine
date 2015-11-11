// class CSoundBehavior
//  Is instantiated by the CAudioInterface class to make wonderful sound
//  Has a position and velocity, but that is it.
//  Keeps track of itself with an internal list that can be accessed for read.
//  This list can then be used to manage AI and other sound-based effects.

#ifndef _C_SOUND_BEHAVIOR_H_
#define _C_SOUND_BEHAVIOR_H_

#include "core/math/Vector3d.h"
#include "engine/behavior/CGameBehavior.h"
#include "audio/AudioStructs.h"
#include <vector>

class CAudioSource;
class CTransform;

class CSoundBehavior : public CGameBehavior
{

public:
	ENGINE_API CSoundBehavior ( void );
	ENGINE_API ~CSoundBehavior ( void );

	ENGINE_API void Update ( void );

	ENGINE_API void Play ( void );
	ENGINE_API void Stop ( void );
	ENGINE_API void SetLooped ( bool );

	ENGINE_API void SetGain ( float );
	ENGINE_API void SetPitch ( float );

	ENGINE_API const static std::vector<CSoundBehavior*> & GetSoundList ( void );

public:
	CAudioSource*	mySource;
	bool			deleteWhenDone;
	Vector3d		position;
	Vector3d		velocity;

	string			filename;

	AudioStructs::soundScript_aialert	ai_alert_amount;
	AudioStructs::soundScript_channel	channel;

	CTransform*		parent;

private:
	static std::vector<CSoundBehavior*>	soundList;
};

#endif//_C_SOUND_BEHAVIOR_H_