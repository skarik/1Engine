#ifndef GPU_WRAPPER_BASE_CONTEXT_H_
#define GPU_WRAPPER_BASE_CONTEXT_H_

#include "core/types.h"
#include <stdint.h>

namespace gpu {
namespace base
{
	class BaseContext
	{
	public:
		GPUW_EXLUSIVE_API virtual void*
							getNativeContext ( void ) =0;
	};
}

#endif//GPU_WRAPPER_BASE_CONTEXT_H_