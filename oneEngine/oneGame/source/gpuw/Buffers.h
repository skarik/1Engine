#include "gpuw/module_config.h"
#ifdef GPU_API_OPENGL
#	include "gpuw/ogl/Buffers.h"
#endif

#ifdef GPU_API_VULKAN
#	include "gpuw/vk/Buffers.vk.h"
#endif

#ifdef GPU_API_DIRECTX11
#	include "gpuw/dx11/Buffers.dx11.h"
#endif