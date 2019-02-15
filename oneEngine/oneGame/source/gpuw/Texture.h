#include "gpuw/module_config.h"
#ifdef GPU_API_OPENGL
#	include "gpuw/ogl/Texture.h"
#endif

#ifdef GPU_API_VULKAN
#	include "gpuw/vk/Texture.vk.h"
#endif