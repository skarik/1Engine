#ifndef RENDERER_TYPES_ID_H_
#define RENDERER_TYPES_ID_H_

#include "core/types.h"
#include <stdint.h>

struct rrId
{
	static const int	kWorldInvalid = UINT8_MAX;

	uint8				world_index = kWorldInvalid;
	uint8				unused;
	uint16				object_index = UINT16_MAX;

	//	IsInWorld() : Returns if this ID indicates this item has been placed in a world.
	FORCE_INLINE bool		IsInWorld ( void ) const
	{
		return world_index != kWorldInvalid;
	}

	FORCE_INLINE bool		operator== ( const rrId& other ) const
	{
		return other.world_index == world_index 
			&& other.object_index == object_index;
	}
};

#endif//RENDERER_TYPES_ID_H_