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
		RENDER_API int			create ( Device* device, PresentMode presentMode );
		RENDER_API int			destroy ( void );
		RENDER_API RenderTarget*
								getRenderTarget ( void );

		RENDER_API int			present ( void );

	private:
		intptr_t			mw_deviceContext;
	};
}

#endif//GPU_WRAPPER_OUTPUT_SURFACE_H_