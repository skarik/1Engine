
#ifndef _PHYS_WRAPPER_QUATERNION_H_
#define _PHYS_WRAPPER_QUATERNION_H_

#include "wrapper_common.h"
#include "physVector4.h"

class physQuaternion : public physVector4
{
public:
	FORCE_INLINE physQuaternion ( void )
	{
		;
	}
	FORCE_INLINE physQuaternion ( physReal x, physReal y, physReal z=0.0, physReal w=0.0 )
	{
		storage.x = x;
		storage.y = y;
		storage.z = z;
		storage.w = w;
	}
};

#endif//_PHYS_WRAPPER_QUATERNION_H_