// Memory Monitor class, Windows definition
//*********************************************************************
//	Parts of this implementation were created by the Borenlabs Corporation
//	They are used with permission from the Borenlabs Corporation.
//*********************************************************************
//               BorenLabs Corporation
//               Irvine, California
//
//        Respective code is Copyright (c) BorenLabs Corporation.
//			It is proprietary and is not to be copied in source,
//               object or any other format without the express
//               written permission of BorenLabs Corporation.
//*********************************************************************


#ifndef _C_MEMORY_MONITOR_WIN32_
#define _C_MEMORY_MONITOR_WIN32_

// Use minimal windows amount
#ifndef WIN32_LEAN_AND_MEAN 
#define WIN32_LEAN_AND_MEAN 1
#endif

// Includes
#include <windows.h>
#include <psapi.h>
#pragma comment(lib, "psapi.lib")

// Class Definition
class CMemoryMonitorWin32
{
public:
	CMemoryMonitorWin32 ( void );
	~CMemoryMonitorWin32 ( void );
	
	void Update ( void );
	unsigned long	GetPeakUsage ( void );
	unsigned long*	GetUsage ( void );
	unsigned int	GetUsageSize ( void );

private:
	PROCESS_HEAP_ENTRY            process_heap_entry;
	MEMORYSTATUS                  mem_status;
	PROCESS_MEMORY_COUNTERS       pmc;
	static HANDLE                 process_handle;
	HINSTANCE                     hlib_process;
	unsigned long                 max_bytes;
	unsigned long                 min_bytes;
	//GETPROCESSMEMORYINFO          GetProcessMemoryInfo;
protected:
	unsigned int	iMaxStored;
	unsigned long *	p_setSize;
};

#endif