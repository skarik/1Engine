#ifndef GPU_WRAPPER_OUTPUT_SURFACE_H_
#define GPU_WRAPPER_OUTPUT_SURFACE_H_

#include "core/types/types.h"
#include "renderer/types/types.h"
#include "gpuw/Public/Formats.h"

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

	enum OutputFormat
	{
		// Normal SDR RGB8 format.
		kOutputFormatRGB8,
		// Limited HDR output format.
		kOutputFormatRGB10,
	};

	class Device;
	class RenderTarget;
	class OutputSurface
	{
	public:
		//	create( device, presentMode, width, height, format, fullscreen ) : Creates output surface.
		// In various APIs (such as OpenGL), certain present modes may not be available.
		GPUW_API int			create ( intptr_t window, Device* device, PresentMode presentMode, uint32_t width, uint32_t height, OutputFormat format, bool fullscreen );
		//	destroy() : Frees output surface.
		GPUW_API int			destroy ( void );
		//	getRenderTarget() : Returns this output surface as a render target, for displaying rendered output
		// The target may be used as a normal RenderTarget.
		GPUW_API RenderTarget*
								getRenderTarget ( void );

		//	activate() : Marks this surface as the next one to be rendered to
		GPUW_API void			activate ( void );

		//	present() : Tells the driver to display the current renderable surface.
		// This call may stall.
		GPUW_API int			present ( void );

		//	getWidth() : Returns the width of the current renderable surface
		GPUW_API uint32_t		getWidth ( void );
		//	getHeight() : Returns the width of the current renderable surface
		GPUW_API uint32_t		getHeight ( void );

		//	getDevice() : Returns the associated device this output is associated with.
		GPUW_API Device*		getDevice ( void );

	private:
		friend Device;

		intptr_t			mw_deviceContext;
		uint32_t			m_width;
		uint32_t			m_height;
		OutputFormat		m_format;
		PresentMode			m_presentMode;

		void*				m_dxSwapchain;
		RenderTarget*		m_renderTarget;
		Device*				m_device;
	};
}

#endif//GPU_WRAPPER_OUTPUT_SURFACE_H_