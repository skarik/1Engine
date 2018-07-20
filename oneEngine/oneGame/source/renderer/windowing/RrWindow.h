#ifndef RENDERER_WINDOW_H_
#define RENDERER_WINDOW_H_

#include "core/os.h"
#include "core/types.h"
#include "core/math/vect2d_template.h"
#include "renderer/gpuw/OutputSurface.h"
#include <vector>

#ifdef _WIN32

class CRenderState;

// Class Definition
class RrWindow
{
public:
	RENDER_API explicit		RrWindow(
		HINSTANCE	hInstance,
		HINSTANCE	hPrevInstance,
		LPSTR		lpCmdLine,
		int			nCmdShow );
	RENDER_API virtual		~RrWindow ( void );

	//friend LRESULT	CALLBACK MessageUpdate(HWND, UINT, WPARAM, LPARAM);
	/*
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
	RENDER_API bool isActive ( void );*/
	RENDER_API bool			Show ( void );
	RENDER_API bool			Close ( void );


	RENDER_API bool			AttachRenderer ( CRenderState* renderer );

	//	OsHasFocus() : Does this window have focus?
	RENDER_API bool			OsHasFocus ( void );

	//	OsShellHandle() : Returns the OS handle of this window representation
	RENDER_API intptr_t		OsShellHandle ( void )
		{ return (intptr_t)hWnd; }

	//	OsDevice() : Returns the OS handle of the device this window is on
	RENDER_API intptr_t		OsDevice ( void )
		{ return (intptr_t)hDC; }

	//	List() : Returns list of active windows.
	RENDER_API static std::vector<RrWindow*>
							List ( void )
		{ return m_windows; }

	//	Main() : Returns the main window (index 0 of list)
	// Will return NULL if list is empty.
	RENDER_API static RrWindow*
							Main ( void )
	{
		if (m_windows.empty()) return NULL;
		return m_windows[0];
	}

private:
	/*enum eReturnStatus {
		status_FAILED		= 0,
		status_SUCCESSFUL	= 1
	};*/
	void					CreateScreen ( void );
	void					CreateConsole ( void );
	void					RegisterInput ( void ); // windows thing where we register input devices

	void CreateGfxInstance ( void );
	void CreateGfxSurface ( void );
	void CreateGfxSwapchain ( void );

	void DestroyScreen ( void );
	void DestroyGfxInstance ( void );
	void DestroyGfxSurface ( void );

	static void ReSizeGLScene( GLsizei width, GLsizei height );

	int InitGL( void );

	int DrawScene( void );

	// Delete the window
	void KillGLWindow( bool releaseRenderContext=true );

	// Create the window
	eReturnStatus	CreateGLWindow( char* title, int width, int height, int bits, bool fullscreenflag );
	// Create the context
	eReturnStatus	CreateGLContext ( void );

	// Creates screen buffer to render to
	//eReturnStatus	CreateBuffer ( void );


	eReturnStatus	ErrorOut( const char* message );
	//void showFailureMessage ( char* message );
private:
	static std::vector<RrWindow*>	m_windows;
	//static RrWindow* pActive;
	//static bool	keys[256];			// Array Used For The Keyboard Routine

	//CRenderState*	mRenderer;

private:
	friend LRESULT CALLBACK MessageUpdate(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	Vector2i			m_resolution;
	bool				m_fullscreen;
	int					m_colordepth;

	// OS:

	HINSTANCE	mw_instance;
	HINSTANCE	mw_previnstance;
	LPSTR		mw_cmdline;
	int			mw_cmdshow;

	HWND		mw_window;
	HDC			mw_devicecontext;
	HGLRC		mw_rendercontext;

	// OS:

	//HDC			hDC;		// Private GDI Device Context
							//HGLRC		hRC;		// Permanent Rendering Context
	//HGLRC		hRenderContext;
	//HWND		hWnd;		// Holds Our Window Handle
	//HINSTANCE	hInstance;		// Holds The Instance Of The Application
	//HINSTANCE	hPrevInstance;
	//LPSTR		lpCmdLine;
	//int			nCmdShow;

	// Gfx:

	CRenderState*		m_renderer;
	gpu::OutputSurface	m_surface;

	// Windows Message Loop
	MSG		msg;
	bool	done;
	bool	active;
	bool	focused;

	// Rendering
	//RrRenderTexture*	pSbuf;	// Main screen buffer owned by the window
};

#endif//_WIN32
#endif//RENDERER_WINDOW_H_