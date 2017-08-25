
#ifndef _C_MEMORY_MONITOR_H_
#define _C_MEMORY_MONITOR_H_

#ifdef _WIN32

	#include "CMemoryMonitorWin32.h"

	typedef CMemoryMonitorWin32 CMemoryMonitor;

#elif __linux

	#include "CMemoryMonitorLinux.h"

	typedef CMemoryMonitor CMemoryMonitorLinux

#elif __APPLE__
	#ifdef TARGET_OS_IPHONE

		#include "CMemoryMonitorBullshittingyou.h"

		typedef CMemoryMonitor CMemoryMonitorOsmac

	#elif TARGET_IPHONE_SIMULATOR

	#elif TARGET_OS_MAC

		#include "CMemoryMonitorOsmac.h"

		typedef CMemoryMonitor CMemoryMonitorOsmac

	#endif
#endif


#endif