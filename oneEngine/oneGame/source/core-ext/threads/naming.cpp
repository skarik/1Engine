#include "naming.h"
#include "core/os.h"

#if PLATFORM_WINDOWS

#include <processthreadsapi.h>

typedef HRESULT ( __cdecl *SetThreadDescriptionPROC )( HANDLE, PCWSTR );

static bool		g_loadedProc = false;
static SetThreadDescriptionPROC
				g_SetThreadDescription = nullptr;

const DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push, 8)
typedef struct tagTHREADNAME_INFO
{
	DWORD		dwType;		// Must be 0x1000.
	LPCSTR		szName;		// Pointer to name (in user addr space).
	DWORD		dwThreadID;	// Thread ID (-1=caller thread).
	DWORD		dwFlags;	// Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

static void SetThreadName ( HANDLE threadHandle, DWORD threadID, const char* thread_name )
{
	// TODO: Ifdef the following two blocks via WindowsSDK version. SetThreadDescription should exist in newer SDKs (probably).

	// Load up SetThreadDescription and see if it is available
	if (!g_loadedProc)
	{
		HINSTANCE hinstLib;
		hinstLib = LoadLibrary(TEXT("KernelBase.lib")); 
		if (hinstLib != NULL)
		{
			g_SetThreadDescription = (SetThreadDescriptionPROC) GetProcAddress(hinstLib, "SetThreadDescription"); 
		}

		g_loadedProc = true;
	}
	
	// Set via new SetThreadDescription()
	if (g_SetThreadDescription != nullptr)
	{
		int threadNameLen = (int)strlen(thread_name);
		int wideLen = MultiByteToWideChar(CP_UTF8, 0, thread_name, threadNameLen, NULL, NIL);
		
		wchar_t* wide = new wchar_t[wideLen + 1];
		MultiByteToWideChar(CP_UTF8, 0, thread_name, threadNameLen, wide, wideLen);
		wide[wideLen] = 0;

		g_SetThreadDescription(threadHandle, wide);

		delete[] wide;
	}

	// Set via exception (fallback, but can be used concurrently with SetThreadDescription())
	{
		THREADNAME_INFO info;
		info.dwType = 0x1000;
		info.szName = thread_name;
		info.dwThreadID = threadID;
		info.dwFlags = 0;

		__try
		{
			RaiseException( MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info );
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}
}

void core::threads::SetThreadName ( std::thread& thread, const char* thread_name )
{
	HANDLE threadHandle = (HANDLE)thread.native_handle();
	DWORD threadID = GetThreadId( threadHandle );
	::SetThreadName( threadHandle, threadID, thread_name );
}

void core::threads::SetThisThreadName ( const char* thread_name )
{
	HANDLE threadHandle = GetCurrentThread();
	DWORD threadID = GetCurrentThreadId();
	::SetThreadName( threadHandle, threadID, thread_name );
}

#endif