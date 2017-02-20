

#ifndef _MATH_H_
#define _MATH_H_

#include <cmath>

#include "core/types/types.h"
#include "core/types/float.h"
#include "Vector3d.h"

// class CMath
// Math class. Straighten up, you're about to get some learning on!
// Provides repeatedly used math operations on basic types, such as Linear Interpolation or Cubic Interpolation (Smoothlerp).
// Provides some not-so-much used math operations that are still used repeatedly, such as generating a non-random sequence of points on a sphere.

class CMath
{
public:
	Real Smoothlerp ( Real t );
	Real Smoothlerp ( Real t, Real a, Real b );

	// Performs a limited linear interpolation
	Real Lerp ( Real t, Real a, Real b );
	// Performs an unlimited linear interpolation
	Real lerp ( Real t, Real a, Real b );

	Real Clamp ( Real val, Real lower, Real upper );
	Real Wrap ( Real val, Real lower, Real upper );

	// pointsOnSphere ( int NumberOfPoints )
	// Generates a list of equidistant points on a sphere. Usable for explosive ideas.
	// Gives away ownership of the returned array. Remember to delete it.
	Vector3d* pointsOnSphere ( int N );

	template <typename T> int sgn(T val) {
		return (T(0) < val) - (val < T(0));
	}
	template <typename T> T Sqr(T val) {
		return sqr(val);
	}
	template <typename T> T Cub(T val) {
		return cub(val);
	}


	template <typename T> int32_t Round(T r) {
		return (int32_t)((r > 0.0) ? (r + 0.5) : (r - 0.5)); 
	}

	// Very fast Log2.
	// It has a very large chance of failure on non-powers of two. Don't fuck up.
	static inline uint32_t log2 ( const uint32_t x )
	{
#ifndef _MSC_VER
		return __builtin_ctz (x); // should work on ARM
#else
#	if _WIN64
		DWORD result;
		_BitScanReverse( &result, DWORD(x) );
		return uint32_t(result);
#	else
		uint32_t y;
		__asm {
			bsr eax, x
			mov y, eax
		};
		return y;
#	endif
#endif
	}

	// Pretty fast integer Exp2.
	// No way to screw this up
	static inline uint32_t exp2 ( const uint32_t x )
	{
		return (1<<x);
	}

};

extern CMath Math;


#endif