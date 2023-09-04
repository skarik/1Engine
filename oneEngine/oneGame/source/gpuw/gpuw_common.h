#ifndef GPU_WRAPPER_COMMON_H_
#define GPU_WRAPPER_COMMON_H_

#include "module_config.h"
#include "core/types/types.h"
#include "core/mem.h"

namespace gpu
{
	enum Api
	{
#if GPU_API_DIRECTX11
		kApiDirectX11,
#endif
#if GPU_API_OPENGL
		kApiOpenGL,
#endif
#if GPU_API_VULKAN
		kApiVulkan,
#endif
	};


	GPUW_API Api GetApi();
}

#endif//GPU_WRAPPER_COMMON_H_