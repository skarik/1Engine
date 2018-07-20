#ifndef RR_WINDOW_BASE_
#define RR_WINDOW_BASE_

#ifdef _WIN32

	#include "rrWindowWin32.h"

#elif __linux

	#include "COglWindowLinux.h"

	typedef RrWindow COglWindowWinLinux

#elif __APPLE__
	#ifdef TARGET_OS_IPHONE

		#include "COglWindowBullshittingyou.h"

		typedef RrWindow COglWindowOsmac

	#elif TARGET_IPHONE_SIMULATOR

	#elif TARGET_OS_MAC

		#include "COglWindowOsmac.h"

		typedef RrWindow COglWindowOsmac

	#endif
#endif

#endif//RR_WINDOW_BASE_