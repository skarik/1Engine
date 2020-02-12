#include "gpuw/module_config.h"
#ifdef GPU_API_OPENGL
#	include "gpuw/ogl/ShaderPipeline.h"
#endif

#ifdef GPU_API_VULKAN
#	include "gpuw/vk/ShaderPipeline.vk.h"
#endif

#ifdef GPU_API_DIRECTX11
#	include "gpuw/dx11/ShaderPipeline.dx11.h"
#endif