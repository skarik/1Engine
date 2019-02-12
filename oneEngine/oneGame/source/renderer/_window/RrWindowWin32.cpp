#include "core/types/types.h"
#include "core/settings/CGameSettings.h"
#include "core/system/Screen.h"
#include "core/input/CInput.h"
#include "core/time.h"
#include "core/debug/CDebugConsole.h"

#include "core-ext/profiler/CTimeProfiler.h"
#include "core-ext/system/shell/Status.h"

#include "rrWindowWin32.h"
#include "renderer/state/Settings.h"
#include "renderer/state/RrRenderer.h"
#include "renderer/texture/RrRenderTexture.h"

#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

#include "core/os.h"
#include "core-ext/system/shell/DragAndDrop.h"
#include <shellapi.h>

namespace window
{
	//	ShowErrorMessage(string) : Shows an error message, creating a message window with the OS.
	void ShowErrorMessage ( const char* fmt, ... )
	{
		const int kBufferSize = 1024 * 16;
		char buffer[kBufferSize];

		va_list argptr;
		va_start(argptr, fmt);
		vsnprintf(buffer, kBufferSize, fmt, argptr);
		va_end(argptr);

		MessageBox(NULL, buffer, "Error", MB_OK | MB_ICONEXCLAMATION);
	}
}

//bool	active;		// Window Active Flag Set To TRUE By Default
bool	fullscreen;	// Fullscreen Flag Set To Fullscreen Mode By Default

RrWindow* RrWindow::pActive = NULL;
bool	RrWindow::keys[256];

RrWindow::RrWindow(
					HINSTANCE	inhInstance,			// Instance
					HINSTANCE	inhPrevInstance,		// Previous Instance
					LPSTR		inlpCmdLine,			// Command Line Parameters
					int			innCmdShow)
{
	// Create the GL system now.
	new glMainSystem;
	new glDrawing;
	GL_ACCESS; // We are using the glMainSystem accessor

	pActive = this;
	mRenderer = NULL;

	hDC	= NULL;
	hRenderContext	= NULL;
	hWnd= NULL;

	//pSbuf=NULL;

	active		= true;
	focused		= true;
	fullscreen	= false;

	hInstance	= inhInstance;
	hPrevInstance = inhPrevInstance;
	lpCmdLine	= inlpCmdLine;
	nCmdShow	= innCmdShow;

	pWindowName = "Test Window";
	iWidth	= CGameSettings::Active()->i_ro_TargetResX;
	iHeight = CGameSettings::Active()->i_ro_TargetResY;
	/*if ( CGameSettings::Active()->b_ro_UseHighRange )
		iColorDepth = 16;
	else
		iColorDepth = 8;*/
	iColorDepth = 16;

	done = false;

	// Set the render settings
	renderer::Settings.lightingEnabled = true;
	renderer::Settings.clearColor = Color( 0,0,0, 1 );
	renderer::Settings.swapIntervals = 0;
	renderer::Settings.maxLights = 8; // Forward rendering light count (still used)

	//RenderBegintings.ambientColor = Color( 0.2f,0.15f,0.3f, 1 );
	renderer::Settings.ambientColor = Color( 0.02f,0.015f,0.03f, 1 );

	renderer::Settings.fogEnabled = true;
	renderer::Settings.fogColor = Color( 0.27f,0.24f,0.3f, 1 );
	renderer::Settings.fogStart = 10.0f;
	renderer::Settings.fogEnd = 300.0f;

	debug::CDebugConsole::Init();

	// Create the window
	if ( createWindow() )
	{	
		// Set the swap number
		GL.SetSwapInterval( renderer::Settings.swapIntervals );
	}
	
}
RrWindow::~RrWindow ( void )
{
	// If we're not in fullscreen mode
	if ( !fullscreen )
	{	// save the window position
		RECT window_rect;
		GetWindowRect(hWnd, &window_rect);
		CGameSettings::Active()->EditSetting("cl_windowposx",window_rect.left);
		CGameSettings::Active()->EditSetting("cl_windowposy",window_rect.top);
		// Force an app data save
		CGameSettings::Active()->SaveSettings();
	}

	// delete the active systems
	delete glMainSystem::ActiveReference();
	delete glDrawing::ActiveReference();

	// kill the ogl context
	wglMakeCurrent( NULL,NULL );
	wglDeleteContext( hRenderContext );
	
	// close misc windows
	debug::CDebugConsole::Free();
}

