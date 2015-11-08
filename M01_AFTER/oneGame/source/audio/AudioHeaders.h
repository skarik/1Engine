
#ifndef _AUDIO_FMOD_H_
#define _AUDIO_FMOD_H_

#define _AUDIO_FMOD_

#ifdef _AUDIO_FMOD_

namespace FMOD
{
	#include "fmod/fmod.h"
};

#else

#include "AL\al.h"
#include "AL\alc.h"

#endif


#endif//_AUDIO_FMOD_H_