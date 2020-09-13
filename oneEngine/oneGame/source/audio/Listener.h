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
		Vector3f			position;
		Vector3f			velocity;
		Vector3f			orient_forward;
		Vector3f			orient_up;

		bool				queue_destruction;
	private:
		Vector3f			position_prev;
	};
}

#endif//AUDIO_LISTENER_H_