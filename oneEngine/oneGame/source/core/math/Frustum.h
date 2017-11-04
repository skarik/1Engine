#ifndef CORE_MATH_FRUSTUM_H_
#define CORE_MATH_FRUSTUM_H_

// Includes
#include "Plane.h"

// Class definition
namespace core
{
	namespace math
	{
		class Frustum
		{
		public:
			Plane plane [6];
		};
	}
}

#endif//CORE_MATH_FRUSTUM_H_