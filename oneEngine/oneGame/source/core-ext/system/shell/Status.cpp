#include "Status.h"
#include "core/os.h"

#if _WIN32 && !__c2__

// TODO: This shit leaks on deinit. Fix that shit.

#include <shobjidl.h>
#include <windows.h>
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Ole32.lib")

class Win7TaskbarProgress  
{
public:
	Win7TaskbarProgress();
	virtual ~Win7TaskbarProgress();

	void SetProgressState(HWND hwnd, TBPFLAG flag);
	void SetProgressValue(HWND hwnd, ULONGLONG ullCompleted, ULONGLONG ullTotal);

private:
	bool Init();
	ITaskbarList3* m_pITaskBarList3;
	bool m_bFailed;
};

Win7TaskbarProgress::Win7TaskbarProgress()
{
	m_pITaskBarList3 = NULL;
	m_bFailed = false;
}

Win7TaskbarProgress::~Win7TaskbarProgress()
{
	if (m_pITaskBarList3)   
	{
		m_pITaskBarList3->Release();
		CoUninitialize();
	}
}

void Win7TaskbarProgress::SetProgressState( HWND hwnd, TBPFLAG flag )
{
	if (Init())
		m_pITaskBarList3->SetProgressState(hwnd, flag);
}

void Win7TaskbarProgress::SetProgressValue( HWND hwnd, ULONGLONG ullCompleted, ULONGLONG ullTotal )
{
	if (Init())
		m_pITaskBarList3->SetProgressValue(hwnd, ullCompleted, ullTotal);
}

bool Win7TaskbarProgress::Init()
{
	if (m_pITaskBarList3)
		return true;

	if (m_bFailed)
		return false;

	// Initialize COM for this thread...
	CoInitialize(NULL);

	CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_ITaskbarList3, (void **)&m_pITaskBarList3);

	if (m_pITaskBarList3)
		return true;

	m_bFailed = true;
	CoUninitialize();
	return false;
}


static Win7TaskbarProgress* progress = NULL;
static intptr_t progress_default_handle = 0;

//		SetTaskbarProgressHandle ( ) : changes default taskbar handle
void core::shell::SetTaskbarProgressHandle ( intptr_t shellhandle )
{
	if ( shellhandle != NIL )
	{
		progress_default_handle = shellhandle;
	}
}
//		SetTaskbarProgressState ( ) : changes current taskbar state
void core::shell::SetTaskbarProgressState ( intptr_t shellhandle, const ETaskbarState_t state )
{
	if ( progress == NULL ) progress = new Win7TaskbarProgress;

	HWND handle = (HWND)progress_default_handle;
	if ( shellhandle != NIL )
		handle = (HWND)shellhandle;

	switch (state)
	{
	case TBP_NO_PROGRESS:
		progress->SetProgressState(handle, TBPF_NOPROGRESS);
		break;
	case TBP_INDETERMINATE:
		progress->SetProgressState(handle, TBPF_INDETERMINATE);
		break;
	case TBP_NORMAL:
		progress->SetProgressState(handle, TBPF_NORMAL);
		break;
	case TBP_ERROR:
		progress->SetProgressState(handle, TBPF_ERROR);
		break;
	case TBP_PAUSED:
		progress->SetProgressState(handle, TBPF_PAUSED);
		break;
	}
}
//		SetTaskbarProgressValue ( ) : changes current taskbar completion value
void core::shell::SetTaskbarProgressValue ( intptr_t shellhandle, const uint64_t& ullcurrent, const uint64_t& ulltotal )
{
	if ( progress == NULL ) progress = new Win7TaskbarProgress;

	HWND handle = (HWND)progress_default_handle;
	if ( shellhandle != NIL )
		handle = (HWND)shellhandle;

	progress->SetProgressValue(handle, ullcurrent, ulltotal);
}

//		FlashTray ( ) : flashes the tray icon a given amount of times
void core::shell::FlashTray ( intptr_t shellhandle, const uint32_t flashcount )
{
	HWND handle = (HWND)progress_default_handle;
	if ( shellhandle != NIL )
		handle = (HWND)shellhandle;

	FLASHWINFO flashinfo = {0};
	flashinfo.cbSize = sizeof(FLASHWINFO);
	flashinfo.hwnd = handle;
	flashinfo.dwFlags = FLASHW_TRAY | FLASHW_CAPTION;
	flashinfo.uCount = flashcount;
	flashinfo.dwTimeout = 200;

	FlashWindowEx(&flashinfo);
}

#else//_WIN32

//		SetTaskbarProgressHandle ( ) : changes default taskbar handle
void core::shell::SetTaskbarProgressHandle ( intptr_t shellhandle )
{
}
//		SetTaskbarProgressState ( ) : changes current taskbar state
void core::shell::SetTaskbarProgressState ( intptr_t shellhandle, const ETaskbarState_t state )
{
}
//		SetTaskbarProgressValue ( ) : changes current taskbar completion value
void core::shell::SetTaskbarProgressValue ( intptr_t shellhandle, const uint64_t& ullcurrent, const uint64_t& ulltotal )
{
}

//		FlashTray ( ) : flashes the tray icon a given amount of times
void core::shell::FlashTray ( intptr_t shellhandle, const uint32_t flashcount )
{
}

#endif//_WIN32