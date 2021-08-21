#include "Inputs.h"
#include "core/os.h"

#if PLATFORM_WINDOWS

const float core::shell::GetDoubleClickInterval ( void )
{
	return GetDoubleClickTime() / 1000.0F;
}

void core::shell::SetSystemMousePosition ( const int x, const int y )
{
	RECT rc;
	HWND hWnd = GetActiveWindow();
	GetClientRect(hWnd, &rc);
	ClientToScreen(hWnd, (LPPOINT)&rc);

	SetCursorPos(x + rc.left, y + rc.top);
}

#endif