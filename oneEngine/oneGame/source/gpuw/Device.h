/*#include "gpuw/module_config.h"
#ifdef GPU_API_OPENGL
#	include "gpuw/ogl/Device.h"
#endif

#ifdef GPU_API_VULKAN
#	include "gpuw/vk/Device.vk.h"
#endif

#ifdef GPU_API_DIRECTX11
#	include "gpuw/dx11/Device.dx11.h"
#endif*/
#ifndef GPU_WRAPPER_DYNAMIC_DEVICE_H_
#define GPU_WRAPPER_DYNAMIC_DEVICE_H_

#include "gpuw/gpuw_common.h"
#include "gpuw/base/Device.base.h"

namespace gpu
{
	typedef gpu::base::Device Device;

	gpu::base::Device* createDynamicDevice ( void );
}

#endif//GPU_WRAPPER_DYNAMIC_DEVICE_H_