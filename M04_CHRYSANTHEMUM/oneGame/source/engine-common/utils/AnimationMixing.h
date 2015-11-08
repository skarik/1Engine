
#ifndef _ENGINE_COMMON_UTIL_ANIMATION_MIXING_H_
#define _ENGINE_COMMON_UTIL_ANIMATION_MIXING_H_

#include "core/types.h"

class CAnimation;
class CAnimAction;

namespace Animation
{
	ENGCOM_API int AddMixingTransform ( CAnimation& anim, CAnimAction& action, const char* boneName, bool recursive=false );

};

#endif//_ENGINE_COMMON_UTIL_ANIMATION_MIXING_H_