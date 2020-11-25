#include "core/debug/console.h"
#include "core/input/CInput.h"
#include "core/system/Screen.h"
#include "core/settings/CGameSettings.h"
#include "core-ext/system/shell/Message.h"
#include "core-ext/system/shell/DragAndDrop.h"
#include "renderer/state/RrRenderer.h"
#include "renderer/windowing/RrWindow.h"

#include <shellapi.h>
#include <algorithm>

//#define ERROR_OUT(_str, ...) { printf(_str, __VA_ARGS__); abort(); }
#define ERROR_OUT(_str, ...) { printf(_str, __VA_ARGS__); return; }

#ifdef _WIN32

// Message loop
LRESULT CALLBACK MessageUpdate(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Constants
static const char* const	kWindowClass = "GraphicsWindow";

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
	done(false), active(true), focused(true), hiddencursor(false)
{
	auto gsi = CGameSettings::Active();

	m_resolution.x = chooseNonzero(gsi->i_ro_TargetResX, 1280);
	m_resolution.y = chooseNonzero(gsi->i_ro_TargetResY, 720);
	m_colordepth   = gsi->b_ro_UseHighRange ? 10 : 8;
	m_outputFormat = gsi->b_ro_UseHighRange ? gpu::kOutputFormatRGB10 : gpu::kOutputFormatRGB8;
	m_fullscreen = false;

	// Load window options 
	CGameSettings::Active()->LoadSettings();

	// Register class
	WNDCLASS	wc; // Windows Class Structure
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) MessageUpdate;				// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= mw_instance;							// Set The Instance
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= kWindowClass;							// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		mw_window = NIL;
		ERROR_OUT("Failed To Register The Window Class. (Has the class already been registered?)\n");
	}

	// Add self to the window list:
	m_windows.push_back(this);

	// Create everything!
	CreateScreen();
	RegisterInput();
	CreateConsole();

	CreateGfxInstance();
	CreateGfxSurface();
}
RrWindow::~RrWindow ( void )
{
	DestroyGfxSurface();
	DestroyGfxInstance();
	DestroyScreen();

	// Remove this from the list of windows
	auto this_window = std::find(m_windows.begin(), m_windows.end(), this);
	if (this_window != m_windows.end())
		m_windows.erase(this_window);
}


void RrWindow::CreateConsole ( void )
{
	debug::ConsoleWindow::Init();
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
	// Unregister class
	if (UnregisterClass(kWindowClass, mw_instance) != 0)
	{
		core::shell::ShowErrorMessage("Could not unregister class.");
		return false;
	}
	return true;
}

//===============================================================================================//
// RRWINDOW INIT:
//===============================================================================================//

