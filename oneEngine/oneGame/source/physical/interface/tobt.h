//===============================================================================================//
//
//		tobt.h : to Bullet
//
// Provides a large amount of overloaded 1Engine -> Bullet conversions.
// Do NOT include this in a header. This is explicitly a CPP only header.
//
//===============================================================================================//
#ifndef PHYSICAL_INTERFACE_TO_BULLET_H_
#define PHYSICAL_INTERFACE_TO_BULLET_H_

#include "core/types.h"
#include "core/math/Vector3.h"
#include "core/math/Quaternion.h"
#include "core/math/Rotator.h"
#include "core/math/XTransform.h"

//#include "bullet/Bullet3Common/b3Vector3.h"
#include "bullet/LinearMath/btVector3.h"
#include "bullet/LinearMath/btQuaternion.h"
#include "bullet/LinearMath/btMatrix3x3.h"
#include "bullet/LinearMath/btTransform.h"

namespace physical
{
	FORCE_INLINE btVector3 bt ( const Vector3f& vect )
		{ return btVector3(vect.x, vect.y, vect.z); }

	FORCE_INLINE btQuaternion bt ( const Quaternion& quat )
		{ return btQuaternion(quat.x, quat.y, quat.z, quat.w); }

	FORCE_INLINE btMatrix3x3 bt ( const Matrix3x3& matx )
		{ return btMatrix3x3(
			matx[0][0], matx[0][1], matx[0][2],
			matx[1][0], matx[1][1], matx[1][2],
			matx[2][0], matx[2][1], matx[2][2]); }

	FORCE_INLINE btTransform bt ( const XrTransform& trans )
		{ return btTransform(bt(trans.rotation), bt(trans.position)); }

	FORCE_INLINE Vector3f ar ( const btVector3& vect )
		{ return Vector3f(vect.x(), vect.y(), vect.z()); }

	FORCE_INLINE Quaternion ar ( const btQuaternion& quat )
		{ return Quaternion(quat.x(), quat.y(), quat.z(), quat.w()); }
}

#endif//PHYSICAL_INTERFACE_TO_BULLET_H_