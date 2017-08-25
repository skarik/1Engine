
#ifndef _PHYS_WRAPPER_AABB_H_
#define _PHYS_WRAPPER_AABB_H_

#include "wrapper_common.h"
#include "physVector4.h"

class physAabb
{
public:
	physVector4 m_min;
	physVector4 m_max;
};

#endif//_PHYS_WRAPPER_AABB_H_