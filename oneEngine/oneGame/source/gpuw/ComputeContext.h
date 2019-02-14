#ifndef GPU_WRAPPER_COMPUTE_CONTEXT_H_
#define GPU_WRAPPER_COMPUTE_CONTEXT_H_

namespace gpu
{
	class Fence;

	class ComputeContext
	{
	public:
		GPUW_API int			dispatch ( void );

		GPUW_API int			signal ( Fence* fence );
		GPUW_API int			waitOnSignal ( Fence* fence );
	};
}

#endif//GPU_WRAPPER_COMPUTE_CONTEXT_H_