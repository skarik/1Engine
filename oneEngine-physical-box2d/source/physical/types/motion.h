#ifndef PHYSICAL_TYPES_MOTION_H_
#define PHYSICAL_TYPES_MOTION_H_

#include "core/types/types.h"

namespace physical
{
	namespace motion
	{
		// prMotionTypes : kinds of motion for rigidbodies
		enum prMotionTypes : uint8_t
		{
			kMotionDynamic		= 0x00,
			kMotionStatic		= 0x01,
			kMotionKinematic	= 0x02,
		};
	}
};

#endif//PHYSICAL_TYPES_MOTION_H_