bool RrWindow::isActive ( void )
{
	return active;
}

bool RrWindow::createWindow ( void )
{
	if ( !CreateGLWindow(pWindowName,iWidth,iHeight,iColorDepth,fullscreen) )
	{
		done = true;
		ErrorOut( "Failed to create OpenGL window." );
		return false;
	}

	core::shell::SetTaskbarProgressHandle(GetShellHandle());

	return true;
}

RrWindow::eReturnStatus RrWindow::ErrorOut ( const char* message )
{
	window::ShowErrorMessage(message);
	return status_FAILED;
}

bool RrWindow::UpdateMessages ( void )
{
	if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Is There A Message Waiting?
	{	// Have We Received A Quit Message?
		if (msg.message==WM_QUIT)
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

void RrWindow::sendEndMessage ( void )
{
	PostMessage( hWnd, WM_QUIT, NIL, NIL );
}

bool RrWindow::Redraw ( void )
{
	//TimeProfiler.BeginTimeProfile( "WD_Preswap" );
	DrawScene();					// Draw The Scene
	//TimeProfiler.EndTimeProfile( "WD_Preswap" );
	//GL.UpdateBuffer();
	//
	/*//TimeProfiler.BeginTimeProfile( "WD_Swap" );
	glFlush();
	SwapBuffers(hDC);				// Swap Buffers (Double Buffering) (VSYNC)
	//TimeProfiler.EndTimeProfile( "WD_Swap" );*/
	return true;
}


bool RrWindow::canContinue ( void )
{
	return (!done);
}

void RrWindow::toggleFullscren ( void )
{
	KillGLWindow( false );				// Kill Our Current Window (but keep the render context)
	fullscreen=!fullscreen;				// Toggle Fullscreen / Windowed Mode

	// Load settings from the file
	iWidth	= CGameSettings::Active()->i_ro_TargetResX;
	iHeight = CGameSettings::Active()->i_ro_TargetResY;
	std::cout << "aHeight: " << iHeight << "  aWidth: " << iWidth << "  aCDP: " << iColorDepth << std::endl;

	// Recreate Our OpenGL Window
	if ( !CreateGLWindow(pWindowName,iWidth,iHeight,iColorDepth,fullscreen) )
	{
		//done = true;
		//showFailureMessage( "Failed to create OpenGL window." );
		fullscreen=false;
		if ( !CreateGLWindow(pWindowName,iWidth,iHeight,iColorDepth,fullscreen) )
		{
			ErrorOut( "Failed to create OpenGL window." );
		}
	}
}

GLvoid RrWindow::ReSizeGLScene(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window
{
	if (height==0)							// Prevent A Divide By Zero By
	{
		height=1;							// Making Height Equal One
	}
	/*
	Screen::Info.width = width;
	Screen::Info.height = height;
	Screen::Info.scale = height/720.0f;
	Screen::Info.Update();

	glViewport(0,0,Screen::Info.width,Screen::Info.height);			// Reset The Current Viewport (RUNS SEPARATE OGL INSTANCE FROM RrRenderer_Render.cpp)
	printf( "vp: %x\n", wglGetCurrentContext() );

	pActive->CreateBuffer();							// Reset the buffer
	*/

	pActive->iWidth	= width;
	pActive->iHeight= height;
}

int RrWindow::InitGL( void )
{
	// Get GL context:
	const uchar* vendor		= glGetString(GL_VENDOR);
	const uchar* renderer	= glGetString(GL_RENDERER);
	// Print the result
	debug::Console->PrintMessage("device vendor: " + std::string((const char*)vendor) + "\n");
	debug::Console->PrintMessage("     renderer: " + std::string((const char*)renderer) + "\n");

	if ( !CGameSettings::Active()->b_ro_EnableShaders ) {
		//glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	}

	//glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping
	glClearColor(0.0F, 0.0F, 0.0F, 0.0F);				// Black Background
	glClearDepth(1.0F);									// Depth Buffer Setup
	glClearStencil(0);
	/*glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthMask(GL_TRUE);								// Enable depth mask 
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_FRAGMENT_SHADER_DERIVATIVE_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	glHint(GL_POLYGON_SMOOTH_HINT, GL_DONT_CARE);					// Want nice fog, but don't really care
	//glHint(GL_FOG_HINT, GL_DONT_CARE);					// Want nice fog, but don't really care
	*/
	glEnable(GL_CULL_FACE);								// Enabled backface culling
	glEnable(GL_SCISSOR_TEST);

	return TRUE;										// Initialization Went OK
}

// Draws the actual scene by calling into the renderer
int RrWindow::DrawScene( void )
{
	GL_ACCESS;

	// Update the opengl render viewport if there's been a change, or if current forward buffer is invalid
	if ( SceneRenderer->GetForwardBuffer() == NULL || Screen::Info.width != iWidth || Screen::Info.height != iHeight )
	{
		wglMakeCurrent( hDC,hRenderContext );

		Screen::Info.width = iWidth;
		Screen::Info.height = iHeight;
		Screen::Info.scale = iHeight/720.0f;
		Screen::Info.Update();

		// Reset the buffers:
		SceneRenderer->CreateTargetBuffers();

		// Set new viewport rect (this is redundant here)
		GL.setupViewport( 0,0, Screen::Info.width,Screen::Info.height );
	}

#define _ENGINE_SMOOTH_FRAMES
#define _ENGINE_LIMIT_FRAMES

#ifdef _ENGINE_SMOOTH_FRAMES
	// It's possible the rendering may stutter due to sync issues.
	// To alleviate that, we apply frame smoothing: if the current frame is way faster than the previous frame, we slow down the current frame.
	// Thus, we track the speed of the previous frame and current frame.
	static auto time_delta_prev = std::chrono::microseconds(0);
	auto time_start_render = std::chrono::system_clock::now();
#endif

	// Render out the scene.
	mRenderer->Render();

#ifdef _ENGINE_SMOOTH_FRAMES
	// Get the time it took to finish rendering and either buffer swap or vsync.
	auto time_end_render = std::chrono::system_clock::now();
	auto time_delta = std::chrono::duration_cast<std::chrono::microseconds>(time_end_render - time_start_render);

	// Is this frame faster?
	if (time_delta < time_delta_prev)
	{	// Smooth out stutters to a simple 80% exp curve.
		auto sleep_time = (time_delta_prev - time_delta) * 0.8;
		sleep_time = std::min(sleep_time, std::chrono::duration<double, std::micro>(1000*100));
		std::this_thread::sleep_for(sleep_time);
	}
	// Save previous frame.
	time_delta_prev = time_delta;
#endif

#ifdef _ENGINE_LIMIT_FRAMES
	// Slow down the framerate if it's too fast.
	// Incredibly high framerates can cause massive issues with the windows Window Manager.
	// To alleviate this, we sleep for 0.1 ms if we ever get too fast.
	int fps = int(1.0f/Time::smoothDeltaTime);
	if ( fps > 600 )
	{
		std::this_thread::sleep_for(std::chrono::microseconds(100));
	}
#endif

#ifdef _ENGINE_DEBUG
	// Change the window text to show the framerate and frametime when in debug mode
	{
		char szTitle[256]={0};
		sprintf( szTitle, "%d FPS :: FT: %d ms ", fps, int(Time::smoothDeltaTime*1000.0f) );
		SetWindowText( hWnd, szTitle );
	}
#endif

	return TRUE; // Everything went OK
}

// Properly Kill The Window
GLvoid RrWindow::KillGLWindow( bool releaseRenderContext )
{
	// If we're not in fullscreen mode
	if ( !fullscreen )
	{	// save the window position
		RECT window_rect;
		GetWindowRect(hWnd, &window_rect);
		CGameSettings::Active()->EditSetting("cl_windowposx",window_rect.left);
		CGameSettings::Active()->EditSetting("cl_windowposy",window_rect.top);
	}

	// Check if we're in fullscreen mode
	if (fullscreen)
	{	// If So Switch Back To The Desktop and show the pointer
		ChangeDisplaySettings(NULL,0);
		ShowCursor(TRUE);
	}

	// Do we want to kill the RC? We may not want to, especially if we're resizing the window
	if ( releaseRenderContext )
	{
		// Do We Have A Rendering Context?
		if (hRenderContext)
		{
			// Release the device and rendering context
			if ( !wglMakeCurrent(NULL,NULL) ) {
				ErrorOut("Release Of DC And RC Failed.");
			}
			// Are We Able To Delete The RC?
			if ( !wglDeleteContext(hRenderContext) ) {
				ErrorOut("Release Rendering Context Failed.");
			}
			// Reset rendering context to null state
			hRenderContext = NULL;
		}
	}

	// Are We Able To Release The DC
	if ( hDC && !ReleaseDC(hWnd,hDC) ) {
		ErrorOut("Release Device Context Failed.");
		// Set DC To NULL
		hDC = NULL;
	}

	// Are We Able To Destroy The Window?
	if ( hWnd && !DestroyWindow(hWnd) ) {
		ErrorOut("Could Not Release hWnd.");
		// Set hWnd To NULL
		hWnd = NULL;
	}

	// Are We Able To Unregister Class
	if ( !UnregisterClass("OpenGL",hInstance) ) {
		ErrorOut("Could Not Unregister Class.");
		// Set hInstance To NULL
		hInstance = NULL;
	}
}

RrWindow::eReturnStatus RrWindow::CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	// Ensure at least a 1x1 pixel buffer. If not, the system will fail.
	if ( width <= 0 ) {
		width = 1;
	}
	if ( height <= 0 ) {
		height = 1;
	}

	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;			// Set Left Value To 0
	WindowRect.right=(long)width;		// Set Right Value To Requested Width
	WindowRect.top=(long)0;				// Set Top Value To 0
	WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height

	fullscreen=fullscreenflag;			// Set The Global Fullscreen Flag

	hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC)MessageUpdate;				// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= hInstance;							// Set The Instance
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= "OpenGL";								// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		//MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		//return FALSE;											// Return FALSE
		printf( "Failed To Register The Window Class. (Has the class already been registered?)\n" );
	}
	
	if (fullscreen)												// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
		//dmScreenSettings.dmBitsPerPel	= bits;				// Selected Bits Per Pixel
		dmScreenSettings.dmBitsPerPel	= 32;				// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			window::ShowErrorMessage("The Requested Fullscreen Mode Is Not Supported By\nYour Video Card.");
			fullscreen = false;		// Windowed Mode Selected.  Fullscreen = FALSE
			return status_FAILED;
		}
	}

	// Are We Still In Fullscreen Mode?
	if (fullscreen) {
		dwExStyle = WS_EX_APPWINDOW;
		dwStyle = WS_POPUP;
	}
	else {
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle = WS_OVERLAPPEDWINDOW;
	}

	// Adjust Window To True Requested Size
	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);

	// Create The Window
	if (!(hWnd=CreateWindowEx(	dwExStyle,							// Extended Style For The Window
								"OpenGL",							// Class Name
								title,								// Window Title
								dwStyle |							// Defined Window Style
								WS_CLIPSIBLINGS |					// Required Window Style
								WS_CLIPCHILDREN,					// Required Window Style
								0, 0,								// Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								NULL,								// No Parent Window
								NULL,								// No Menu
								hInstance,							// Instance
								NULL)))								// Dont Pass Anything To WM_CREATE
	{
		KillGLWindow();								// Reset The Display
		window::ShowErrorMessage("Window Creation Error.");
		return status_FAILED;						// Return FALSE
	}

	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
													// Pixel format
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,								// Request An RGBA Format
		32,											// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// (Ignored) Color Bits Ignored
		0,											// (Ignored) No Alpha Buffer
		0,											// (Ignored) Shift Bit Ignored
		0,											// (Ignored) No Accumulation Buffer
		0, 0, 0, 0,									// (Ignored) Accumulation Bits Ignored
		24,											// 16Bit Z-Buffer (Depth Buffer)  
		8,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		0,											// (Ignored) Main Drawing Layer
		0,											// (Ignored) Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	// Did We Get A Device Context?
	hDC = GetDC(hWnd);
	if ( !hDC ) {
		KillGLWindow(); // Reset The Display
		return ErrorOut("Can't Create A GL Device Context.");
	}
	// Did Windows Find A Matching Pixel Format?
	PixelFormat = ChoosePixelFormat(hDC,&pfd);
	if ( !PixelFormat )	{
		KillGLWindow();	// Reset The Display
		return ErrorOut("Can't Find A Suitable PixelFormat.");
	}
	// Are We Able To Set The Pixel Format?
	if( !SetPixelFormat(hDC,PixelFormat,&pfd) ) {
		KillGLWindow();	// Reset The Display
		return ErrorOut("Can't Set The PixelFormat.");
	}

	// If no rendering context exists, create one
	if ( !hRenderContext )
	{
		ReSizeGLScene( width, height );
		if ( CreateGLContext() == status_FAILED ) {
			return ErrorOut( "Could not create render context." );
		}
	}
	else // If there's already a rendering context, use the old one
	{
		// Try To Activate The Rendering Context
		if ( !wglMakeCurrent(hDC,hRenderContext) )
		{
			KillGLWindow();								// Reset The Display
			return ErrorOut("Can't Activate The GL Rendering Context.");
		}

		ShowWindow(hWnd,SW_SHOW);						// Show The Window
		SetForegroundWindow(hWnd);						// Slightly Higher Priority
		SetFocus(hWnd);									// Sets Keyboard Focus To The Window
		ReSizeGLScene(width, height);					// Set Up Our Perspective GL Screen

		// Set view info
		//Screen::Info.width = width;
		//Screen::Info.height = height;
	}

	// init raw mouse input
	RAWINPUTDEVICE Rid[2];
	Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC; 
	Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE; 
	Rid[0].dwFlags = RIDEV_INPUTSINK;   
	Rid[0].hwndTarget = hWnd;
	Rid[1].usUsagePage = HID_USAGE_PAGE_GENERIC;
	Rid[1].usUsage = 0x06;
	Rid[1].dwFlags = 0;//RIDEV_NOLEGACY;
	Rid[1].hwndTarget = hWnd;
	if ( !RegisterRawInputDevices(Rid, 2, sizeof(RAWINPUTDEVICE) ) ) {
		printf( "BAD HID REGISTRATION.\n" );
		throw std::exception("BAD HID REGISTRATION");
	}
	// init touch input
	RegisterTouchWindow( hWnd, 0 );
	// set drag and drop
	DragAcceptFiles( hWnd, TRUE );

	// If not fullscreen, start the window in the center of the screen (or where it was last set to)
	if ( !fullscreen )
	{
		// Center window
		RECT rc;
		int screenWidth = GetSystemMetrics(SM_CXSCREEN);
		int screenHeight = GetSystemMetrics(SM_CYSCREEN);
		GetWindowRect( hWnd, &rc );
		int window_pos_x = (screenWidth - rc.right)/2;
		int window_pos_y = (screenHeight - rc.bottom)/2;
		// Grab saved window position
		if ( CGameSettings::Active()->GetSettingExists("cl_windowposx") )
			window_pos_x = CGameSettings::Active()->GetSettingAsInt("cl_windowposx");
		if ( CGameSettings::Active()->GetSettingExists("cl_windowposy") )
			window_pos_y = CGameSettings::Active()->GetSettingAsInt("cl_windowposy");
		// Set window position
		SetWindowPos( hWnd, 0,
			window_pos_x,
			window_pos_y,
			0, 0, SWP_NOZORDER|SWP_NOSIZE );
	}

	// Return success
	return status_SUCCESSFUL;
}

