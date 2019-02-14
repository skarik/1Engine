#include "gpuw/module_config.h"
#ifdef GPU_API_OPENGL
#	include "gpuw/ogl/Fence.h"
#endif

#ifdef GPU_API_VULKAN
#	include "gpuw/vk/Fence.h"
#endif