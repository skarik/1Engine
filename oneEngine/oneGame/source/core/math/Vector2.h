//===============================================================================================//
//
//		class Vector2[f/d]
//
// POD vector class, 8/16 bytes.
// Represents a 2-component floating point vector.
//
//===============================================================================================//
#ifndef CORE_MATH_VECTOR2_H_
#define CORE_MATH_VECTOR2_H_

#include "core/types/types.h"
#include "core/types/float.h"
#include <iostream>
#include <cmath>

class Matrix2x2;

namespace core
{
	template <typename Float> class Vector3_T;

	// 2D Vector Math Class
	template <typename Float>
	class Vector2_T 
	{
		// Out stream overload
		friend std::ostream& operator<< (std::ostream& out, Vector2_T<Float> const& current);
	
	public:
		FORCE_INLINE			Vector2_T (void)
			: x(0), y(0)
			{}

		template <typename FloatOther>
		FORCE_INLINE			Vector2_T (Vector2_T<FloatOther> const& old)
			: x((Float)old.x), y((Float)old.y)
			{}
		
		template <typename FloatOther>
								Vector2_T (Vector3_T<FloatOther> const& old); // Defined in CPP.

		template <typename FloatOther>
		FORCE_INLINE			Vector2_T (FloatOther const& new_x, FloatOther const& new_y)
			: x((Float)new_x), y((Float)new_y)
			{}

		FORCE_INLINE Vector2_T<Float>
								operator+ (Vector2_T<Float> const& right) const
		{
			return Vector2_T<Float>(x + right.x, y + right.y);
		}

		FORCE_INLINE void		operator+= (Vector2_T<Float> const& right)
		{
			x += right.x;
			y += right.y;
		}

		FORCE_INLINE Vector2_T<Float>
								operator- (Vector2_T<Float> const& right) const
		{
			return Vector2_T<Float>(x - right.x, y - right.y);
		}

		FORCE_INLINE void		operator-= (Vector2_T<Float> const& right)
		{
			x -= right.x;
			y -= right.y;
		}

		FORCE_INLINE Vector2_T<Float>
								operator* (Float const& right) const
		{
			return Vector2_T<Float>(x * right, y * right);
		}

		FORCE_INLINE void		operator*= (Float const& right)
		{
			x *= right;
			y *= right;
		}
		
		FORCE_INLINE Vector2_T<Float>
								operator/ (Float const& right) const
		{
			return Vector2_T<Float>(x / right, y / right);
		}

		FORCE_INLINE void		operator/= (Float const& right)
		{
			x /= right;
			y /= right;
		}

		FORCE_INLINE Vector2_T<Float>
								operator- () const
		{
			return Vector2_T<Float>(-x, -y);
		}

		//	lerp(to, t) : Returns a new value lerped to the given value.
		// Note: this doesn't edit the values.
		FORCE_INLINE Vector2_T<Float>
								lerp (Vector2_T<Float> const& right, Float const& t) const
		{
			if (t <= 0)
				return (*this);
			else if (t >= 1)
				return right;
			else
				return ((right - (*this)) * t + (*this));
		}

		FORCE_INLINE Float		dot (Vector2_T<Float> const& right) const
		{
			return x * right.x + y * right.y;
		}

		FORCE_INLINE Float		cross (Vector2_T<Float> const& right) const
		{
			return x * right.y - y * right.x; 
		}

		FORCE_INLINE Float		magnitude (void) const
		{
			return (Float)sqrt(x*x + y*y);
		}

		FORCE_INLINE Float		sqrMagnitude (void) const
		{
			return x*x + y*y;
		}

		FORCE_INLINE Vector2_T<Float>
								normal (void) const
		{
			Float invMagnitude = magnitude();
			if (fabs(invMagnitude) <= FLOAT_PRECISION)
				return Vector2_T<Float>(0, 0);
			invMagnitude = 1.0F / invMagnitude;
			return (*this) * invMagnitude;
		}

		FORCE_INLINE void		normalize (void)
		{
			(*this) = normal();
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

		Vector2_T<Float>		rvrMultMatx (Matrix2x2 const& right) const; // Defined in CPP.
		
		Vector2_T<Float>		mulComponents (Vector2_T<Float> const& right) const
		{
			return Vector2_T<Float>(x * right.x, y * right.y);
		}

		Vector2_T<Float>		divComponents (Vector2_T<Float> const& right) const
		{
			return Vector2_T<Float>(x / right.x, y / right.y);
		}

		FORCE_INLINE bool		operator== (Vector2_T<Float> const& right) const
		{
			if (fabs(right.x - x) > FLOAT_PRECISION)
				return false;
			if (fabs(right.y - y) > FLOAT_PRECISION)
				return false;
			return true;
		}

		FORCE_INLINE bool		operator!= (Vector2_T<Float> const& right) const
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
			};
			struct
			{
				Float raw [2];
			};
		};
	};
}

// 32-bit float 2D vector class
typedef core::Vector2_T<Real32> Vector2f;
// 64-bit float 2D vector class
typedef core::Vector2_T<Real64> Vector2_d;

#endif//CORE_MATH_VECTOR2_H_