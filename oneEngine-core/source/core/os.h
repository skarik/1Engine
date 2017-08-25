#ifndef CORE_OS_
#define CORE_OS_

// OS NAME:
#if defined(_WIN64)
#	define __OS_STRING_NAME__ "Win32 x64"
#elif defined(_WIN32)
#	define __OS_STRING_NAME__ "Win32 x86"
#else
#	define __OS_STRING_NAME__ "Unknown"
#endif

// OS SPECIFIC INCLUDES:

#ifdef _WIN32

	// Input defines
	#ifndef HID_USAGE_PAGE_GENERIC
	#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
	#endif
	#ifndef HID_USAGE_GENERIC_MOUSE
	#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
	#endif

	// No MFC
	#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN 1
	#endif

	// Disable certain security warnings
#	ifndef _CRT_SECURE_NO_WARNINGS
#	define _CRT_SECURE_NO_WARNINGS
#	endif
#	ifndef _CRT_SECURE_NO_DEPRECATE
#	define _CRT_SECURE_NO_DEPRECATE
#	endif
#	ifndef _SCL_SECURE_NO_DEPRECATE
#	define _SCL_SECURE_NO_DEPRECATE
#	endif

	#include <windows.h>		// Header File For Windows

	#ifdef min
	#undef min
	#endif
	#ifdef max
	#undef max
	#endif

#endif

#endif//CORE_OS_