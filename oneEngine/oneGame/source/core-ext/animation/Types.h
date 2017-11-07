//===============================================================================================//
//
//		core-ext/animation/Types.h
//
// Contains common types for animation system.
//
//===============================================================================================//
#ifndef CORE_ANIMATIONS_TYPES_H_
#define CORE_ANIMATIONS_TYPES_H_

#include "core/types/types.h"

namespace animation
{
	//		Animation Category
	// Used for general categorization of animations for organizing.
	enum arAnimType : uint8_t
	{
		kTypeIdle,
		kTypeFoot,
		kTypeFall,
		kTypeSwim,
		kTypeAttack,
		kTypeBlock,
		kTypeCast,
		kTypeHurt
	};
};

#endif//CORE_ANIMATIONS_TYPES_H_