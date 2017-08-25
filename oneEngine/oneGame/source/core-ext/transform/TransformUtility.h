#ifndef _C_TRANSFORM_UTILITY_H_
#define _C_TRANSFORM_UTILITY_H_

// Includes
#include <string>
#include <vector>

#include "core/common.h"
#include "core/types/float.h"
#include "core/math/Vector2d.h"
#include "core/math/Vector3d.h"
#include "core/math/Vector4d.h"
#include "core/math/Quaternion.h"
#include "core/math/matrix/CMatrix.h"
#include "core/math/Rotator.h"

#include "core/math/XTransform.h"

namespace core
{
	class TransformLite
	{
	private:
		// Transformation
		Matrix4x4	matx;
		Matrix4x4	matxLocal;
		// Rotation
		Matrix4x4	matxRot;
		Matrix4x4	matxLocalRot;

	public:
		XrTransform world;
		XrTransform local;

	public:
		CORE_API	void UpdateWorldFromLocal ( const TransformLite* parent );
		CORE_API	void UpdateLocalFromWorld ( const TransformLite* parent=NULL );

		const Matrix4x4& WorldMatrix ( void ) { return matx; }
		const Matrix4x4& WorldRotation ( void ) { return matxRot; }
		const Matrix4x4& LocalMatrix ( void ) { return matxLocal; }
		const Matrix4x4& LocalRotation ( void ) { return matxLocalRot; }
	};

	class TransformUtility
	{
	public:
		CORE_API static void WorldToLocal (
			const Matrix4x4& parent_inv, const Rotator& parent_rot_inv, const Vector3d& parent_scale,
			const Vector3d& position, const Rotator& rotation, const Vector3d& scale,
			Vector3d& local_position, Rotator& local_rotation, Vector3d& local_scale );

		static void TRSToMatrix4x4 (
			const XrTransform& trs,
			Matrix4x4& transform, Matrix4x4& transform_rot )
		{
			TRSToMatrix4x4( trs.position, trs.rotation, trs.scale, transform, transform_rot );
		}
		CORE_API static void TRSToMatrix4x4 (
			const Vector3d& position, const Rotator& rotation, const Vector3d& scale,
			Matrix4x4& transform, Matrix4x4& transform_rot );

		CORE_API static void LocalToWorld (
			const int32_t* parents, const XTransform* local, TransformLite* result,
			const int count );
		CORE_API static void WorldToLocal (
			const int32_t* parents, const XTransform* local, TransformLite* result,
			const int count );
	};
};

#endif//_C_TRANSFORM_UTILITY_H_