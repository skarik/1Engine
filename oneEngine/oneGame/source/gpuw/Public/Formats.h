#ifndef GPU_WRAPPER_PUBLIC_FORMATS_H_
#define GPU_WRAPPER_PUBLIC_FORMATS_H_

#include "core/types.h"
#include "core/gfx/textureFormats.h"
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
		kFormatR8G8B8A8UNorm,
		kFormatR8G8B8A8UInteger,
		kFormatR16UInteger,
		kFormatR16G16UInteger,
		kFormatR16G16B16UInteger,
		kFormatR16G16B16A16UNorm,
		kFormatR16G16B16A16UInteger,
		kFormatR32UInteger,
		kFormatR32SFloat,
		kFormatR32G32UInteger,
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
		case kFormatR32G32UInteger:
		case kFormatR32G32SFloat:		return 2;

		case kFormatR8G8B8UInteger:
		case kFormatR16G16B16UInteger:
		case kFormatR32G32B32SFloat:	return 3;

		case kFormatR8G8B8A8UNorm:
		case kFormatR8G8B8A8UInteger:
		case kFormatR16G16B16A16UNorm:
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

		case kFormatR8G8B8A8UNorm:
		case kFormatR8G8B8A8UInteger:
		case kFormatR16G16UInteger:
		case kFormatR32UInteger:
		case kFormatR32SFloat:			return 4;

		case kFormatR16G16B16UInteger:	return 6;

		case kFormatR16G16B16A16UNorm:
		case kFormatR16G16B16A16UInteger:
		case kFormatR32G32UInteger:
		case kFormatR32G32SFloat:		return 8;

		case kFormatR32G32B32SFloat:	return 12;

		case kFormatR32G32B32A32SFloat:	return 16;
		}
		return 0;
	}

	static FORCE_INLINE Format ArFormatToGPUFormat ( const core::gfx::tex::arColorFormat format )
	{
		using namespace core::gfx::tex;
		switch (format)
		{
		case kColorFormatRGB8:		return kFormatR8G8B8UInteger;
		case kColorFormatRGB16:		return kFormatR16G16B16UInteger;
		//case kColorFormatRGB16F:
		//case kColorFormatRGB32:
		case kColorFormatRGB32F:	return kFormatR32G32B32SFloat;

		case kColorFormatRGBA8:		return kFormatR8G8B8A8UNorm;
		case kColorFormatRGBA8UI:	return kFormatR8G8B8A8UInteger;
		case kColorFormatRGBA16:	return kFormatR16G16B16A16UNorm;
		case kColorFormatRGBA16UI:	return kFormatR16G16B16A16UInteger;
		//case kColorFormatRGBA16F:
		//case kColorFormatRGBA32:
		case kColorFormatRGBA32F:	return kFormatR32G32B32A32SFloat;

		case kColorFormatRG8:		return kFormatR8G8UInteger;
		case kColorFormatRG16:		return kFormatR16G16UInteger;
		//case kColorFormatRG16F:
		//case kColorFormatRG32:
		case kColorFormatRG32UI:	return kFormatR32G32UInteger;
		case kColorFormatRG32F:		return kFormatR32G32SFloat;

		case KStencilFormatIndex8:
		case kColorFormatR8:		return kFormatR8UInteger;
		case kDepthFormat16:
		case kColorFormatR16:		return kFormatR16UInteger;
		//case kColorFormatR16F:
		case kDepthFormat32:
		case kColorFormatR32:		return kFormatR32UInteger;
		case kDepthFormat32F:
		case kColorFormatR32F:		return kFormatR32SFloat;

		//case KStencilFormatIndex1:
		//case KStencilFormatIndex4:
		//case KStencilFormatIndex16:
		//case kDepthFormat32FStencil8:
		//case kDepthFormat24Stencil8:
		}
		return kFormatUndefined;
	}
}

#endif//GPU_WRAPPER_PUBLIC_FORMATS_H_