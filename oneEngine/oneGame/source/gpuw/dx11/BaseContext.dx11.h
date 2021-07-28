#ifndef GPU_WRAPPER_BASE_CONTEXT_H_
#define GPU_WRAPPER_BASE_CONTEXT_H_

#include "core/types.h"
#include <stdint.h>

namespace gpu
{
	class BaseContext
	{
	public:
		GPUW_EXLUSIVE_API void*	getNativeContext ( void )
			{ return m_deferredContext; }

	protected:
		void*					m_deferredContext;
	};
}

#endif//GPU_WRAPPER_BASE_CONTEXT_H_