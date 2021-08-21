//===============================================================================================//
//
//		Math.h
//
// Provides repeatedly used and some not-so-much used math operations in the math:: namespace.
//
// Math namespace. Straighten up, you're about to get some learning on!
// Provides repeatedly used math operations on basic types, such as Linear Interpolation or Cubic
//	Interpolation (Smoothlerp).
// Provides some not-so-much used math operations that are still used repeatedly, such as
//	generating a non-random sequence of points on a sphere.
//
//===============================================================================================//
#ifndef CORE_MATH_H_
#define CORE_MATH_H_

#include "core/types/types.h"
#include "core/types/float.h"
#include "Vector3.h"

#include <cmath>
#ifdef _MSC_VER
#	include <intrin.h>
#endif

namespace math
{
	//
	// Values:
	//

	//	saturate ( float ) : clamps the value before 0 and 1
	template <typename Number> inline
		constexpr Number saturate ( Number f ) noexcept
	{
		f =    ( (f>1) ? 1 : f );
		return ( (0>f) ? 0 : f );
	}
	//	clamp ( float value, lower, upper ) : clamps the value between the two input numbers
	template <typename Number> inline
		constexpr Number clamp ( Number val, Number lower, Number upper ) noexcept
	{
		val = ( (val > upper) ? upper : val );
		return  (lower > val) ? lower : val;
	}
	//	wrap_max ( float value, max ) : wraps the value to range of 0 to max
	template <typename Number> inline
		constexpr Number wrap_max ( Number val, Number max ) noexcept
	{
		return (Number)fmod(max + fmod(val, max), max);
	}
	//	wrap ( float value, lower, upper ) : wraps the value to range of lower to upper
	template <typename Number> inline
		constexpr Number wrap ( Number val, Number lower, Number upper ) noexcept
	{
		return lower + wrap_max<Number>(val - lower, upper - lower);
	}


	//
	// Operations:
	//

	//	sgn ( number ) : finds sign of the input value
	template <typename Number> inline
		constexpr int sgn(Number val) noexcept
	{
		return (Number(0) < val) - (val < Number(0));
	}

	//	square ( number ) : returns value to the second power
	template <typename Number> inline
		constexpr Number square(Number val) noexcept
	{
		return sqr(val);
	}

	//	cube ( number ) : returns value to the third power
	template <typename Number> inline
		constexpr Number cube(Number val) noexcept
	{
		return cub(val);
	}

	//	round ( number ) : rounds value to the neearest integer
	template <typename Number> inline
		constexpr int32_t round(Number r) noexcept
	{
		return (int32_t)((r > 0.0) ? (r + 0.5) : (r - 0.5)); 
	}

	//	round ( number ) : rounds value to the neearest integer
	template <typename Number> inline
		constexpr int32_t floor(Number r) noexcept
	{
		int32_t i = (int32)r;
		return (r < i) ? (i - 1) : i;
	}

	//	fract ( number ) : rounds value to the neearest integer
	template <typename Number> inline
		constexpr Number fract(Number r) noexcept
	{
		return r - floor(r);
	}

	//	log2 ( integer ) : Very fast Log2.
	// It has a very large chance of failure on non-powers of two. Don't fuck up.
	inline
		uint32_t log2 ( const uint32_t x ) noexcept
	{
#ifdef _MSC_VER
		unsigned long result = 0;
		_BitScanReverse( &result, (unsigned long)(x) );
		return uint32_t(result);
		/*uint32_t y;
		__asm {
			bsr eax, x
			mov y, eax
		};
		return y;*/
#endif
#ifdef __clang__
		return __builtin_ctz (x); // should work on ARM
#endif
	}

	//	exp2 ( integer ) : pretty fast integer Exp2.
	// No way to screw this up
	inline
		constexpr uint32_t exp2 ( const uint32_t x ) noexcept
	{
		return (1<<x);
	}

	//
	// Interpolation:
	//

	template <typename Number> inline
		constexpr Number lerp ( Number t, Number a, Number b ) noexcept
	{
		return ( a+t*(b-a) );
	}
	template <typename Number> inline
		constexpr Number lerp_clamp ( Number t, Number a, Number b ) noexcept
	{
		return lerp( saturate(t), a, b );
	}

	template <typename Number> inline
		constexpr Number smoothlerp ( Number t )
	{
		return ( t * t * (3 - 2 * t) );
	}
	template <typename Number> inline
		constexpr Number smoothlerp ( Number t, Number a, Number b ) noexcept
	{
		return lerp( smoothlerp(t), a, b );
	}


	//
	// Other:
	//

	//	largest ( a, b ) : Returns larger (magnitude) of the two numbers
	template <typename Number> inline
		constexpr Number largest ( Number a, Number b ) noexcept
	{
		return (std::abs(a) > std::abs(b)) ? a : b;
	}

	//	smallest ( a, b ) : Returns smaller (magnitude) of the two numbers
	template <typename Number> inline
		constexpr Number smallest ( Number a, Number b ) noexcept
	{
		return (std::abs(a) < std::abs(b)) ? a : b;
	}

	//
	// Helpers:
	// TODO: move elsewhere

	// pointsOnSphere ( int NumberOfPoints )
	// Generates a list of equidistant points on a sphere. Usable for explosive ideas.
	// Gives away ownership of the returned array. Remember to delete it.
	Vector3f* points_on_sphere ( int N );

};


#endif//CORE_MATH_H_