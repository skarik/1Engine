// linkerfix_legacy.h
// This header is for allowing legacy VC++ 2012 and below to run on above.

#ifndef _LINKER_FIX_LEGACY_H_
#define _LINKER_FIX_LEGACY_H_

#include <cstdio>

struct FILE* __cdecl __iob_func(void);

#endif // _LINKER_FIX_LEGACY_H_