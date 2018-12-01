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
		kFormatR32UInteger,
		kFormatR32SFloat,
		kFormatR32G32SFloat,
		kFormatR32G32B32SFloat,
		kFormatR32G32B32A32SFloat,
	};

	static FORCE_INLINE uint32_t FormatComponentCount ( const gpu::Format format )
	{
		switch (format)
		{
		case kFormatR8UInteger:
		case kFormatR16UInteger:
		case kFormatR32UInteger:
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

	static FORCE_INLINE intptr_t FormatGetByteStride ( const gpu::Format format )
	{
		switch (format)
		{
		case kFormatR8UInteger:			return 1;
		
		case kFormatR8G8UInteger:
		case kFormatR16UInteger:		return 2;

		case kFormatR8G8B8UInteger:		return 3;

		case kFormatR8G8B8A8UInteger:
		case kFormatR16G16UInteger:
		case kFormatR32UInteger:
		case kFormatR32SFloat:			return 4;

		case kFormatR16G16B16UInteger:	return 6;

		case kFormatR16G16B16A16UInteger:
		case kFormatR32G32SFloat:		return 8;

		case kFormatR32G32B32SFloat:	return 12;

		case kFormatR32G32B32A32SFloat:	return 16;
		}
		return 0;
	}
}

#endif//GPU_WRAPPER_PUBLIC_FORMATS_H_