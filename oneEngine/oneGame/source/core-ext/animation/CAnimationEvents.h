//===============================================================================================//
//
//		core-ext/animation/CAnimationEvents.h
//
// Contains animation event information.
//
//===============================================================================================//
#ifndef CORE_ANIMATIONS_ANIMATION_EVENTS_H_
#define CORE_ANIMATIONS_ANIMATION_EVENTS_H_

#include "core/types/types.h"

namespace animation
{
	enum arAnimSystemEvent : uint16_t
	{
		kAnimEventAttack,
		kAnimEventFootstep,
		kAnimEventFootstepLeft,
		kAnimEventFootstepRight,
		kAnimEventClangCheck,

		kAnimEventSlide,
		kAnimEventSlideStart,
		kAnimEventSlideEnd,

		kAnimEventINVALID = 0xFFFF
	};
	
	typedef uint16_t arTag;
	enum arAnimEventTags : arTag
	{
		kAnimEventTagNone		= arTag(-1),
		kAnimEventTagDefault	= 0,
	};

	// Event class definition
	class ActionEvent
	{
	public:
		arAnimSystemEvent	type;
		arTag				data;
		Real				frame;
	};
};

#endif//CORE_ANIMATIONS_ANIMATION_EVENTS_H_