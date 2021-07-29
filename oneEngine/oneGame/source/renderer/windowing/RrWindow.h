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
		HINSTANCE	hInstance,
		LPSTR		lpCmdLine,
		int			nCmdShow );
	RENDER_API virtual		~RrWindow ( void );

	// Setup
	// ================================

	RENDER_API bool			Show ( void );
	RENDER_API bool			Close ( void );
	//RENDER_API bool			AttachRenderer ( RrRenderer* renderer );
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

	// State update
	// ================================

	RENDER_API bool			UpdateMessages ( void );
	RENDER_API bool			IsDone ( void );
	RENDER_API bool			IsActive ( void );

	RENDER_API void			PostEndMessage ( void );
	RENDER_API void			PostRedrawMessage ( void );


	// Platform specific getters
	// ================================

	//	OsHasFocus() : Does this window have focus?
	//RENDER_API bool			OsHasFocus ( void );

	//	OsShellHandle() : Returns the OS handle of this window representation
	RENDER_API intptr_t		OsShellHandle ( void )
		{ return (intptr_t)mw_window; }

	//	GpuDevice() : Returns the GPU device this renderer is using.
	//RENDER_API gpu::Device*	GpuDevice ( void )
	//	{ return m_device; } // TODO: For DX11, this should be the same across multiple windows.

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
	void					CreateConsole ( void );
	void					RegisterInput ( void ); // windows thing where we register input devices

	//void					CreateGfxInstance ( void );
	void					CreateGfxSurface ( void );
	void					CreateGfxSwapchain ( void );

	void					DestroyScreen ( void );
	//void					DestroyGfxInstance ( void );
	void					DestroyGfxSurface ( void );

	void					UpdateMouseClipping ( void );

private:
	static std::vector<RrWindow*>	m_windows;

private:
	friend LRESULT CALLBACK MessageUpdate(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	int					m_windowListIndex;

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
	LPSTR		mw_cmdline;
	int			mw_cmdshow;

	HWND		mw_window = NIL;

	// Gfx:

	RrRenderer*			m_renderer;
	//gpu::Device*		m_device;
	gpu::OutputSurface	m_surface;

	// Windows Message Loop:

	MSG		msg;
	bool	done = false;
	bool	active = true;
	bool	focused = true;
	bool	hiddencursor = false;
};

#endif//_WIN32
#endif//RENDERER_WINDOW_H_