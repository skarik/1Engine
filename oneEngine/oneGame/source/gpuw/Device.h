#include "gpuw/module_config.h"
#ifdef GPU_API_OPENGL
#	include "gpuw/ogl/Device.h"
#endif

#ifdef GPU_API_VULKAN
#	include "gpuw/vk/Device.vk.h"
#endif

#ifdef GPU_API_DIRECTX11
#	include "gpuw/dx11/Device.dx11.h"
#endif