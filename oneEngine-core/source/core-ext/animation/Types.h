#ifndef _ANIMATIONS_TYPES_H_
#define _ANIMATIONS_TYPES_H_

#include "core/types/types.h"

namespace Animation
{
	//		Animation Category
	// Used for general categorization of animations for organizing.
	enum types_t : uint8_t
	{
		TYPE_IDLE,
		TYPE_FOOT,
		TYPE_FALL,
		TYPE_SWIM,
		TYPE_ATTACK,
		TYPE_BLOCK,
		TYPE_CAST,
		TYPE_HURT
	};
};

#endif//_ANIMATIONS_TYPES_H_