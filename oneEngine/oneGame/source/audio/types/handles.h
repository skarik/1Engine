#ifndef AUDIO_HANDLES_H_
#define AUDIO_HANDLES_H_

#include "audio/AudioHeaders.h"

namespace audio
{
	typedef uint32_t arBufferHandle;
	typedef uint32_t arSourceHandle;
	typedef uint32_t arReturnCode;
//#ifdef _AUDIO_FMOD_
//	typedef FMOD::FMOD_SOUND*	arBufferHandle;
//	typedef FMOD::FMOD_CHANNEL*	arSourceHandle;
//#	define BUFFER_NULL NULL
//#	define HANDLE_NULL NULL
//
//	typedef unsigned int arReturnCode;
//#else
//	typedef ALuint arBufferHandle;
//	typedef ALuint arSourceHandle;
//#	define BUFFER_NULL NIL
//#	define HANDLE_NULL NIL
//
//	typedef ALuint arReturnCode;
//#endif
}

#endif//AUDIO_HANDLES_H_