//	CreateScreen
// Creates OS specific rendering surface
void RrWindow::CreateScreen ( void )
{
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left		= 0;			
	WindowRect.right	= m_resolution.x;
	WindowRect.top		= 0;
	WindowRect.bottom	= m_resolution.y;

	//mw_instance			= GetModuleHandle(NULL);			// Grab An Instance For Our Window
	if (m_fullscreen)
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth	= m_resolution.x;		// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= m_resolution.y;		// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= 32;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields = DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			mw_window = NIL;
			ERROR_OUT("The requested fullscreen mode is not supported by your video card.\n");
		}

		// Set window style
		dwExStyle = WS_EX_APPWINDOW;
		dwStyle = WS_POPUP;
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MAXIMIZEBOX;
	}

	// Adjust Window To True Requested Size
	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);

	// Create The Window
	mw_window = CreateWindowEx( dwExStyle,	// Extended Style For The Window
		kWindowClass,						// Class Name
		"1Engine Base Window",				// Window Title
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
		ERROR_OUT("Window Creation Error.\n");
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
		ERROR_OUT("Window is not created. Cannot register input.\n");
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

// Creates the instance & device
void RrWindow::CreateGfxInstance ( void )
{
	if (mw_window == NIL)
	{
		ERROR_OUT("Window is not created. Cannot create device.\n");
	}

	m_device = new gpu::Device((intptr_t)mw_instance, (intptr_t)mw_window);

#ifdef _ENGINE_DEBUG
	uint32_t layerCount = 1;
	gpu::DeviceLayer layers [] = {gpu::kDeviceLayerDebug};
#else
	uint32_t layerCount = 0;
	gpu::DeviceLayer* layers = NULL;
#endif

	if (m_device->create(layers, layerCount) != 0)
	{
		delete m_device;
		DestroyScreen();
		ERROR_OUT("Gfx instance creation error.\n");
	}
}

// Sets up surface to render to
void RrWindow::CreateGfxSurface ( void )
{
	if (mw_window == NIL)
	{
		ERROR_OUT("Window is not created. Cannot create surface.\n");
	}

	if (m_surface.create(m_device, gpu::kPresentModeImmediate, m_resolution.x, m_resolution.y, m_outputFormat, m_fullscreen) != 0)
	{
		DestroyScreen();
		ERROR_OUT("Gfx surface creation error.\n");
	}

	if (m_device->initialize(&m_surface) != 0)
	{
		delete m_device;
		DestroyScreen();
		core::shell::ShowErrorMessage("Your video card does not support the minimum required OpenGL version.\nOpenGL 4.5 is required. Please update your drivers.");
		ERROR_OUT("Gfx instance creation error.\n");
	}
}

void RrWindow::CreateGfxSwapchain ( void )
{}

bool RrWindow::AttachRenderer ( RrRenderer* renderer )
{
	m_renderer = renderer;
	m_renderer->InitializeWithDeviceAndSurface( this->m_device, &this->m_surface );
	return true;
}


//===============================================================================================//
// RRWINDOW CLEANUP:
//===============================================================================================//

void RrWindow::DestroyScreen ( void )
{
	// If we're not in fullscreen mode
	if (m_fullscreen)
	{	// save the window position
		RECT window_rect;
		GetWindowRect(mw_window, &window_rect);
		CGameSettings::Active()->EditSetting("cl_windowposx", window_rect.left);
		CGameSettings::Active()->EditSetting("cl_windowposy", window_rect.top);
	}

	// Check if we're in fullscreen mode
	if (m_fullscreen)
	{	// If So Switch Back To The Desktop and show the pointer
		ChangeDisplaySettings(NULL, 0);
		ShowCursor(TRUE);
	}

	// Destroy the window
	if (mw_window != NIL)
	{
		if (DestroyWindow(mw_window) == FALSE)
		{
			core::shell::ShowErrorMessage("Could Not Release hWnd.");
		}
		mw_window = NIL;
	}
}

void RrWindow::DestroyGfxInstance ( void )
{
	delete m_device;
	m_device = NULL;
}
void RrWindow::DestroyGfxSurface ( void )
{
	m_surface.destroy();
}

//===============================================================================================//
// RRWINDOW STATE HANDLING:
//===============================================================================================//

bool RrWindow::Resize ( int width, int height )
{
	if (m_resolution.x != width || m_resolution.y != height)
	{
		m_resolution.x = std::max(1, width); // Prevent zero or lower image sizes.
		m_resolution.y = std::max(1, height);

		// Refresh the device
		m_device->refresh((intptr_t)mw_instance, (intptr_t)mw_window);

		// Refresh the surface
		m_surface.destroy();
		m_surface.create(m_device, gpu::kPresentModeImmediate, m_resolution.x, m_resolution.y, m_outputFormat, m_fullscreen);

		// Resize the renderer
		if (m_renderer != NULL)
			m_renderer->ResizeSurface();
	}
	return true;
}

bool RrWindow::SetFullscreen ( bool fullscreen )
{
	if (m_fullscreen != fullscreen)
	{
		m_fullscreen = fullscreen;
		
		int width = m_resolution.x;
		int height = m_resolution.y;

		m_resolution.x = 0;
		Resize(width, height);
	}
	return true;
}

bool RrWindow::IsFullscreen ( void )
{
	return m_fullscreen;
}

bool RrWindow::SetTitle ( const char* title )
{
	if (mw_window != NIL)
	{
		SetWindowTextA(mw_window, title);
		return true;
	}
	return false;
}


bool RrWindow::UpdateMessages ( void )
{
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))	// Is There A Message Waiting?
	{	// Have We Received A Quit Message?
		if (msg.message == WM_QUIT)
		{	// then done
			done = true;							
		}
		// If Not, Deal With Window Messages
		else
		{	// Translate The Message
			TranslateMessage(&msg);			
			// Dispatch The Message
			DispatchMessage(&msg);
		}
		return false;
	}
	return active;
}

bool RrWindow::IsDone ( void )
{
	return done;
}

bool RrWindow::IsActive ( void )
{
	return active;
}

void RrWindow::PostEndMessage ( void )
{
	PostMessage(mw_window, WM_QUIT, NIL, NIL);
}

void RrWindow::PostRedrawMessage ( void )
{
	PostMessage(mw_window, WM_SETREDRAW, NIL, NIL);
}

