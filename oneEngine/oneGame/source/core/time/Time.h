

#ifndef _C_TIME_BASE_
#define _C_TIME_BASE_

#ifdef _WIN32

	#include "CTimeWin32.h"

#elif __linux

	#include "CTimeLinux.h"

#elif __APPLE__
	#ifdef TARGET_OS_IPHONE

		#include "COglWindowBullshittingyou.h"

	#elif TARGET_IPHONE_SIMULATOR

	#elif TARGET_OS_MAC

		#include "CTimeOsmac.h"

	#endif
#endif

#endif