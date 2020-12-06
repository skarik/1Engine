//===============================================================================================//
//
//		class Vector3[f/d]
//
// POD vector class, 12/24 bytes.
// Represents a 3-component floating point vector.
//
//===============================================================================================//
#ifndef CORE_MATH_VECTOR3_H_
#define CORE_MATH_VECTOR3_H_

#include "core/debug.h"
#include "core/types/types.h"
#include "core/types/float.h"
#include <iostream>
#include <cmath>

class Matrix3x3;
class Matrix4x4;

namespace core
{
	template <typename Float> class Vector2_T;
	template <typename Float> class Vector4_T;

	// 3D Vector Math Class
	template <typename Float>
	class Vector3_T
	{
		// Out stream overload
		friend std::ostream& operator<< (std::ostream& out, Vector3_T<Float> const& current);
	
	public:
		FORCE_INLINE			Vector3_T ( void )
			: x(0), y(0), z(0)
			{}

		template <typename FloatOther>
								Vector3_T (Vector2_T<FloatOther> const& old, Float const new_z = 0); // Defined in CPP.

		template <typename FloatOther>
		FORCE_INLINE			Vector3_T (Vector3_T<FloatOther> const& old)
			: x((Float)old.x), y((Float)old.y), z((Float)old.z)
			{}

		template <typename FloatOther>
								Vector3_T (Vector4_T<FloatOther> const& old); // Defined in CPP.

		FORCE_INLINE			Vector3_T (Float const& new_x, Float const& new_y, Float const& new_z)
			: x(new_x), y(new_y), z(new_z)
			{}

		FORCE_INLINE			Vector3_T (const Float* arr)
			: x(0), y(0), z(0)
		{
			if (arr != NULL)
			{
				x = arr[0];
				y = arr[1];
				z = arr[2];
			}
		}

		FORCE_INLINE Vector3_T<Float>
								operator+ (Vector3_T<Float> const& right) const
		{
			return Vector3_T<Float>(x + right.x, y + right.y, z + right.z);
		}

		FORCE_INLINE void		operator+= (Vector3_T<Float> const& right)
		{
			x += right.x;
			y += right.y;
			z += right.z;
		}

		FORCE_INLINE Vector3_T<Float>
								operator- (Vector3_T<Float> const& right) const
		{
			return Vector3_T<Float>(x - right.x, y - right.y, z - right.z);
		}

		FORCE_INLINE void		operator-= (Vector3_T<Float> const& right)
		{
			x -= right.x;
			y -= right.y;
			z -= right.z;
		}

		FORCE_INLINE Vector3_T<Float>
								operator* (Float const& right) const
		{
			return Vector3_T<Float>(x * right, y * right, z * right);
		}

		FORCE_INLINE void		operator*= (Float const& right)
		{
			x *= right;
			y *= right;
			z *= right;
		}

		FORCE_INLINE Vector3_T<Float>
								operator/ (Float const& right) const
		{
			return Vector3_T<Float>(x / right, y / right, z / right);
		}

		FORCE_INLINE void		operator/= (Float const& right)
		{
			x /= right;
			y /= right;
			z /= right;
		}

		FORCE_INLINE Vector3_T<Float>
								operator- () const
		{
			return Vector3_T<Float>(-x, -y, -z);
		}

		FORCE_INLINE Float		dot (Vector3_T<Float> const& right) const
		{
			return x * right.x + y * right.y + z * right.z;
		}

		FORCE_INLINE Vector3_T<Float>
								cross (Vector3_T<Float> const& right) const
		{
			return Vector3_T<Float>(y * right.z - z * right.y, 
									z * right.x - x * right.z,
									x * right.y - y * right.x);
		}

		//	lerp(to, t) : Returns a new value lerped to the given value.
		// Note: this doesn't edit the values.
		FORCE_INLINE Vector3_T<Float>
								lerp (Vector3_T<Float> const& right, Float const& t) const
		{
			if (t <= 0)
				return (*this);
			else if (t >= 1)
				return right;
			else
				return ((right - (*this)) * t + (*this));
		}

		FORCE_INLINE Vector3_T<Float>
								normal (void) const
		{
			Float invMagnitude = magnitude();
			if (fabs(invMagnitude) <= FLOAT_PRECISION)
				return Vector3_T<Float>::zero;
			invMagnitude = 1.0F / invMagnitude;
			return (*this)*invMagnitude;
		}

		FORCE_INLINE void		normalize (void)
		{
			(*this) = normal();
		}

		FORCE_INLINE Float		magnitude (void) const
		{
			return (Float)sqrt(x*x + y*y + z*z);
		}

		FORCE_INLINE Float		sqrMagnitude (void) const
		{
			return (x*x + y*y + z*z);
		}

		FORCE_INLINE Float&		operator[] (const int i)
		{
			return raw[i];
		}

		FORCE_INLINE const Float&
								operator[] (const int i) const
		{
			return raw[i];
		}

		Vector3_T<Float>		toEulerAngles ( void ) const
		{
			ARCORE_ERROR("Out of date function call. Reevaluate.");
			return Vector3_T<Float>(0,
									-(Real)radtodeg( atan2(z, sqrt((x * x) + (y * y))) ),
									 (Real)radtodeg( atan2(y, x) ));
		}

		Vector3_T<Float> rvrMultMatx ( Matrix4x4 const& right ) const; // Defined in CPP.
		Vector3_T<Float> rvrMultMatx ( Matrix3x3 const& right ) const; // Defined in CPP.

		Vector3_T<Float>		mulComponents ( Vector3_T<Float> const& right ) const
		{
			return Vector3_T<Float>(x * right.x, y * right.y, z * right.z);
		}

		Vector3_T<Float>		divComponents ( Vector3_T<Float> const& right ) const
		{
			return Vector3_T<Float>(x / right.x, y / right.y, z / right.z);
		}

		FORCE_INLINE bool		operator== (Vector3_T<Float> const& right) const
		{
			if (fabs(right.x - x) > FLOAT_PRECISION)
				return false;
			if (fabs(right.y - y) > FLOAT_PRECISION)
				return false;
			if (fabs(right.z - z) > FLOAT_PRECISION)
				return false;
			return true;
		}

		FORCE_INLINE bool		operator!= (Vector3_T<Float> const& right) const
		{
			return !((*this) == right);
		}

	public:
		union
		{
			struct 
			{
				Float x;
				Float y;
				Float z;
			};
			struct
			{
				Float raw [3];
			};
		};

	public:
		// A vector of value Vector3_T<Float>( 0,0,0 )
		static const Vector3_T<Float> zero;
		// A vector of value Vector3_T<Float>::forward
		static const Vector3_T<Float> forward;
		// A vector of value Vector3_T<Float>( 0,0,1 )
		static const Vector3_T<Float> up;
		// A vector of value Vector3_T<Float>( 0,1,0 )
		static const Vector3_T<Float> left;

	};
}

// 32-bit float 3D vector class
typedef core::Vector3_T<Real32> Vector3f;
// 64-bit float 3D vector class
typedef core::Vector3_T<Real64> Vector3_d;

#endif//CORE_MATH_VECTOR3_H_