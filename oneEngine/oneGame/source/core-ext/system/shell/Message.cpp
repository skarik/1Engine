#include "core/os.h"
#include <varargs.h>
#include <stdio.h>

namespace core {
namespace shell
{
	//	ShowErrorMessage(string) : Shows an error message, creating a message window with the OS.
	void ShowErrorMessage ( const char* fmt, ... )
	{
		const int kBufferSize = 1024 * 16;
		char buffer[kBufferSize];

		va_list argptr;
		va_start(argptr, fmt);
		vsnprintf(buffer, kBufferSize, fmt, argptr);
		va_end(argptr);

		MessageBox(NULL, buffer, "Error", MB_OK | MB_ICONEXCLAMATION);
	}
}}