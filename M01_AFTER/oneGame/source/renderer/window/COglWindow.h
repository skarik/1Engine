
#ifndef _COGL_WINDOW_BASE_
#define _COGL_WINDOW_BASE_

#ifdef _WIN32

	#include "COglWindowWin32.h"

	typedef COglWindowWin32 COglWindow;

#elif __linux

	#include "COglWindowLinux.h"

	typedef COglWindow COglWindowWinLinux

#elif __APPLE__
	#ifdef TARGET_OS_IPHONE

		#include "COglWindowBullshittingyou.h"

		typedef COglWindow COglWindowOsmac

	#elif TARGET_IPHONE_SIMULATOR

	#elif TARGET_OS_MAC

		#include "COglWindowOsmac.h"

		typedef COglWindow COglWindowOsmac

	#endif
#endif

#endif