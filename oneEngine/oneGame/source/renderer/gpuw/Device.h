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

	class Device
	{
	public:
		// Non-exposed API for creating device.
		explicit				Device ( void );
		// Non-exposed API for destroying device.
								~Device ( void );

		//	DeviceSetFillMode( device, fillMode ) : Set device's fill mode.
		// Controls how to fill polygons for given device. (glPolygonMode in OpenGL)
		// NULL device sets for current active device.
		//RENDER_API void			setFillMode( const FillMode fillMode );

		//RENDER_API void			sync ( Fence* fence );
		RENDER_API GraphicsContext*	getContext ( void );

		RENDER_API ComputeContext*	getComputeContext ( void );
	};

	RENDER_API Device* getDevice ( void );

}

#endif//GPU_WRAPPER_BUFFERS_H_