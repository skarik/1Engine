#ifndef GPU_WRAPPER_DEVICE_BASE_H_
#define GPU_WRAPPER_DEVICE_BASE_H_

#include "core/types/types.h"
#include "renderer/types/types.h"
#include "gpuw/Public/Transfer.h"
/*#include "./BaseContext.h"
#include "./GraphicsContext.h"
#include "./ComputeContext.h"
#include "./gpu.h"*/

namespace gpu
{
	enum DeviceLayer
	{
		kDeviceLayerDebug,
	};

	enum DeviceFeature
	{
		kDeviceFeatureRaytracing,
	};

	namespace base
	{
		class Fence;
		class GraphicsContext;
		class ComputeContext;
		class OutputSurface;

		class Device
		{
		public:
			// Non-exported API for creating device (must be created internally to GPU Wrapper).
			explicit				Device ( void );
			// Non-exposed API for destroying device.
			GPUW_EXLUSIVE_API		~Device ( void );

			// @brief Non-exposed API for initializing the device. (before OutputSurface ready)
			GPUW_EXLUSIVE_API virtual int
									create ( DeviceFeature* features, uint32_t featureCount, DeviceLayer* layers, uint32_t layerCount ) =0;
			// @brief Non-exposed API for starting up the device. (after OutputSurface ready)
			GPUW_EXLUSIVE_API virtual int
									initialize ( OutputSurface* surface ) =0;

			GPUW_EXLUSIVE_API virtual int
									free ( void ) =0;

			// Non-exported API for grabbing native device
			/*virtual void*			getNative ( void ) { return nullptr; }
			// Non-exported API for allocating memory
			virtual void*			allocateMemory ( TransferStyle typeFlags, uint64_t size, uint64_t offset ) { return nullptr; }
			// Non-exported API for grabbing the immediate device context
			virtual void*			getImmediateContext ( void ) { return nullptr; }*/
		private:
			friend OutputSurface;
		};
	}

	GPUW_API gpu::base::Device* getDevice ( void );
	GPUW_API void setActiveDevice ( gpu::base::Device* );
}

#endif//GPU_WRAPPER_DEVICE_BASE_H_