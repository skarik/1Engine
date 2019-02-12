#ifndef GPU_WRAPPER_OUTPUT_SURFACE_H_
#define GPU_WRAPPER_OUTPUT_SURFACE_H_

#include "core/types/types.h"
#include "renderer/types/types.h"
#include "renderer/gpuw/Public/Formats.h"

namespace gpu
{
	enum PresentMode
	{
		// No Sync
		// Will allow tearing.
		kPresentModeImmediate,
		// Normal V-Sync
		kPresentModeFIFO,
		// V-Sync, but allows for late images, immediately rolling on an empty FIFO.
		// Will allow tearing.
		kPresentModeFIFORelaxed,
		// V-Sync, but will replace the old images when FIFO is full.
		// Essentially a triple-buffering implementation.
		kPresentModeMailbox
	};

	class Device;
	class RenderTarget;
	class OutputSurface
	{
	public:
		//	create( device, presentMode, width, height ) : Creates output surface.
		// In various APIs (such as OpenGL), certain present modes may not be available.
		RENDER_API int			create ( Device* device, PresentMode presentMode, uint32_t width, uint32_t height );
		//	destroy() : Frees output surface.
		RENDER_API int			destroy ( void );
		//	getRenderTarget() : Returns this output surface as a render target, for displaying rendered output
		// The target may be used as a normal RenderTarget.
		RENDER_API RenderTarget*
								getRenderTarget ( void );

		//	present() : Tells the driver to display the current renderable surface.
		// This call may stall.
		RENDER_API int			present ( void );

		RENDER_API uint32_t		getWidth ( void );
		RENDER_API uint32_t		getHeight ( void );

	private:
		intptr_t			mw_deviceContext;
		uint32_t			m_width;
		uint32_t			m_height;
	};
}

#endif//GPU_WRAPPER_OUTPUT_SURFACE_H_