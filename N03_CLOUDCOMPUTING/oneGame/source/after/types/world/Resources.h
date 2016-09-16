
#ifndef _RESOURCE_TYPES_H_
#define _RESOURCE_TYPES_H_

#include "core/types/types.h"

namespace World
{

	enum eResourceType : uint8_t
	{
		RESOURCE_WOOD = 0,
		RESOURCE_STONE,
		RESOURCE_FARMLAND,
		RESOURCE_LUXURY,

		RESOURCE_INVALID = 255
	};

};

#endif//_RESOURCE_TYPES_H_