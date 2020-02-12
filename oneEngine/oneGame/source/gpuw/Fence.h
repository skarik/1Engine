#include "gpuw/module_config.h"
#ifdef GPU_API_OPENGL
#	include "gpuw/ogl/Fence.h"
#endif

#ifdef GPU_API_VULKAN
#	include "gpuw/vk/Fence.vk.h"
#endif

#ifdef GPU_API_DIRECTX11
#	include "gpuw/dx11/Fence.dx11.h"
#endif