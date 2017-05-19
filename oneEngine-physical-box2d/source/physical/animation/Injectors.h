#ifndef _PHYS_ANIMATION_INJECTORS_
#define _PHYS_ANIMATION_INJECTORS_

#include "core/types.h"
#include "core/math/Math3d.h"

namespace animation
{
	enum splitMode_t
	{
		SPLIT_123_6,
		SPLIT_234_9,
	};

	// Aiming information
	struct injectorAimer_t
	{
		Vector3d rotation_spine;
		Vector3d rotation_neck;
		Vector3d rotation_head;
		// Controls if shoulder spin is used to split spine rotation.
		bool spine_split_to_last;
		// Controls what table is used to pull split values
		splitMode_t spine_split_mode;
	};

	// Lookat information
	struct injectorLookat_t
	{
		Rotator rotation_eye;
	};

	// Jigglebone animation
	struct injectorJiggle_t
	{
		int32_t target;
	};
}

#endif//_PHYS_ANIMATION_INJECTORS_