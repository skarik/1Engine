
#include "linkerfix_legacy.h"

struct FILE*  __iob_func(void) {
	FILE _iob[] = { *stdin, *stdout, *stderr };
	return _iob;
}