#ifndef GPU_WRAPPER_PUBLIC_FORMATS_H_
#define GPU_WRAPPER_PUBLIC_FORMATS_H_

#include "core/types.h"
#include <stdint.h>

namespace gpu
{
	enum IndexFormat
	{
		kIndexFormatUnsigned16,
		kIndexFormatUnsigned32,
	};

	enum Format
	{
		kFormatUndefined,
		kFormatR8UInteger,
		kFormatR8G8UInteger,
		kFormatR8G8B8UInteger,
		kFormatR8G8B8A8UInteger,
		kFormatR16UInteger,
		kFormatR16G16UInteger,
		kFormatR16G16B16UInteger,
		kFormatR16G16B16A16UInteger,
		kFormatR32SFloat,
		kFormatR32G32SFloat,
		kFormatR32G32B32SFloat,
		kFormatR32G32B32A32SFloat,
	};
}

#endif//GPU_WRAPPER_PUBLIC_FORMATS_H_