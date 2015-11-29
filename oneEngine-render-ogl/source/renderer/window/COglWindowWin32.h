

#ifndef _COGL_WINDOW_WIN_32_
#define _COGL_WINDOW_WIN_32_


// Input defines
#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
#endif

// No MFC
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif

// Includes
#include "core/types/types.h"
#include "renderer/ogl/GLCommon.h" // includes windows headers and Win32

#include <stdlib.h>
#include <string.h>
#include <tchar.h>

/*#include "CRenderState.h"
#include "CInput.h"
#include "core/time.h"
#include "CDebugConsole.h"*/

class CRenderState;
class CRenderTexture;

// Function definition
//LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//void initStuff ( void );
///

//void createBuffer ( void );
//void drawBuffer ( void );

// Class Definition
class COglWindowWin32
{
public:
	//COglWindowWin32 ( void );
	//~COglWindowWin32 ( void );
	RENDER_API COglWindowWin32 (
					HINSTANCE	inhInstance,			// Instance
					HINSTANCE	inhPrevInstance,		// Previous Instance
					LPSTR		inlpCmdLine,			// Command Line Parameters
					int			innCmdShow);
	RENDER_API ~COglWindowWin32 ( void );

	friend LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	int TestMain( void );

	RENDER_API bool createWindow ( void );
	// Is this window still valid for executing (no WinAPI end messages)
	RENDER_API bool canContinue ( void );

	// Update WinAPI messages in the queue
	RENDER_API bool UpdateMessages ( void );
	// Render out the next frame
	RENDER_API bool Redraw ( void );

	// Toggle this window being fullscreen
	RENDER_API void toggleFullscren ( void );

	// Manually send the WinAPI end message
	RENDER_API void sendEndMessage ( void );

	// Does this window have focus?
	RENDER_API bool isActive ( void );

	GLdouble getAspect ( void );

	// Return window handles
	HDC	getDevicePointer ( void ) {
		return hDC;
	}
	HWND getWindowHandle ( void ) {
		return hWnd;
	}

	// Return screen buffer
	CRenderTexture* getScreenBuffer ( void ) {
		return pSbuf;
	}

private:
	enum eReturnStatus {
		status_FAILED		= 0,
		status_SUCCESSFUL	= 1
	};

	static void ReSizeGLScene( GLsizei width, GLsizei height );

	int InitGL( void );

	int DrawGLScene( void );

	// Delete the window
	void KillGLWindow( bool releaseRenderContext=true );

	// Create the window
	eReturnStatus	CreateGLWindow( char* title, int width, int height, int bits, bool fullscreenflag );
	// Create the context
	eReturnStatus	CreateGLContext ( void );

	// Creates screen buffer to render to
	eReturnStatus	CreateBuffer ( void );


	eReturnStatus	ErrorOut( const char* message );
	//void showFailureMessage ( char* message );
public:
	static COglWindowWin32* pActive;
	static bool	keys[256];			// Array Used For The Keyboard Routine

	CRenderState*	mRenderer;


private:
	// Window variables and handles
	HDC			hDC;		// Private GDI Device Context
	//HGLRC		hRC;		// Permanent Rendering Context
	HGLRC		hRenderContext;
	HWND		hWnd;		// Holds Our Window Handle
	HINSTANCE	hInstance;		// Holds The Instance Of The Application
	HINSTANCE	hPrevInstance;
	LPSTR		lpCmdLine;
	int			nCmdShow;

	// Window settings
	char*		pWindowName;
	GLsizei		iWidth;
	GLsizei		iHeight;
	int			iColorDepth;

	// Windows Message Loop
	MSG		msg;
	bool	done;
	bool	active;
	bool	focused;

	// Rendering
	CRenderTexture*	pSbuf;	// Main screen buffer owned by the window
};



#endif