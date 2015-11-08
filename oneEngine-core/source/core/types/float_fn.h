
#ifndef FLOAT_TYPE_FN_H
#define FLOAT_TYPE_FN_H

// Utility functions
FORCE_INLINE int fltcmp ( ftype _flt_a, ftype _flt_b ) {
	ftype result = _flt_a-_flt_b;
	if ( result < -FTYPE_PRECISION ) return -1;
	else if ( result > FTYPE_PRECISION ) return 1;
	else return 0;
}


#endif//FLOAT_TYPE_FN_H