RrWindow::eReturnStatus	RrWindow::CreateGLContext ( void )
{
	GL_ACCESS; // We are using the glMainSystem accessor

	// Create a temporary context to load the OpenGL system
	HGLRC tempContext = wglCreateContext(hDC);
	wglMakeCurrent(hDC, tempContext);

	// Attempt to load up Windows OGL extension
	if ( wgl_LoadFunctions( hDC ) == wgl_LOAD_FAILED ) {
		//throw core::NullReferenceException();
		wglCreateContextAttribsARB = NULL;
	}
	// Set target attributes to load OpenGL 3.3
	int attribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,
		WGL_CONTEXT_FLAGS_ARB, 0,
		0
	};

	// Create the context with whatever option is available.
	if ( wglCreateContextAttribsARB ) {
		hRenderContext = wglCreateContextAttribsARB(hDC,0,attribs);	
	}
	else {
		hRenderContext = wglCreateContext(hDC);
	}

	// Are We Able To Get A Rendering Context?
	if ( !hRenderContext ) {
		KillGLWindow();								// Reset The Display
		return ErrorOut("Can't Create A GL Rendering Context.");
	}

	// Delete the temporary context
	wglMakeCurrent( NULL, NULL );
	wglDeleteContext( tempContext );

	// Try To Activate The Rendering Context
	if ( !wglMakeCurrent(hDC,hRenderContext) ) {
		KillGLWindow();								// Reset The Display
		return ErrorOut("Can't Activate The GL Rendering Context.");
	}

	// Now, init the OpenGL pointers and functions
	// Get all the OpenGL functions
	try {
		GL.InitializeCommonExtensions();
	}
	catch ( core::NullReferenceException ) {
		debug::Console->PrintError( "Could not create OpenGL 3.3+ device." );
	}

	// Clear out errors now
	while ( glGetError() != 0 ) { ; }

	//Or better yet, use the GL3 way to get the version number
	int OpenGLVersion[2] = {0,0};
	glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);
	printf( "Created an OpenGL %d.%d context\n", OpenGLVersion[0],OpenGLVersion[1] );

	ShowWindow(hWnd,SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window
	//SetCapture(hWnd);								// Sets Mouse Focus to the Window
	//ReSizeGLScene(width, height);					// Set Up Our Perspective GL Screen
	Screen::Info.width	= iWidth;
	Screen::Info.height	= iHeight;
	Screen::Info.scale = iHeight/720.0f;
	Screen::Info.Update();

	// Initialize Our Newly Created GL Window
	if ( !InitGL() ) {
		KillGLWindow();								// Reset The Display
		return ErrorOut("Initialization Failed.");
	}

	// Load window options 
	CGameSettings::Active()->LoadSettings();

	// Create buffer for the window
	/*if ( !CreateBuffer() )
	{
		KillGLWindow();								// Reset The Display
		return ErrorOut("Buffer creation Failed.");
	}*/

	return status_SUCCESSFUL;
}


