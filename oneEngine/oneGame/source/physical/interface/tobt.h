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
#include "core/math/Vector3d.h"
#include "core/math/Quaternion.h"

//#include "bullet/Bullet3Common/b3Vector3.h"
#include "bullet/LinearMath/btVector3.h"
#include "bullet/LinearMath/btQuaternion.h"

namespace physical
{
	FORCE_INLINE btVector3 bt ( const Vector3f& vect )
		{ return btVector3(vect.x, vect.y, vect.z); }

	FORCE_INLINE btQuaternion bt ( const Quaternion& quat )
		{ return btQuaternion(quat.x, quat.y, quat.z, quat.w); }


	FORCE_INLINE Vector3f ar ( const btVector3& vect )
		{ return Vector3f(vect.x(), vect.y(), vect.z()); }

	FORCE_INLINE Quaternion ar ( const btQuaternion& quat )
		{ return Quaternion(quat.x(), quat.y(), quat.z(), quat.w()); }
}

#endif//PHYSICAL_INTERFACE_TO_BULLET_H_