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

	uint32_t FormatComponentCount ( const gpu::Format format )
	{
		switch (format)
		{
		case kFormatR8UInteger:
		case kFormatR16UInteger:
		case kFormatR32SFloat:			return 1;

		case kFormatR8G8UInteger:
		case kFormatR16G16UInteger:
		case kFormatR32G32SFloat:		return 2;

		case kFormatR8G8B8UInteger:
		case kFormatR16G16B16UInteger:
		case kFormatR32G32B32SFloat:	return 3;

		case kFormatR8G8B8A8UInteger:
		case kFormatR16G16B16A16UInteger:
		case kFormatR32G32B32A32SFloat:	return 4;
		}
		return 0;
	}

	intptr_t FormatByteWidth ( const gpu::Format format );
}

#endif//GPU_WRAPPER_PUBLIC_FORMATS_H_