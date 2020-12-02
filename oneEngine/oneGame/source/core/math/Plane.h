#ifndef CORE_MATH_PLANE_
#define CORE_MATH_PLANE_

#include <algorithm>
#include "core/types/float.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"
#include "Matrix/CMatrix.h"
#include "Plane.h"
#include "Ray.h"
#include "Math.h"
#include "vect3d_template.h"

namespace core {
namespace math
{
	class Plane
	{
	public:
		Vector3f			n;
		Real				d;

	public:
		explicit			Plane ( void )
		{
			n = Vector3f( 0,0,1 );
			d = 0;
		}

		explicit			Plane ( Vector3f* points )
		{
			ConstructFromPoints(points);
		}

		explicit			Plane ( Vector3f position, Vector3f normal )
		{
			n = normal;
			d = -position.x * n.x - position.y * n.y - position.z * n.z;
		}

		FORCE_INLINE void	ConstructFromPoints ( Vector3f* points )
		{
			ConstructFromPoints(points[0], points[1], points[2]);
		}

		FORCE_INLINE void	ConstructFromPoints ( const Vector3f& point0, const Vector3f& point1, const Vector3f& point2 )
		{
			n = (point1 - point0).cross(point2 - point0);
			n.normalize();
			d = -point1.x * n.x - point1.y * n.y - point1.z * n.z;
		}

		FORCE_INLINE Real	DistanceToPoint ( const Vector3f& point ) const
		{
			return n.dot( point ) + d;
		}

		//	Raycast( ray ) : Checks if ray hits the box
		FORCE_INLINE bool	Raycast( const Ray& ray ) const
		{
			Real unused_distance;
			return Raycast(ray, unused_distance);
		}

		//	Raycast( ray, out_distance ) : Checks if ray hits the box, sets distance to the box if so.
		FORCE_INLINE bool	Raycast( const Ray& ray, Real& out_distance ) const
		{
			float d_t = n.dot(ray.dir);
			
			// Parallel case
			if (std::abs(d_t) < FLOAT_PRECISION)
			{
				out_distance = INFINITY;
				return false;
			}

			// Get offset to center
			Vector3f d_center = Vector3f(0, 0, -d) - ray.pos;
			float t = d_center.dot(n);

			// Apply parametric divide thingy
			out_distance = t / d_t;
			// And if it's bigger than zero, it hits. Otherwise, it's the other direciton.
			return out_distance > 0;
		}
	};
}}

#endif CORE_MATH_PLANE_