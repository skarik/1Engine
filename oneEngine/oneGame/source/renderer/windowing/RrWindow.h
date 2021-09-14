#ifndef RENDERER_WINDOW_H_
#define RENDERER_WINDOW_H_

#include "core/os.h"
#include "core/types.h"
#include "core/math/vect2d_template.h"
#include "gpuw/Device.h"
#include "gpuw/OutputSurface.h"
#include <vector>

#ifdef _WIN32

class ArScreen;
class RrRenderer;

// Class Definition
class RrWindow
{
public:
	RENDER_API explicit		RrWindow(
		RrRenderer*	renderer,
		HINSTANCE	hInstance );
	RENDER_API virtual		~RrWindow ( void );

	// Setup
	// ================================

	RENDER_API bool			Show ( void );
	RENDER_API bool			Close ( void );
	RENDER_API bool			Resize ( int width, int height );

	RENDER_API bool			SetFullscreen ( bool fullscreen );
	RENDER_API bool			IsFullscreen ( void );

	RENDER_API bool			SetTitle ( const char* title );

	RENDER_API void			SetWantsHideCursor ( bool hideCursor )
		{ m_wantHideCursor = hideCursor; }
	RENDER_API void			SetWantsSystemCursor ( bool useSystemCursor )
		{ m_wantSystemCursor = useSystemCursor; }
	RENDER_API void			SetWantsClipCursor ( bool clipCursor )
		{ m_wantClipCursor = clipCursor; }
	RENDER_API void			SetZeroInputOnLoseFocus ( bool zeroInput )
		{ m_zeroInputOnLoseFocus = zeroInput; }

	// Getters
	// ================================

	RENDER_API Vector2i		GetSize ( void )
		{ return m_resolution; }
	RENDER_API bool			GetFocused ( void )
		{ return focused; }
	RENDER_API const ArScreen&
							GetScreen ( void );
	RENDER_API const int	GetListIndex ( void )
		{ return m_windowListIndex; }

	// State update
	// ================================

	RENDER_API bool			UpdateMessages ( void );
	//	WantsClose() : Is the window given a CLOSE message?
	// This means the current window requested closing or will be closed.
	RENDER_API bool			WantsClose ( void ) { return wantsClose; }
	//	IsDone() : Is the window given a QUIT message?
	// This means the current app wants to be killed.
	RENDER_API bool			IsDone ( void ) { return done; }
	//	IsActive() : Is the window focused and shown?
	RENDER_API bool			IsActive ( void ) { return active; }

	RENDER_API void			PostEndMessage ( void );
	RENDER_API void			PostRedrawMessage ( void );


	// Platform specific getters
	// ================================

	//	OsShellHandle() : Returns the OS handle of this window representation
	RENDER_API intptr_t		OsShellHandle ( void )
		{ return (intptr_t)mw_window; }

	//	GpuSurface() : Returns the surface the renderer is currently using.
	RENDER_API gpu::OutputSurface*
							GpuSurface ( void )
		{ return &m_surface; }


	// Static Calls
	// ================================
	
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
	
	void					CreateScreen ( void );
	void					RegisterInput ( void ); // windows thing where we register input devices

	void					CreateGfxSurface ( void );
	void					CreateGfxSwapchain ( void );

	void					DestroyScreen ( void );
	void					DestroyGfxSurface ( void );

	void					UpdateMouseClipping ( void );

private:
	static std::vector<RrWindow*>	m_windows;

private:
	friend LRESULT CALLBACK MessageUpdate(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	bool				m_screenReady = false;
	int					m_windowListIndex = -1;

	Vector2i			m_resolution;
	gpu::OutputFormat	m_outputFormat;
	bool				m_fullscreen;
	int					m_colordepth;
	
	bool				m_wantClipCursor = true;
	bool				m_wantSystemCursor = false; // If system mouse position should be used instead of direct hardware stuff
	bool				m_wantHideCursor = true;
	bool				m_zeroInputOnLoseFocus = false;

	// OS:

	HINSTANCE	mw_instance;
	HWND		mw_window = NIL;

	// Gfx:

	RrRenderer*			m_renderer;
	gpu::OutputSurface	m_surface;

	// Windows Message Loop:

	MSG		msg;
	bool	done = false;
	bool	wantsClose = false;
	bool	active = true;
	bool	focused = true;
};

#endif//_WIN32
#endif//RENDERER_WINDOW_H_