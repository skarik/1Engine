#ifndef GPU_WRAPPER_DEVICE_H_
#define GPU_WRAPPER_DEVICE_H_

#include "core/types/types.h"
//#include "core/gfx/textureFormats.h"
#include "renderer/types/types.h"
#include "renderer/gpuw/Transfer.h"
#include "renderer/gpuw/GraphicsContext.h"
//#include "renderer/gpuw/ComputeContext.h"

namespace gpu
{
	class Fence;
	class ComputeContext;
	class OutputSurface;

	class Device
	{
	public:
		// Non-exposed API for creating device.
		explicit				Device ( intptr_t module_handle, intptr_t module_window );
		// Non-exposed API for destroying device.
								~Device ( void );

		// Non-exposed API for initializing the device.
		int						create ( void );
		// Non-exposed API for refreshing the device
		int						refresh ( intptr_t module_handle, intptr_t module_window );

		// Grabs the graphics context for the device.
		RENDER_API GraphicsContext*
								getContext ( void );
		// Grabs the first compute-enabled context for the device.
		RENDER_API ComputeContext*
								getComputeContext ( void );

	private:
		friend OutputSurface;
		intptr_t			mw_module;
		intptr_t			mw_window;
		intptr_t			mw_deviceContext;
		intptr_t			mw_renderContext;

		GraphicsContext*	m_graphicsContext;
		ComputeContext*		m_computeContext;
	};

	RENDER_API Device* getDevice ( void );

}

#endif//GPU_WRAPPER_BUFFERS_H_