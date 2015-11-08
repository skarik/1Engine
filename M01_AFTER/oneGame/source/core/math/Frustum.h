

#ifndef _C_FRUSTUM_
#define _C_FRUSTUM_

// Includes
#include "Plane.h"

// Class definition
namespace Maths
{
	class Frustum
	{
	public:
		Plane planes [6];
	};
}

#endif