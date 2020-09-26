#ifndef AUDIO_LISTENER_H_
#define AUDIO_LISTENER_H_

#include "core/math/Vector3.h"

namespace audio
{
	class Manager;

	class Listener
	{
	private:
		friend Manager;
		AUDIO_API				~Listener ( void );

	public:
		AUDIO_API explicit		Listener ( void );

		AUDIO_API void			Update ( void );
		AUDIO_API void			Destroy ( void );

	public:
		Vector3f			position = Vector3f();
		Vector3f			velocity = Vector3f();
		Vector3f			orient_forward = Vector3f::forward;
		Vector3f			orient_up = Vector3f::up;

		bool				queue_destruction = false;
	private:
		Vector3f			position_prev = Vector3f();
	};
}

#endif//AUDIO_LISTENER_H_