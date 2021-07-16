#include "Profiles.h"
#include "core/os.h"

#if PLATFORM_WINDOWS

#ifndef SECURITY_WIN32
#define SECURITY_WIN32
#endif

// TODO: this better
#pragma comment(lib, "Secur32.lib")

#include <security.h>

std::string core::os::profiles::GetCurrentProfileName ( void )
{
	std::string result;

	// Get length of the username
	ULONG textLength = 0;
	GetUserNameEx(NameSamCompatible, NULL, &textLength);

	// Get the actual username
	char* textBuffer = new char[textLength];
	GetUserNameEx(NameSamCompatible, textBuffer, &textLength);
	result = textBuffer;
	delete textBuffer;

	return result;
}

#endif