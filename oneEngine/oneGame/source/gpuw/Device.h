#include "gpuw/module_config.h"
#ifdef GPU_API_OPENGL
#	include "gpuw/ogl/Device.h"
#endif

#ifdef GPU_API_VULKAN
#	include "gpuw/vk/Device.vk.h"
#endif