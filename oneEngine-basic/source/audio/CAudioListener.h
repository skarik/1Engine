

#ifndef _C_AUDIO_LISTENER_H_
#define _C_AUDIO_LISTENER_H_

#include "core/math/Vector3d.h"

class CAudioListener
{
public:
	AUDIO_API CAudioListener ( void );
	AUDIO_API ~CAudioListener ( void );

	AUDIO_API void Update ( void );
	AUDIO_API void Destroy ( void );

public:
	Vector3d position;
	Vector3d velocity;
	Vector3d orient_forward;
	Vector3d orient_up;

	bool queue_destruction;
private:
	Vector3d position_prev;
	

};

#endif