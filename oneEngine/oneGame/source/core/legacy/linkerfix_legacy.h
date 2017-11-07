//===============================================================================================//
//
//		linkerfix_legacy.h
//
// This header is for allowing legacy VC++ 2012 libraries and below to run on VC++ 2013 and up.
//
//===============================================================================================//
#ifndef LINKER_FIX_LEGACY_H_
#define LINKER_FIX_LEGACY_H_

// Fix for compiling 1700 and below libraries with 1800 and up
#if _MSC_VER > 1700
#include <cstdio>

extern "C" FILE _iob[] = { *stdin, *stdout, *stderr };
extern "C" FILE*  __iob_func(void)
{
	return _iob;
}

#endif

#endif // LINKER_FIX_LEGACY_H_