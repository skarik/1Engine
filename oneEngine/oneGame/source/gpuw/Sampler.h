#include "gpuw/module_config.h"
#ifdef GPU_API_OPENGL
#	include "gpuw/ogl/Sampler.h"
#endif

#ifdef GPU_API_VULKAN
#	include "gpuw/vk/Sampler.vk.h"
#endif

#ifdef GPU_API_DIRECTX11
#	include "gpuw/dx11/Sampler.dx11.h"
#endif