//RrWindow::eReturnStatus RrWindow::CreateBuffer ( void )
//{
//	// Remove old buffer
//	if ( pSbuf )
//	{
//		delete pSbuf;
//	}
//	pSbuf = NULL;
//	// If requested, make the screen buffer
//	if ( CGameSettings::Active()->b_ro_UseBufferModel )
//	{
//		/*if ( iColorDepth == 8 )
//		{
//			pSbuf = new RrRenderTexture( RGB8, Screen::Info.width, Screen::Info.height, Clamp,Clamp, Texture2D, Depth16, true, false );
//		}
//		else if ( iColorDepth == 16 )
//		{
//			pSbuf = new RrRenderTexture( RGB16F, Screen::Info.width, Screen::Info.height, Clamp,Clamp, Texture2D, Depth32, true, false );
//			//pSbuf = new RrRenderTexture( RGB16, iWidth, iHeight, Clamp,Clamp, Texture2D, Depth32, true,false );
//		}
//		else {
//			throw core::NotYetImplementedException();
//			return status_FAILED;
//		}*/
//		eColorFormat	colorMode	= SceneRenderer->GetSettings().mainColorAttachmentFormat;
//		eDepthFormat	depthMode	= SceneRenderer->GetSettings().mainDepthFormat;
//		eStencilFormat	stencilMode	= SceneRenderer->GetSettings().mainStencilFormat;
//		uint			colorCount	= SceneRenderer->GetSettings().mainColorAttachmentCount;
//
//		pSbuf = new RrRenderTexture(
//			colorMode,
//			Screen::Info.width, Screen::Info.height, Clamp,Clamp, Texture2D,
//			depthMode,		(depthMode == DepthNone) ? false : true,
//			false,
//			stencilMode,	(stencilMode == StencilNone) ? false : true );
//	}
//	return status_SUCCESSFUL;
//}


