
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
	#define PI 3.14159265358979323846264338327950288419716939937510582097494459230781640628620
	#define PHI 1.6180339887498948482045868343656381177203091798057628

	#define degtorad(f) ( ((f) * 3.14159265358979323846264338327950288419716939937510582097494459230781640628620)/180.0 )
	#define radtodeg(f) ( ((f) / 3.14159265358979323846264338327950288419716939937510582097494459230781640628620)*180.0 )

	#define sqr(f) ( ((f)*(f)) )
	#define cub(f) ( ((f)*(f)*(f)) )

	#define to_meters(f) ( (f)*(0.3048) ) )
	#define to_feet(f) ( (f)*(3.2808399) ) )

	#define random_range(a,b) ( ((a) + ((rand()/((ftype)RAND_MAX))*((b)-(a)))) )

	#ifdef ENGINE_PRECISION_DOUBLE
		#define FTYPE_PRECISION 1e-8
	#else
		#define FTYPE_PRECISION 1e-6
	#endif
#endif

#endif