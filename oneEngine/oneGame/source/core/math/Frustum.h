#ifndef CORE_MATH_FRUSTUM_H_
#define CORE_MATH_FRUSTUM_H_

// Includes
#include "Plane.h"

// Class definition
namespace core
{
	namespace math
	{
		enum arShapeCheckResult
		{
			kShapeCheckResultOutside = 0,
			kShapeCheckResultIntersect = 1,
			kShapeCheckResultInside = 2,
		};
		class BoundingBox;
		class Frustum
		{
		public:
			Plane plane [6];

			//	PointIsInside() : Check if the point lies within the frustum.
			FORCE_INLINE arShapeCheckResult
									PointIsInside ( const Vector3d& point );
			//	SphereIsInside() : Checks if the sphere lies within the frustum.
			FORCE_INLINE arShapeCheckResult
									SphereIsInside ( const Vector3d& center, const Real radius );
			//	BoundingBoxIsInside() : Checks if bounding box is inside the frustum.
			// NOTE: Currently this only will return Partial or Outside.
			FORCE_INLINE arShapeCheckResult
									BoundingBoxIsInside ( const BoundingBox& box );

		public:
			//	BuildFromProjectionMatrix() : Creates a frustum from the input view-projection matrix.
			// Can be used to build a frustum for use in frustum culling.
			// Arguments:
			//	viewprojMatrix:	View projection matrix to build frustum from.
			//					Can be taken from rrCameraPass.
			// Returns:
			//	Frustum built from the view-projection matrix in world space.
			static Frustum			BuildFromProjectionMatrix ( const Matrix4x4& viewprojMatrix );
		};
	}
}

#endif//CORE_MATH_FRUSTUM_H_