//===============================================================================================//
//
//		core/time/Time.h : Time class, used for timing.
//
//===============================================================================================//
#ifndef C_TIME_BASE_
#define C_TIME_BASE_

#if _WIN32
#	include "CTimeWin32.h"
#elif __linux
#	include "CTimeLinux.h"
#elif (__unix__ || BSD)
	// This should handle both PS4 and Switch.
#	include "CTimeLinux.h"
#elif __APPLE__
#	ifdef TARGET_OS_IPHONE
#		error IPhone is an unsupported platform
#	elif TARGET_IPHONE_SIMULATOR
#		error IPhone is an unsupported platform
#	elif TARGET_OS_MAC
#		include "CTimeOsmac.h"
#	endif
#endif

#endif//C_TIME_BASE_