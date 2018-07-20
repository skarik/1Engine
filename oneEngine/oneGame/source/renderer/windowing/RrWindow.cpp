#include "core/debug/console.h"
#include "core/settings/CGameSettings.h"
#include "renderer/windowing/RrWindow.h"

#include <shellapi.h>

//#define ERROR_OUT(_str, ...) { printf(_str, __VA_ARGS__); abort(); }
#define ERROR_OUT(_str, ...) { printf(_str, __VA_ARGS__); return; }

#ifdef _WIN32

// Message loop
LRESULT CALLBACK MessageUpdate(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//===============================================================================================//
// Local Utilties:
//===============================================================================================//

FORCE_INLINE
static int chooseNonzero ( const int variable, const int fallback )
{
	if (variable != 0)
		return variable;
	return fallback;
}

//===============================================================================================//
// WINDOW CLASS:
//===============================================================================================//

// Static variables
std::vector<RrWindow*> RrWindow::m_windows;

RrWindow::RrWindow(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
	: mw_instance(hInstance), mw_previnstance(hPrevInstance), mw_cmdline(lpCmdLine), mw_cmdshow(nCmdShow),
	mw_window(0),
	done(false), active(true), focused(true)
{
	auto gsi = CGameSettings::Active();

	m_resolution.x = chooseNonzero(gsi->i_ro_TargetResX, 1280);
	m_resolution.y = chooseNonzero(gsi->i_ro_TargetResY, 720);
	m_colordepth   = gsi->b_ro_UseHighRange ? 10 : 8;
	m_fullscreen = false;

	CreateScreen();
	RegisterInput();
	CreateConsole();


}
RrWindow::~RrWindow ( void )
{
}


void RrWindow::CreateConsole ( void )
{
	debug::CDebugConsole::Init();
}

bool RrWindow::Show ( void )
{
	if (mw_window == NIL)
	{
		return false;
	}

	// Show the window now
	ShowWindow(mw_window, SW_SHOW);
	SetForegroundWindow(mw_window);
	SetFocus(mw_window);

	return true;
}

bool RrWindow::Close ( void )
{

}

//===============================================================================================//
// RRWINDOW INIT:
//===============================================================================================//

//	CreateScreen
// Creates OS specific rendering surface
void RrWindow::CreateScreen ( void )
{
	uint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left		= 0;			
	WindowRect.right	= m_resolution.x;
	WindowRect.top		= 0;
	WindowRect.bottom	= m_resolution.y;

	//mw_instance			= GetModuleHandle(NULL);			// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) MessageUpdate;				// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= mw_instance;							// Set The Instance
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= "GraphicsWindow";						// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		mw_window = NIL;
		ERROR_OUT("Failed To Register The Window Class. (Has the class already been registered?)\n");
	}

	if (m_fullscreen)
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth	= m_resolution.x;		// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= m_resolution.y;		// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= 32;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			mw_window = NIL;
			ERROR_OUT("The Requested Fullscreen Mode Is Not Supported By\nYour Video Card.");
		}

		// Set window style
		dwExStyle = WS_EX_APPWINDOW;
		dwStyle = WS_POPUP;
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle = WS_OVERLAPPEDWINDOW;
	}

	// Adjust Window To True Requested Size
	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);

	// Create The Window
	mw_window = CreateWindowEx( dwExStyle,	// Extended Style For The Window
		"GraphicsWindow",					// Class Name
		"OpenGL Window",					// Window Title
		dwStyle |							// Defined Window Style
		WS_CLIPSIBLINGS |					// Required Window Style
		WS_CLIPCHILDREN,					// Required Window Style
		64, 64,								// Window Position
		WindowRect.right-WindowRect.left,	// Calculate Window Width
		WindowRect.bottom-WindowRect.top,	// Calculate Window Height
		NULL,								// No Parent Window
		NULL,								// No Menu
		mw_instance,						// Instance
		NULL								// Dont Pass Anything To WM_CREATE
	);
	if ( mw_window == NIL )
	{
		ERROR_OUT("Window Creation Error.");
	}

	// If not fullscreen, start the window in the center of the screen (or where it was last set to)
	if ( !m_fullscreen )
	{
		// Center window
		RECT rc;
		int screenWidth = GetSystemMetrics(SM_CXSCREEN);
		int screenHeight = GetSystemMetrics(SM_CYSCREEN);
		GetWindowRect( mw_window, &rc );
		int window_pos_x = (screenWidth - rc.right)/2;
		int window_pos_y = (screenHeight - rc.bottom)/2;

		// Grab saved window position
		if ( CGameSettings::Active()->GetSettingExists("cl_windowposx") )
			window_pos_x = CGameSettings::Active()->GetSettingAsInt("cl_windowposx");
		if ( CGameSettings::Active()->GetSettingExists("cl_windowposy") )
			window_pos_y = CGameSettings::Active()->GetSettingAsInt("cl_windowposy");

		// Set window position
		SetWindowPos( mw_window, 0,
			window_pos_x,
			window_pos_y,
			0, 0, SWP_NOZORDER|SWP_NOSIZE );
	}
}

void RrWindow::RegisterInput ( void )
{
	if (mw_window == NIL)
	{
		ERROR_OUT("Window is not created. Cannot register input.");
	}

	// init raw mouse input
	RAWINPUTDEVICE Rid[2];

	Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC; 
	Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE; 
	Rid[0].dwFlags = RIDEV_INPUTSINK;   
	Rid[0].hwndTarget = mw_window;

	Rid[1].usUsagePage = HID_USAGE_PAGE_GENERIC;
	Rid[1].usUsage = 0x06;
	Rid[1].dwFlags = 0;//RIDEV_NOLEGACY;
	Rid[1].hwndTarget = mw_window;

	if ( !RegisterRawInputDevices(Rid, 2, sizeof(RAWINPUTDEVICE) ) )
	{
		printf( "BAD HID REGISTRATION.\n" );
		throw std::exception("BAD HID REGISTRATION");
	}

	// init touch input
	RegisterTouchWindow( mw_window, 0 );
	// set drag and drop
	DragAcceptFiles( mw_window, TRUE );
}

#endif//_WIN32