void WndSetMouseClip ( HWND	hWnd, bool & hiddencursor )
{
	if (   ( CInput::SysMouseX() == std::min<long>( std::max<long>( CInput::SysMouseX(), 0 ), Screen::Info.width ) )
		&& ( CInput::SysMouseY() == std::min<long>( std::max<long>( CInput::SysMouseY(), 0 ), Screen::Info.height ) ))
	{
		hiddencursor = false;
		RECT rc;
		GetClientRect( hWnd, &rc );
		POINT xy, wh;
		xy.x = rc.left;
		xy.y = rc.top;
		wh.x = rc.right;
		wh.y = rc.bottom;
		ClientToScreen( hWnd, &xy );
		ClientToScreen( hWnd, &wh );
		rc.left = xy.x;
		rc.top = xy.y;
		rc.right = wh.x;
		rc.bottom = wh.y;
		ClipCursor( &rc );
	}
}

LRESULT CALLBACK MessageUpdate(
	HWND	hWnd,	// Handle For This Window
	UINT	uMsg,	// Message For This Window
	WPARAM	wParam,	// Additional Message Information
	LPARAM	lParam)	// Additional Message Information
{
	static bool hiddencursor = false;
	switch (uMsg)									// Check For Windows Messages
	{
		case WM_ACTIVATE:							// Watch For Window Activate Message
		{
			if (!HIWORD(wParam))					// Check Minimization State
			{
				RrWindow::pActive->active = true;	// Program Is Active
			}
			else
			{
				RrWindow::pActive->active = false;	// Program Is No Longer Active
				RrWindow::pActive->focused = false;	// Program is no longer focused
			}

			return 0;								// Return To The Message Loop
		}
		case WM_SETFOCUS:
		{
			RrWindow::pActive->focused = true;	// Program is no longer focused
			WndSetMouseClip( hWnd, hiddencursor );
			return 0;
		}
		case WM_KILLFOCUS:
		{
			RrWindow::pActive->focused = false;	// Program is no longer focused
			return 0;
		}
		// Window move
		case WM_MOVE:
		{
			if ( RrWindow::pActive->focused )
			{
				hiddencursor = false;
				POINT pt;
				GetCursorPos( &pt );
				ScreenToClient( hWnd, &pt );
				CInput::_sysMouseX( pt.x );
				CInput::_sysMouseY( pt.y );
				WndSetMouseClip( hWnd, hiddencursor );
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
			hiddencursor = false;
			RrWindow::ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));  // LoWord=Width, HiWord=Height
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
			if ( RrWindow::pActive->focused )
			{
				CInput::_sysMouseX( LOWORD(lParam) );
				CInput::_sysMouseY( HIWORD(lParam) );
			}
			return 0;
		}
		case WM_SETCURSOR:
		{ //http://stackoverflow.com/questions/5629613/hide-cursor-in-client-rectangle-but-not-on-title-bar
			WORD ht = LOWORD(lParam);
			if (HTCLIENT==ht && !hiddencursor)
			{
				hiddencursor = true;
				ShowCursor(false);
			}
			else if (HTCLIENT!=ht && hiddencursor) 
			{
				hiddencursor = false;
				ShowCursor(true);
			}
		}
		break;
		case WM_INPUT: 
		if ( wParam == RIM_INPUT )
		{
			UINT dwSize = 40;
			static BYTE lpb[40];
	    
			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, 
							lpb, &dwSize, sizeof(RAWINPUTHEADER));
	    
			RAWINPUT* raw = (RAWINPUT*)lpb;
	    
			if ((raw->header.dwType == RIM_TYPEMOUSE) && ( RrWindow::pActive->focused ))
			{
				CInput::_addRawMouseX( raw->data.mouse.lLastX );
				CInput::_addRawMouseY( raw->data.mouse.lLastY );

				//return 0;
				return DefWindowProc(hWnd,uMsg,wParam,lParam);
			} 
			else if ((raw->header.dwType == RIM_TYPEKEYBOARD) && ( RrWindow::pActive->focused ))
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

				/*{
					INPUT inputs [1];
					memset( inputs, 0, sizeof(INPUT) * 1 );
					inputs[0].type = INPUT_KEYBOARD;
					inputs[0].ki.wVk = vkey;
					inputs[0].ki.dwFlags = flags;
					inputs[0].ki.wScan = scankey;
					SendInput( 1, inputs, sizeof(INPUT) );
				}*/
				//return DefWindowProc(hWnd,uMsg,wParam,lParam);

				//return 0;
				//return DefWindowProc(hWnd,uMsg,wParam,lParam);
			}
			return DefWindowProc(hWnd,uMsg,wParam,lParam);
		}
		break;

		// Mouse Buttons
		case WM_LBUTTONDOWN:
		{
			CInput::_mousedown(CInput::MBLeft,true);
			CInput::_mouse(CInput::MBLeft,true);

			WndSetMouseClip( hWnd, hiddencursor );
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

			WndSetMouseClip( hWnd, hiddencursor );
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

			WndSetMouseClip( hWnd, hiddencursor );
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

			WndSetMouseClip( hWnd, hiddencursor );
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