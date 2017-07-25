#include <stdio.h>
#include <stdarg.h>
#include "errors.h"

namespace cone
{
	int errorReport(int code, const char *format, ...)
	{
		va_list args;
		va_start(args, format);

		printf("RT%04d: ", code);
		vprintf(format, args);

		va_end(args);

		return code;
	}
}