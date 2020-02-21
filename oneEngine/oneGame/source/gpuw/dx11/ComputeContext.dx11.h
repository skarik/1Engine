#ifndef GPU_WRAPPER_COMPUTE_CONTEXT_H_
#define GPU_WRAPPER_COMPUTE_CONTEXT_H_

#include "core/types.h"

namespace gpu
{
	class Fence;

	class ComputeContext
	{
	public:
		GPUW_EXLUSIVE_API explicit
								ComputeContext ( void* wrapperDevice );
		GPUW_EXLUSIVE_API 		~ComputeContext ( void );

		//	reset() : Resets the context state to defaults.
		// Note that on some API's, this will do nothing.
		GPUW_API int			reset ( void );

		//	submit() : Submits the current graphics commands in the queue.
		GPUW_API int			submit ( void );

		//	validate() : Checks to make sure submitted graphics commands are valid.
		// Returns gpu::kError_SUCCESS on no error.
		GPUW_API int			validate ( void );

		GPUW_API int			dispatch ( void );

		GPUW_API int			signal ( Fence* fence );
		GPUW_API int			waitOnSignal ( Fence* fence );

	private:
		// implementation details:

		void*					m_wrapperDevice;
		void*					m_deferredContext;

	};
}

#endif//GPU_WRAPPER_COMPUTE_CONTEXT_H_