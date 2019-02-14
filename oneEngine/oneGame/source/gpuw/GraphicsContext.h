#include "gpuw/module_config.h"
#ifdef GPU_API_OPENGL
#	include "gpuw/ogl/GraphicsContext.h"
#endif

#ifdef GPU_API_VULKAN
#	include "gpuw/vk/GraphicsContext.h"
#endif