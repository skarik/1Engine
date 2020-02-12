#include "gpuw/module_config.h"
#ifdef GPU_API_OPENGL
#	include "gpuw/ogl/Shader.h"
#endif

#ifdef GPU_API_VULKAN
#	include "gpuw/vk/Shader.vk.h"
#endif

#ifdef GPU_API_DIRECTX11
#	include "gpuw/dx11/Shader.dx11.h"
#endif