
#ifndef _ENGINE_COMMON_UTIL_ANIMATION_MIXING_H_
#define _ENGINE_COMMON_UTIL_ANIMATION_MIXING_H_

#include "core/types.h"

class AnimationControl;
class AnimationAction;

namespace animation
{
	ENGCOM_API int AddMixingTransform ( AnimationControl& anim, AnimationAction& action, const char* boneName, bool recursive=false );

};

#endif//_ENGINE_COMMON_UTIL_ANIMATION_MIXING_H_