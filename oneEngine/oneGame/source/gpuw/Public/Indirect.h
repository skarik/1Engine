#ifndef GPU_WRAPPER_GRAPHICS_INDIRECT_H_
#define GPU_WRAPPER_GRAPHICS_INDIRECT_H_

#include "core/types/types.h"
#include "gpuw/Public/Enums.h"
#include "renderer/types/types.h"

namespace gpu
{
	struct DrawIndirectArgs
	{
		uint32_t		indexCount;
		uint32_t		instanceCount;
		uint32_t		firstIndex;
		int32_t			baseVertex;		// Cannot be negative in OpenGL.
		uint32_t		baseInstance;
	};

	struct DispatchIndirectArgs
	{
		uint32_t		groupCountX;
		uint32_t		groupCountY;
		uint32_t		groupCountZ;
	};
}

#endif//GPU_WRAPPER_GRAPHICS_INDIRECT_H_