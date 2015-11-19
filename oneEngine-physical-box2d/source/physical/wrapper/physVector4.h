
#ifndef _PHYS_WRAPPER_VECTOR4_H_
#define _PHYS_WRAPPER_VECTOR4_H_

#include "wrapper_common.h"

class physVector4
{
public:
	Vector4d storage;

	FORCE_INLINE void set ( physReal x, physReal y, physReal z, physReal w=0.0 )
	{
			storage.x = x;
			storage.y = y;
			storage.z = z;
			storage.w = w;
	}
};

#endif//_PHYS_WRAPPER_VECTOR4_H_