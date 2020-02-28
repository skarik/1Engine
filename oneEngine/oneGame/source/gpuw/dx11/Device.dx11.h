#ifndef GPU_WRAPPER_DEVICE_H_
#define GPU_WRAPPER_DEVICE_H_

#include "core/types/types.h"
#include "renderer/types/types.h"
#include "gpuw/Public/Transfer.h"
#include "./GraphicsContext.dx11.h"
#include "./ComputeContext.dx11.h"
#include "./gpu.h"

namespace gpu
{
	class Fence;
	class GraphicsContext;
	class ComputeContext;
	class OutputSurface;

	enum DeviceLayer
	{
		kDeviceLayerDebug,
	};

	class Device
	{
	public:
		// Non-exposed API for creating device.
		GPUW_EXLUSIVE_API explicit
								Device ( intptr_t module_handle, intptr_t module_window );
		// Non-exposed API for destroying device.
		GPUW_EXLUSIVE_API		~Device ( void );

		// Non-exposed API for initializing the device. (before OutputSurface ready)
		GPUW_EXLUSIVE_API int	create ( DeviceLayer* layers, uint32_t layerCount );
		// Non-exposed API for starting up the device. (after OutputSurface ready)
		GPUW_EXLUSIVE_API int	initialize ( OutputSurface* surface );
		// Non-exposed API for refreshing the device
		GPUW_EXLUSIVE_API int	refresh ( intptr_t module_handle, intptr_t module_window );

		// Grabs the graphics context for the device.
		GPUW_API GraphicsContext*
								getContext ( void );
		// Grabs the first compute-enabled context for the device.
		GPUW_API ComputeContext*
								getComputeContext ( void );

		// Non-exposed API for grabbing native device
		GPUW_EXLUSIVE_API ID3D11Device*
								getNative ( void );
		// Non-exposed API for allocating memory
		GPUW_EXLUSIVE_API void*	allocateMemory ( uint32_t typeFlags, uint64_t size, uint64_t offset );
		// Non-exposed API for grabbing native device context
		GPUW_EXLUSIVE_API ID3D11DeviceContext*
								getNativeContext ( void );
	private:
		friend OutputSurface;
		intptr_t			mw_module;
		intptr_t			mw_window;
		/*intptr_t			mw_deviceContext;
		intptr_t			mw_renderContext;*/

	private:
		// Pointers to functions loaded from extensions, that aren't needed to be global.
		/*struct rrExtensionPointers
		{
			PFN_vkCreateDebugReportCallbackEXT	vkCreateDebugReportCallbackEXT;
			PFN_vkDestroyDebugReportCallbackEXT	vkDestroyDebugReportCallbackEXT;
			PFN_vkDebugReportMessageEXT			vkDebugReportMessageEXT;
		};*/

	private:
		/*rrExtensionPointers
							m_extensions;
							*/
		/*VkInstance			m_instance;
		VkDevice			m_device;
		VkAllocationCallbacks*
							m_allocator;*/
		IDXGIFactory*		m_dxFactory;
		IDXGIAdapter*		m_dxAdapter;
		ID3D11Device*		m_dxDevice;
		ID3D11DeviceContext*
							m_dxImmediateContext;

		uint32_t			m_queueIndexGraphics;
		uint32_t			m_queueIndexCompute;
		uint32_t			m_queueIndexTransfer;

		/*VkPhysicalDevice	m_physicalDevice;
		VkPhysicalDeviceProperties
							m_physicalDeviceProperties;

		VkDebugReportCallbackEXT
							m_debugCallback;
							*/

		GraphicsContext*	m_graphicsContext;
		ComputeContext*		m_computeContext;
	};

	GPUW_API Device* getDevice ( void );

}

#endif//GPU_WRAPPER_BUFFERS_H_