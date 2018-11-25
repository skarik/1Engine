//===============================================================================================//
//
//		class Vector4[f/d]
//
// POD vector class, 16/32 bytes.
// Represents a 4-component floating point vector.
//
//===============================================================================================//
#ifndef CORE_MATH_VECTOR4_H_
#define CORE_MATH_VECTOR4_H_

#include "core/debug.h"
#include "core/types/types.h"
#include "core/types/float.h"
#include <iostream>
#include <cmath>

class Matrix4x4;

namespace core
{
	template <typename Float> class Vector2_T;
	template <typename Float> class Vector3_T;
	
	// 4D Vector Math Class
	template <typename Float>
	class Vector4_T
	{
		friend std::ostream& operator<< (std::ostream& out, Vector4_T<Float> const& current);		//Out stream overload
	
	public:
		FORCE_INLINE			Vector4_T (void)
			: x(0), y(0), z(0), w(0)
			{}
		template <typename FloatOther>
		FORCE_INLINE			Vector4_T (Vector4_T<FloatOther> const& old)
			: x((Float)old.x), y((Float)old.y), z((Float)old.z), w((Float)old.w)
			{}
		FORCE_INLINE			Vector4_T (Float const& new_x, Float const& new_y, Float const& new_z, Float const& new_w = 0)
			: x(new_x), y(new_y), z(new_z), w(new_w)
			{}
		FORCE_INLINE			Vector4_T (const Float* arr)
			: x(0), y(0), z(0), w(0)
		{
			if (arr != NULL)
			{
				x = arr[0];
				y = arr[1];
				z = arr[2];
				w = arr[3];
			}
		}
		template <typename FloatOther>
								Vector4_T (Vector3_T<FloatOther> const& old); // Defined in CPP.

		template <typename FloatOther>
								Vector4_T (const Vector2_T<FloatOther>& part1, const Vector2_T<FloatOther>& part2 = Vector2_T<FloatOther>()); // Defined in CPP.

		FORCE_INLINE Vector4_T<Float>
								operator+ (Vector4_T<Float> const& right) const
		{
			return Vector4_T<Float>(x + right.x, y + right.y, z + right.z, w + right.w);
		}

		FORCE_INLINE void		operator+= (Vector4_T<Float> const& right)
		{
			x += right.x;
			y += right.y;
			z += right.z;
			w += right.w;
		}

		FORCE_INLINE Vector4_T<Float>
								operator- (Vector4_T<Float> const& right) const
		{
			return Vector4_T<Float>(x - right.x, y - right.y, z - right.z, w - right.w);
		}

		FORCE_INLINE void		operator-= (Vector4_T<Float> const& right)
		{
			x -= right.x;
			y -= right.y;
			z -= right.z;
			w -= right.w;
		}
		
		FORCE_INLINE Vector4_T<Float>
								operator* (Float const& right) const
		{
			return Vector4_T<Float>(x * right, y * right, z * right, w * right);
		}

		FORCE_INLINE void		operator*= (Float const& right)
		{
			x *= right;
			y *= right;
			z *= right;
			w *= right;
		}

		FORCE_INLINE Vector4_T<Float>
								operator/ (Float const& right) const
		{
			return Vector4_T<Float>(x / right, y / right, z / right, w / right);
		}

		FORCE_INLINE void		operator/= (Float const& right)
		{
			x /= right;
			y /= right;
			z /= right;
			w /= right;
		}

		FORCE_INLINE Vector4_T<Float>
								operator- () const
		{
			return Vector4_T<Float>(-x, -y, -z, -w);
		}

		FORCE_INLINE Float		dot (Vector4_T<Float> const& right) const
		{
			return x * right.x + y * right.y + z * right.z + w * right.w;
		}

		FORCE_INLINE Vector4_T<Float>
								normal ( void ) const
		{
			Float invMagnitude = magnitude();
			if (fabs(invMagnitude) <= FLOAT_PRECISION)
				return Vector4_T<Float>(0, 0, 0, 0);
			invMagnitude = 1.0F / invMagnitude;
			return (*this) * invMagnitude;
		}
	
		FORCE_INLINE void		normalize ( void )
		{
			(*this) = normal();
		}

		//	lerp(to, t) : Returns a new value lerped to the given value.
		// Note: this doesn't edit the values.
		FORCE_INLINE Vector4_T<Float>
								lerp (Vector4_T<Float> const& right, Float const& t)
		{
			if (t <= 0)
				return (*this);
			else if (t >= 1)
				return right;
			else
				return ((right - (*this)) * t + (*this));
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

		FORCE_INLINE Float		magnitude (void) const
		{
			return (Float)sqrt(x*x + y*y + z*z + w*w);
		}

		FORCE_INLINE Float		sqrMagnitude (void) const
		{
			return (x*x + y*y + z*z + w*w);
		}

		Vector4_T<Float>		rvrMultMatx (Matrix4x4 const& right) const; // Defined in CPP.

		FORCE_INLINE Vector4_T<Float>
								mulComponents (Vector4_T<Float> const& right) const
		{
			return Vector4_T<Float>(x * right.x, y * right.y, z * right.z, w * right.w);
		}
		FORCE_INLINE Vector4_T<Float>
								divComponents (Vector4_T<Float> const& right) const
		{
			return Vector4_T<Float>(x / right.x, y / right.y, z / right.z, w / right.w);
		}

		bool operator== (Vector4_T<Float> const& right) const
		{
			if (fabs(right.x - x) > FLOAT_PRECISION)
				return false;
			if (fabs(right.y - y) > FLOAT_PRECISION)
				return false;
			if (fabs(right.z - z) > FLOAT_PRECISION)
				return false;
			if (fabs(right.w - w) > FLOAT_PRECISION)
				return false;
			return true;
		}

		bool operator!= (Vector4_T<Float> const& right) const
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
				Float w;
			};
			struct
			{
				Float raw [4];
			};
		};
	};
}

// 32-bit float 4D vector class
typedef core::Vector4_T<Real32> Vector4f;
// 64-bit float 4D vector class
typedef core::Vector4_T<Real64> Vector4_d;

#endif//CORE_MATH_VECTOR4_H_