//===============================================================================================//
//
//		float.h
//
// Floating point definitions for the engine.
// Real is the type used by the engine for most floating point maths, and defaults to 32-bit float.
// 
//
// The renderer is especially reliant on being only 32-bit float, with all 64-bit floating point
//	maths being "emulated" via offsets and local coordinates.
//
//===============================================================================================//
#ifndef FLOAT_TYPE_H_
#define FLOAT_TYPE_H_

//#define ENGINE_PRECISION_DOUBLE
#undef ENGINE_PRECISION_DOUBLE // Override all compiler settings for now - 32-bit floats are faster.

// Extra typedef for the people used to other engines
typedef float	Real_f;
typedef double	Real_d;
// Explicit bit count floats
typedef float	Real32;
typedef double	Real64;

// Check precision options
#ifdef ENGINE_PRECISION_DOUBLE
	typedef Real64	Real;
	typedef Real64	val_t;
#else
	typedef Real32	Real;
	typedef Real32	val_t;
#endif


#ifndef ENGINE_FTYPE_DEFINES
	
	// Reminder of constants in <cmath>:
#	define M_E        2.71828182845904523536   // e
#	define M_LOG2E    1.44269504088896340736   // log2(e)
#	define M_LOG10E   0.434294481903251827651  // log10(e)
#	define M_LN2      0.693147180559945309417  // ln(2)
#	define M_LN10     2.30258509299404568402   // ln(10)
#	define M_PI       3.14159265358979323846   // pi
#	define M_PI_2     1.57079632679489661923   // pi/2
#	define M_PI_4     0.785398163397448309616  // pi/4
#	define M_1_PI     0.318309886183790671538  // 1/pi
#	define M_2_PI     0.636619772367581343076  // 2/pi
#	define M_2_SQRTPI 1.12837916709551257390   // 2/sqrt(pi)
#	define M_SQRT2    1.41421356237309504880   // sqrt(2)
#	define M_SQRT1_2  0.707106781186547524401  // 1/sqrt(2)

	// Common constants ripped from somewhere:
#	define PI		3.14159265358979323846	// pi
#	define PI_2		1.57079632679489661923	// pi/2
#	define PI_4		.785398163397448309616	// pi/4
#	define PHI		1.61803398874989484820	// phi (golden ratio)

	// Common quick mathematical operations:

#	define degtorad(f) ( ((f) * 3.14159265358979323846264338327950288419716939937510582097494459230781640628620)/180.0 )
#	define radtodeg(f) ( ((f) / 3.14159265358979323846264338327950288419716939937510582097494459230781640628620)*180.0 )
#	define sqr(f) ( ((f)*(f)) )
#	define cub(f) ( ((f)*(f)*(f)) )

	// Unit conversions used for AFTER's display.
#	define to_meters(f) ( (f)*(0.3048) ) )
#	define to_feet(f) ( (f)*(3.2808399) ) )

	// Precision constant used for floating point compares:
#	ifdef ENGINE_PRECISION_DOUBLE
#		define FTYPE_PRECISION 1e-8
#		define FLOAT_PRECISION 1e-8
#	else
#		define FTYPE_PRECISION 1e-6F
#		define FLOAT_PRECISION 1e-6F
#	endif
#endif

#endif//FLOAT_TYPE_H_