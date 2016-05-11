// linkerfix_legacy.h
// This header is for allowing legacy VC++ 2012 and below to run on above.

#ifndef _LINKER_FIX_LEGACY_H_
#define _LINKER_FIX_LEGACY_H_

// Fix for compiling 1700 and below libraries with 1800 and up
#if _MSC_VER > 1700
#include <cstdio>

extern "C" FILE _iob[] = { *stdin, *stdout, *stderr };
extern "C" FILE*  __iob_func(void)
{
	return _iob;
}

#endif

#endif // _LINKER_FIX_LEGACY_H_