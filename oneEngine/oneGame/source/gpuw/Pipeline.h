#include "gpuw/module_config.h"
#ifdef GPU_API_OPENGL
#	include "gpuw/ogl/Pipeline.h"
#endif

#ifdef GPU_API_VULKAN
#	include "gpuw/vk/Pipeline.vk.h"
#endif