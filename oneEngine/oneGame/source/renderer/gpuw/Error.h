#ifndef GPU_WRAPPER_ERROR_H_
#define GPU_WRAPPER_ERROR_H_

#include <stdint.h>

namespace gpu
{
	enum ErrorCode : uint8_t
	{
		kError_SUCCESS = 0,

		kErrorOutOfMemory,
		kErrorNullReference,
		kErrorBadArgument,
		kErrorInvalidDevice,
		kErrorInvalidModule,
		kErrorModuleUnsupported,
		kErrorFormatUnsupported,
		kErrorDeviceError,
	};

};

#endif//GPU_WRAPPER_ERROR_H_