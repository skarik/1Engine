#include "gpuw/module_config.h"
#ifdef GPU_API_OPENGL
#	include "gpuw/ogl/WriteableResource.h"
#endif

#ifdef GPU_API_VULKAN
#	include "gpuw/vk/WriteableResource.vk.h"
#endif

#ifdef GPU_API_DIRECTX11
#	include "gpuw/dx11/WriteableResource.dx11.h"
#endif