//===============================================================================================//
//
//		float_fn.h
//
// Contains quick one-off floating point utilities that don't belong anywhere else.
//
//===============================================================================================//
#ifndef CORE_FLOAT_TYPE_FN_H_
#define CORE_FLOAT_TYPE_FN_H_

#include "float.h"

// Utility functions
FORCE_INLINE int fltcmp ( Real _flt_a, Real _flt_b )
{
	Real result = _flt_a-_flt_b;
	if ( result < -FTYPE_PRECISION ) return -1;
	else if ( result > FTYPE_PRECISION ) return 1;
	else return 0;
}


#endif//CORE_FLOAT_TYPE_FN_H_