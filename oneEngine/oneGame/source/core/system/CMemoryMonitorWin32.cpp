// Memory Monitor class, Windows implementation
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

#include "CMemoryMonitorWin32.h"

HANDLE CMemoryMonitorWin32::process_handle = NULL;

// Constructor
CMemoryMonitorWin32::CMemoryMonitorWin32 ( void )
{
	process_handle = GetCurrentProcess();

	iMaxStored = 1024*4;
	p_setSize = new uint64_t [iMaxStored];
	for ( unsigned int i = 0; i < iMaxStored; i++ )
	{
		p_setSize[i] = 0;
	}
}	

// Destructor
CMemoryMonitorWin32::~CMemoryMonitorWin32 ( void )
{
	CloseHandle( process_handle );
	delete [] p_setSize;
}	


// Update
void CMemoryMonitorWin32::Update ( void )
{
	GetProcessMemoryInfo( process_handle,&pmc, sizeof(pmc) );

	for ( unsigned int i = iMaxStored-1; i > 0; i-- )
	{
		p_setSize[i] = p_setSize[i-1];
	}
	p_setSize[0] = pmc.WorkingSetSize;
	
}

// == Getters ==
uint64_t	CMemoryMonitorWin32::GetPeakUsage ( void )
{
	uint64_t max = p_setSize[0];
	for ( uint64_t i = 1; i < iMaxStored; i++ )
	{
		if ( p_setSize[i] > max )
			max = p_setSize[i];
	}
	return max;
}
uint64_t*	CMemoryMonitorWin32::GetUsage ( void )
{
	return p_setSize;
}
unsigned int	CMemoryMonitorWin32::GetUsageSize ( void )
{
	return iMaxStored;
}