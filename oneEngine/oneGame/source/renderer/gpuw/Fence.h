#ifndef GPU_WRAPPER_FENCE_H_
#define GPU_WRAPPER_FENCE_H_

#include "core/types/types.h"
#include "renderer/types/types.h"

namespace gpu
{
	class GraphicsContext;
	class ComputeContext;
	class Device;

	class Fence
	{
	public:
		RENDER_API int			create ( Device* device );
		RENDER_API int			destroy ( Device* device );

		//	signaled() : has this fence been signaled?
		// Note this will return true in any invalid state.
		// Signaling is done by the GPU, a signal command is added to a queue via ctx->signal(...)
		RENDER_API bool			signaled ( void );

		//	init() : resets this fence to the initial state
		// Before a fence is reused, this must be called.
		RENDER_API int			init ( void );

		//	valid() : is this fence valid to be used?
		// If the buffer has not been created, it will be removed.
		RENDER_API bool			valid ( void );
		//	nativePtr() : returns native index or pointer to the resource.
		RENDER_API gpuHandle	nativePtr ( void );

	private:
		friend GraphicsContext;
		friend ComputeContext;

		void*				m_syncId;
		GraphicsContext*	m_gfxc;
		ComputeContext*		m_compute;
	};
}

#endif//GPU_WRAPPER_FENCE_H_