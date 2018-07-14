#ifndef GPU_WRAPPER_GRAPHICS_CONTEXT_H_
#define GPU_WRAPPER_GRAPHICS_CONTEXT_H_

#include "core/types.h"
#include <stdint.h>

namespace gpu
{
	enum FillMode
	{
		kFillModeSolid,
		kFillModeWireframe,
	};

	class Fence;
	class GraphicsContext
	{
	public:
		RENDER_API int			reset ( void );

		RENDER_API int			submit ( void );

		//	DeviceSetFillMode( device, fillMode ) : Set device's fill mode.
		// Controls how to fill polygons for given device. (glPolygonMode in OpenGL)
		// NULL device sets for current active device.
		RENDER_API int			setFillMode( const FillMode fillMode );

		RENDER_API int			setViewport ( uint32_t left, uint32_t top, uint32_t right, uint32_t bottom );
		RENDER_API int			setScissor ( uint32_t left, uint32_t top, uint32_t right, uint32_t bottom );

		RENDER_API int			sync ( Fence* fence );
	};
}

#endif//GPU_WRAPPER_GRAPHICS_CONTEXT_H_