//===============================================================================================//
// WINDOWS MESSAGE LOOP:
//===============================================================================================//

void RrWindow::UpdateMouseClipping ( void )
{
	if (   ( CInput::SysMouseX() == std::min<long>( std::max<long>( CInput::SysMouseX(), 0 ), m_resolution.x ) )
		&& ( CInput::SysMouseY() == std::min<long>( std::max<long>( CInput::SysMouseY(), 0 ), m_resolution.y ) ))
	{
		hiddencursor = false;

		// Get the window's rect
		RECT rc;
		GetClientRect( mw_window, &rc );
		POINT xy, wh;
		xy.x = rc.left;
		xy.y = rc.top;
		wh.x = rc.right;
		wh.y = rc.bottom;
		
		// Get the window rect in screen-space
		ClientToScreen( mw_window, &xy );
		ClientToScreen( mw_window, &wh );
		rc.left = xy.x;
		rc.top = xy.y;
		rc.right = wh.x;
		rc.bottom = wh.y;

		// Clip cursor against that
		ClipCursor( &rc );
	}
}

LRESULT CALLBACK MessageUpdate(
	HWND	hWnd,
	UINT	uMsg,
	WPARAM	wParam,
	LPARAM	lParam)
{
	// Get the active window:
	RrWindow* rrWindow = NULL;
	for (RrWindow* window : RrWindow::List())
	{
		if (window->OsShellHandle() == (intptr_t)hWnd)
		{
			rrWindow = window;
			break;
		}
	}

	switch (uMsg)
	{
	case WM_ACTIVATE:
	{
		if (!HIWORD(wParam))
		{
			rrWindow->active = true;
		}
		else
		{
			rrWindow->active = false;	// Program Is No Longer Active
			rrWindow->focused = false;	// Program is no longer focused
		}

		return 0;								// Return To The Message Loop
	}
	case WM_SETFOCUS:
	{
		rrWindow->focused = true;	// Program is no longer focused
		rrWindow->UpdateMouseClipping();
		return 0;
	}
	case WM_KILLFOCUS:
	{
		rrWindow->focused = false;	// Program is no longer focused
		return 0;
	}
	// Window move
	case WM_MOVE:
	{
		if ( rrWindow->focused )
		{
			rrWindow->hiddencursor = false;
			POINT pt;
			GetCursorPos( &pt );
			ScreenToClient( hWnd, &pt );
			CInput::_sysMouseX( pt.x );
			CInput::_sysMouseY( pt.y );
			rrWindow->UpdateMouseClipping();
		}
		return 0;
	}
	// Intercept System Commands
	case WM_SYSCOMMAND:
	{
		switch (wParam)
		{
		case SC_SCREENSAVE:		// Screensaver Trying To Start?
		case SC_MONITORPOWER:	// Monitor Trying To Enter Powersave?
		case SC_KEYMENU:		// ALT-Key nonsense?
			return 0;	// Prevent From Happening
		}
		// Otherwise, we want to let windows handle it:
		break;
	}
	// Window close button
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	// Window resize
	case WM_SIZE:
	{
		rrWindow->hiddencursor = false;
		rrWindow->Resize(LOWORD(lParam), HIWORD(lParam));
		return 0;								// Jump Back
	}

	// File drag & drop
	case WM_DROPFILES:
	{
		TCHAR lpszFile[MAX_PATH] = {0};
		UINT uFile = 0;
		HDROP hDrop = (HDROP)wParam;

		uFile = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, NIL);
		for (UINT i = 0; i < uFile; ++i)
		{
			POINT point = {0};
			DragQueryFile(hDrop, i, lpszFile, MAX_PATH);
			DragQueryPoint(hDrop, &point);
			printf("file %d: \"%s\" @ %d, %d\n", i, lpszFile, point.x, point.y);

			core::shell::arDragAndDropEntry dndEntry;
			dndEntry.filename = lpszFile;
			dndEntry.point = Vector2i(point.x, point.y);
			core::shell::AddDragAndDropEntry(dndEntry);
		}

		DragFinish(hDrop);
		return 0;
	}

	// Mouse movement
	case WM_MOUSEMOVE:
	{
		if ( rrWindow->focused )
		{
			CInput::_sysMouseX( LOWORD(lParam) );
			CInput::_sysMouseY( HIWORD(lParam) );
		}
		return 0;
	}
	case WM_SETCURSOR:
	{ //http://stackoverflow.com/questions/5629613/hide-cursor-in-client-rectangle-but-not-on-title-bar
		WORD ht = LOWORD(lParam);
		if (HTCLIENT == ht && !rrWindow->hiddencursor)
		{
			rrWindow->hiddencursor = true;
			ShowCursor(false);
		}
		else if (HTCLIENT != ht && rrWindow->hiddencursor) 
		{
			rrWindow->hiddencursor = false;
			ShowCursor(true);
		}
	}
	break;
	case WM_INPUT: 
		if ( wParam == RIM_INPUT )
		{
			UINT dwSize = 0;
			static BYTE lpb[128];

			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
			if (dwSize > 128)
				throw core::OutOfMemoryException();

			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));

			RAWINPUT* raw = (RAWINPUT*)lpb;

			if ((raw->header.dwType == RIM_TYPEMOUSE) && ( rrWindow->focused ))
			{
				CInput::_addRawMouseX( raw->data.mouse.lLastX );
				CInput::_addRawMouseY( raw->data.mouse.lLastY );
			} 
			else if ((raw->header.dwType == RIM_TYPEKEYBOARD) && ( rrWindow->focused ))
			{
				ushort vkey		= raw->data.keyboard.VKey;
				ushort flags	= raw->data.keyboard.Flags;
				ushort scankey	= raw->data.keyboard.MakeCode;

				if ( flags&RI_KEY_BREAK )
				{
					CInput::_keyup(_inputtable[vkey], true);
					CInput::_key(_inputtable[vkey], false);
					//SendMessage( hWnd, WM_KEYUP, vkey, 0 );
				}
				else // is a make
				{
					CInput::_keydown(_inputtable[vkey], true);
					CInput::_key(_inputtable[vkey], true);
					//SendMessage( hWnd, WM_KEYDOWN, vkey, 0 );
				}
			}
			else if ( rrWindow->focused )
			{
				//printf("Unknown input");
			}
			return DefWindowProc(hWnd,uMsg,wParam,lParam);
		}
		break;

		// Mouse Buttons
	case WM_LBUTTONDOWN:
	{
		CInput::_mousedown(CInput::MBLeft,true);
		CInput::_mouse(CInput::MBLeft,true);

		rrWindow->UpdateMouseClipping();
		return 0;
	}
	case WM_LBUTTONUP:
	{
		CInput::_mouseup(CInput::MBLeft,true);
		CInput::_mouse(CInput::MBLeft,false);
		return 0;
	}
	case WM_RBUTTONDOWN:
	{
		CInput::_mousedown(CInput::MBRight,true);
		CInput::_mouse(CInput::MBRight,true);

		rrWindow->UpdateMouseClipping();
		return 0;
	}
	case WM_RBUTTONUP:
	{
		CInput::_mouseup(CInput::MBRight,true);
		CInput::_mouse(CInput::MBRight,false);
		return 0;
	}
	case WM_MBUTTONDOWN:
	{
		CInput::_mousedown(CInput::MBMiddle,true);
		CInput::_mouse(CInput::MBMiddle,true);

		rrWindow->UpdateMouseClipping();
		return 0;
	}
	case WM_MBUTTONUP:
	{
		CInput::_mouseup(CInput::MBMiddle,true);
		CInput::_mouse(CInput::MBMiddle,false);
		return 0;
	}
	case WM_XBUTTONDOWN:
	{
		CInput::_mousedown(CInput::MBXtra,true);
		CInput::_mouse(CInput::MBXtra,true);

		rrWindow->UpdateMouseClipping();
		return 0;
	}
	case WM_XBUTTONUP:
	{
		CInput::_mouseup(CInput::MBXtra,true);
		CInput::_mouse(CInput::MBXtra,false);
		return 0;
	}

	// Mouse wheel
	case WM_MOUSEWHEEL:
	{
		CInput::_currMouseW( GET_WHEEL_DELTA_WPARAM(wParam) );
		return 0;
	}
	case WM_HSCROLL:
	{
		debug::Console->PrintWarning( "HScroll message\n" );
		return 0;
	}
	case WM_VSCROLL:
	{
		debug::Console->PrintWarning( "VScroll message\n" );
		return 0;
	}

	// Touchscreen support (nonfunctional)
	case WM_TOUCH:
	{
		debug::Console->PrintWarning( "Touch message\n" );
		return 0;
	}
	case WM_GESTURENOTIFY:
	{
		debug::Console->PrintWarning( "Gesture notify message\n" );
		return 0;
	}
	case WM_GESTURE:
	{
		debug::Console->PrintWarning( "Gesture message\n" );
		return 0;
	}
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

#endif//_WIN32