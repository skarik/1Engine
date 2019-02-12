#ifndef CORE_MATH_PLANE_
#define CORE_MATH_PLANE_

// Includes
#include "core/types/float.h"
#include "Vector3.h"

// Class Def
namespace core
{
	namespace math
	{
		class Plane
		{
		public:
			Vector3f n;
			Real d;

		public:
			// Constructors
			Plane ( void );
			Plane ( Vector3f * );
			void ConstructFromPoints ( Vector3f * );
			void ConstructFromPoints ( Vector3f const&, Vector3f const&, Vector3f const& );
		};
	}
}

#endif CORE_MATH_PLANE_