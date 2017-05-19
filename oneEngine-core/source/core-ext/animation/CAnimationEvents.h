
#ifndef _C_ANIMATION_EVENTS_H_
#define _C_ANIMATION_EVENTS_H_

#include "core/types/types.h"

namespace animation
{
	enum eAnimSystemEvent : uint16_t
	{
		Event_Attack,
		Event_Footstep,
		Event_Footstep_Left,
		Event_Footstep_Right,
		Event_ClangCheck,

		Event_Slide,
		Event_SlideStart,
		Event_SlideEnd,

		Event_INVALID = 0xFFFF
	};
	
	typedef uint32_t tag_t;
	enum eAnimEventTags : tag_t
	{
		EventTag_NoTag = tag_t(-1),
		EventTag_Default = 0,
	};

	// Event class definition
	class ActionEvent
	{
	public:
		eAnimSystemEvent	type;
		tag_t				data;
		Real				frame;
	};
};

#endif//_C_ANIMATION_EVENTS_H_