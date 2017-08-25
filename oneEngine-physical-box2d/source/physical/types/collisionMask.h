#ifndef PHYSICAL_TYPES_COLLISION_MASK_H_
#define PHYSICAL_TYPES_COLLISION_MASK_H_

#include "core/types/types.h"

namespace physical
{
	struct prCollisionMask
	{
		unsigned int layer : 16;
		unsigned int group : 8;
		unsigned int id : 8;
	};
}

#endif//PHYSICAL_TYPES_COLLISION_MASK_H_