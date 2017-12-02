#ifndef GPU_WRAPPER_DEVICE_H_
#define GPU_WRAPPER_DEVICE_H_

#include "core/types/types.h"
#include "renderer/types/types.h"
#include "renderer/types/textureFormats.h"

#include "renderer/gpuw/Transfer.h"

namespace gpu
{
	class Device
	{
	
	};

	enum FillMode
	{
		kFillModeSolid,
		kFillModeWireframe,
	};

	//	DeviceSetFillMode( device, fillMode ) : Set device's fill mode.
	// Controls how to fill polygons for given device. (glPolygonMode in OpenGL)
	// NULL device sets for current active device.
	RENDER_API void DeviceSetFillMode( Device* device, const FillMode fillMode );
}

#endif//GPU_WRAPPER_BUFFERS_H_