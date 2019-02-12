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
		Number saturate ( Number f )
	{
		f =    ( (f>1) ? 1 : f );
		return ( (0>f) ? 0 : f );
	}
	//	clamp ( float value, lower, upper ) : clamps the value between the two input numbers
	template <typename Number> inline
		Number clamp ( Number val, Number lower, Number upper )
	{
		val = ( (val > upper) ? upper : val );
		return  (lower > val) ? lower : val;
	}
	//	wrap_max ( float value, max ) : wraps the value to range of 0 to max
	template <typename Number> inline
		Number wrap_max ( Number val, Number max )
	{
		return (Number)fmod(max + fmod(val, max), max);
	}
	//	wrap ( float value, lower, upper ) : wraps the value to range of lower to upper
	template <typename Number> inline
		Number wrap ( Number val, Number lower, Number upper )
	{
		return lower + wrap_max<Number>(val - lower, upper - lower);
	}


	//
	// Operations:
	//

	//	sgn ( number ) : finds sign of the input value
	template <typename Number> inline
		int sgn(Number val)
	{
		return (Number(0) < val) - (val < Number(0));
	}
	//	square ( number ) : returns value to the second power
	template <typename Number> inline
		Number square(Number val)
	{
		return sqr(val);
	}
	//	cube ( number ) : returns value to the third power
	template <typename Number> inline
		Number cube(Number val)
	{
		return cub(val);
	}
	//	round ( number ) : rounds value to the neearest integer
	template <typename Number> inline
		int32_t round(Number r)
	{
		return (int32_t)((r > 0.0) ? (r + 0.5) : (r - 0.5)); 
	}

	//	log2 ( integer ) : Very fast Log2.
	// It has a very large chance of failure on non-powers of two. Don't fuck up.
	inline
		uint32_t log2 ( const uint32_t x )
	{
#ifdef _MSC_VER
		unsigned long result;
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
		uint32_t exp2 ( const uint32_t x )
	{
		return (1<<x);
	}


	//
	// Interpolation:
	//

	inline
		Real lerp ( Real t, Real a, Real b )
	{
		return ( a+t*(b-a) );
	}
	inline
		Real lerp_clamp ( Real t, Real a, Real b )
	{
		return lerp( saturate(t), a, b );
	}

	inline
		Real smoothlerp ( Real t )
	{
		return ( t * t * (3 - 2 * t) );
	}
	inline
		Real smoothlerp ( Real t, Real a, Real b )
	{
		return lerp( smoothlerp(t), a, b );
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