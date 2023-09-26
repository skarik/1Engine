#ifndef GPU_WRAPPER_DEVICE_DX11_H_
#define GPU_WRAPPER_DEVICE_DX11_H_

#include "core/types/types.h"
#include "renderer/types/types.h"
#include "gpuw/Public/Transfer.h"
#include "./BaseContext.dx11.h"
#include "./GraphicsContext.dx11.h"
#include "./ComputeContext.dx11.h"
#include "./gpu.h"
#include "gpuw/base/Device.base.h"

namespace gpu {
namespace dx11
{
	class Fence;
	class OutputSurface;

	class Device : public gpu::base::Device
	{
	public:
		// Non-exposed API for creating device.
		GPUW_EXLUSIVE_API explicit
								Device ( void );
		// Non-exposed API for destroying device.
		GPUW_EXLUSIVE_API		~Device ( void );

		// Non-exposed API for initializing the device. (before OutputSurface ready)
		GPUW_EXLUSIVE_API int	create ( DeviceFeature* features, uint32_t featureCount, DeviceLayer* layers, uint32_t layerCount ) override;
		// Non-exposed API for starting up the device. (after OutputSurface ready)
		GPUW_EXLUSIVE_API int	initialize ( gpu::base::OutputSurface* surface ) override;
		// Non-exposed API for refreshing the device
		//GPUW_EXLUSIVE_API int	refresh ( intptr_t module_handle, intptr_t module_window );

		// Grabs the graphics context for the device.
		//GPUW_API GraphicsContext*
		//						getContext ( void );
		// Grabs the first compute-enabled context for the device.
		//GPUW_API ComputeContext*
		//						getComputeContext ( void );

		GPUW_EXLUSIVE_API int	free ( void );

		// Non-exposed API for grabbing native device
		ID3D11Device*			getNative ( void );
		// Non-exposed API for allocating memory
		//void*					allocateMemory ( uint32_t typeFlags, uint64_t size, uint64_t offset );
		// Non-exposed API for grabbing the immediate device context
		ID3D11DeviceContext*	getImmediateContext ( void );
	private:
		friend OutputSurface;
		//intptr_t			mw_module;
		//intptr_t			mw_window;
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

		//GraphicsContext*	m_graphicsContext;
		//ComputeContext*		m_computeContext;
	};

	//GPUW_API Device* getDevice ( void );
	//GPUW_API void setActiveDevice ( Device* );

}}

#endif//GPU_WRAPPER_BUFFERS_DX11_H_