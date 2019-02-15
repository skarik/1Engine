#include "gpuw/module_config.h"
#ifdef GPU_API_OPENGL
#	include "gpuw/ogl/ComputeContext.h"
#endif

#ifdef GPU_API_VULKAN
#	include "gpuw/vk/ComputeContext.vk.h"
#endif