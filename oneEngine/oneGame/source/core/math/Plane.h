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
			Plane ( void )
			{
				n = Vector3f( 0,0,1 );
				d = 0;
			}

			Plane ( Vector3f* points )
			{
				ConstructFromPoints(points);
			}

			FORCE_INLINE void ConstructFromPoints ( Vector3f* points )
			{
				ConstructFromPoints(points[0], points[1], points[2]);
			}

			FORCE_INLINE void ConstructFromPoints ( const Vector3f& point0, const Vector3f& point1, const Vector3f& point2 )
			{
				n = (point1-point0).cross(point2-point0);
				d = -point1.x*n.x-point1.y*n.y-point1.z*n.z;
			}

			FORCE_INLINE Real	DistanceToPoint ( const Vector3f& point ) const
			{
				return n.dot( point ) + d;
			}
		};
	}
}

#endif CORE_MATH_PLANE_