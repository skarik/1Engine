#ifndef GPU_WRAPPER_COMPUTE_CONTEXT_H_
#define GPU_WRAPPER_COMPUTE_CONTEXT_H_

namespace gpu
{
	class Fence;

	class ComputeContext
	{
	public:
		RENDER_API int			dispatch ( void );

		RENDER_API int			sync ( Fence* fence );
	};
}

#endif//GPU_WRAPPER_COMPUTE_CONTEXT_H_