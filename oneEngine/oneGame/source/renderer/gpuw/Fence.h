#ifndef GPU_WRAPPER_FENCE_H_
#define GPU_WRAPPER_FENCE_H_

#include "core/types/types.h"
#include "renderer/types/types.h"

namespace gpu
{
	class GraphicsContext;
	class ComputeContext;

	class Fence
	{
	public:
		RENDER_API bool			signaled ( void );

		RENDER_API bool			create ( Device* device );
		RENDER_API bool			destroy ( Device* device );

	private:
		unsigned int		m_syncId;
		GraphicsContext*	m_gfxc;
		ComputeContext*		m_compute;
	};
}

#endif//GPU_WRAPPER_FENCE_H_