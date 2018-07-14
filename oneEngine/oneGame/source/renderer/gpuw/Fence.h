#ifndef GPU_WRAPPER_FENCE_H_
#define GPU_WRAPPER_FENCE_H_

#include "core/types/types.h"
#include "renderer/types/types.h"

namespace gpu
{
	class Fence
	{
	public:

		RENDER_API bool			signaled ( void );

		RENDER_API bool			create ( Device* device );
		RENDER_API bool			destroy ( Device* device );

	};
}

#endif//GPU_WRAPPER_FENCE_H_