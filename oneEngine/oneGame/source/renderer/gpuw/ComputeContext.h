#ifndef GPU_WRAPPER_COMPUTE_CONTEXT_H_
#define GPU_WRAPPER_COMPUTE_CONTEXT_H_

namespace gpu
{
	class Fence;

	class ComputeContext
	{
	public:
		RENDER_API int			dispatch ( void );

		RENDER_API int			signal ( Fence* fence );
		RENDER_API int			waitOnSignal ( Fence* fence );
	};
}

#endif//GPU_WRAPPER_COMPUTE_CONTEXT_H_