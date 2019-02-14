#include "gpuw/module_config.h"
#ifdef GPU_API_OPENGL
#	include "gpuw/ogl/OutputSurface.h"
#endif

#ifdef GPU_API_VULKAN
#	include "gpuw/vk/OutputSurface.h"
#endif