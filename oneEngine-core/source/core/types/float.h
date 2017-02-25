
#ifndef FLOAT_TYPE_H
#define FLOAT_TYPE_H

//#define ENGINE_PRECISION_DOUBLE
#undef ENGINE_PRECISION_DOUBLE

// Toggle the behavior of things based on the precision option
#ifdef ENGINE_PRECISION_DOUBLE
	typedef double	ftype;
	typedef double	val_t;
#else
	typedef float	ftype;
	typedef float	val_t;
#endif
// Extra typedef for the people used to other engines
typedef ftype	Real;
typedef ftype	Real_f;
typedef double	Real_d;
// Explicit bit count floats
typedef float	Real_32;
typedef double	Real_64;

#ifndef ENGINE_FTYPE_DEFINES
	/*#define PI 3.14159265358979323846264338327950288419716939937510582097494459230781640628620
	#define PHI 1.6180339887498948482045868343656381177203091798057628

	#define M_E        2.71828182845904523536   // e
	#define M_LOG2E    1.44269504088896340736   // log2(e)
	#define M_LOG10E   0.434294481903251827651  // log10(e)
	#define M_LN2      0.693147180559945309417  // ln(2)
	#define M_LN10     2.30258509299404568402   // ln(10)
	#define M_PI       3.14159265358979323846   // pi
	#define M_PI_2     1.57079632679489661923   // pi/2
	#define M_PI_4     0.785398163397448309616  // pi/4
	#define M_1_PI     0.318309886183790671538  // 1/pi
	#define M_2_PI     0.636619772367581343076  // 2/pi
	#define M_2_SQRTPI 1.12837916709551257390   // 2/sqrt(pi)
	#define M_SQRT2    1.41421356237309504880   // sqrt(2)
	#define M_SQRT1_2  0.707106781186547524401  // 1/sqrt(2)*/

#	define PI		3.14159265358979323846	// pi
#	define PI_2		1.57079632679489661923	// pi/2
#	define PI_4		.785398163397448309616	// pi/4
#	define PHI		1.61803398874989484820	// phi (golden ratio)

#	define degtorad(f) ( ((f) * 3.14159265358979323846264338327950288419716939937510582097494459230781640628620)/180.0 )
#	define radtodeg(f) ( ((f) / 3.14159265358979323846264338327950288419716939937510582097494459230781640628620)*180.0 )

#	define sqr(f) ( ((f)*(f)) )
#	define cub(f) ( ((f)*(f)*(f)) )

#	define to_meters(f) ( (f)*(0.3048) ) )
#	define to_feet(f) ( (f)*(3.2808399) ) )

#	ifdef ENGINE_PRECISION_DOUBLE
#		define FTYPE_PRECISION 1e-8
#	else
#		define FTYPE_PRECISION 1e-6
#	endif
#endif

#endif