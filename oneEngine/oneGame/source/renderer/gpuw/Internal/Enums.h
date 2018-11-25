#ifndef GPU_WRAPPER_INTERNAL_ENUMS_H_
#define GPU_WRAPPER_INTERNAL_ENUMS_H_

#include "core/types/types.h"
#include "renderer/types/types.h"
#include "core/gfx/textureFormats.h"
#include "core/exceptions.h"

#include "renderer/ogl/GLCommon.h"

namespace gpu {
namespace internal {

	gpuEnum ArEnumToGL ( const core::gfx::tex::arTextureType format )
	{
		using namespace core::gfx::tex;
		switch ( format ) {
		case kTextureType1D:		return GL_TEXTURE_1D;
		case kTextureType2D:		return GL_TEXTURE_2D;
		case kTextureType3D:		return GL_TEXTURE_3D;
		case kTextureTypeCube:		return GL_TEXTURE_CUBE_MAP;
		case kTextureType1DArray:	return GL_TEXTURE_1D_ARRAY;
		case kTextureType2DArray:	return GL_TEXTURE_2D_ARRAY;
		case kTextureTypeCubeArray:	return GL_TEXTURE_CUBE_MAP_ARRAY;
		}
		throw core::InvalidArgumentException();
		return GL_INVALID_ENUM;
	}
	
	gpuEnum ArEnumToGL ( const core::gfx::tex::arColorFormat format )
	{
		using namespace core::gfx::tex;
		switch ( format ) {
			// Color formats:
		case kColorFormatRGB8:		return GL_RGB8;
		case kColorFormatRGB16:		return GL_RGB16;
		case kColorFormatRGB16F:	return GL_RGB16F;
		case kColorFormatRGB32:		return GL_RGB32UI;
		case kColorFormatRGB32F:	return GL_RGB32F;

		case kColorFormatRGBA8:		return GL_RGBA8;
		case kColorFormatRGBA16:	return GL_RGBA16;
		case kColorFormatRGBA16F:	return GL_RGBA16F;
		case kColorFormatRGBA32:	return GL_RGBA32UI;
		case kColorFormatRGBA32F:	return GL_RGBA32F;

		case kColorFormatRG8:		return GL_RG8;
		case kColorFormatRG16:		return GL_RG16;
		case kColorFormatRG16F:		return GL_RG16F;
		case kColorFormatRG32:		return GL_RG32UI;
		case kColorFormatRG32F:		return GL_RG32F;

		case kColorFormatR8:		return GL_R8;
		case kColorFormatR16:		return GL_R16;
		case kColorFormatR16F:		return GL_R16F;
		case kColorFormatR32:		return GL_R32I;
		case kColorFormatR32F:		return GL_R32F;

			// Depth formats:
		case kDepthFormat16:		return GL_DEPTH_COMPONENT16;
		case kDepthFormat24:		return GL_DEPTH_COMPONENT24;
		case kDepthFormat32:		return GL_DEPTH_COMPONENT32;
		case kDepthFormat32F:		return GL_DEPTH_COMPONENT32F;

			// Stencil formats:
		case KStencilFormatIndex1:	return GL_STENCIL_INDEX1;
		case KStencilFormatIndex4:	return GL_STENCIL_INDEX4;
		case KStencilFormatIndex8:	return GL_STENCIL_INDEX8;
		case KStencilFormatIndex16:	return GL_STENCIL_INDEX16;
		}
		throw core::InvalidArgumentException();
		return GL_INVALID_ENUM;
	}

	gpuEnum ArColorFormatToGlDataType ( const core::gfx::tex::arColorFormat format )
	{
		using namespace core::gfx::tex;
		switch ( format ) {
			// Color formats:
		case kColorFormatRGB8:		return GL_UNSIGNED_BYTE;
		case kColorFormatRGB16:		return GL_UNSIGNED_SHORT;
		case kColorFormatRGB16F:	return GL_HALF_FLOAT;
		case kColorFormatRGB32:		return GL_UNSIGNED_INT;
		case kColorFormatRGB32F:	return GL_FLOAT;

		case kColorFormatRGBA8:		return GL_UNSIGNED_BYTE;
		case kColorFormatRGBA16:	return GL_UNSIGNED_SHORT;
		case kColorFormatRGBA16F:	return GL_HALF_FLOAT;
		case kColorFormatRGBA32:	return GL_UNSIGNED_INT;
		case kColorFormatRGBA32F:	return GL_FLOAT;

		case kColorFormatRG8:		return GL_UNSIGNED_BYTE;
		case kColorFormatRG16:		return GL_UNSIGNED_SHORT;
		case kColorFormatRG16F:		return GL_HALF_FLOAT;
		case kColorFormatRG32:		return GL_UNSIGNED_INT;
		case kColorFormatRG32F:		return GL_FLOAT;

		case kColorFormatR8:		return GL_UNSIGNED_BYTE;
		case kColorFormatR16:		return GL_UNSIGNED_SHORT;
		case kColorFormatR16F:		return GL_HALF_FLOAT;
		case kColorFormatR32:		return GL_UNSIGNED_INT;
		case kColorFormatR32F:		return GL_FLOAT;

			// Depth formats:
		case kDepthFormat16:		return GL_UNSIGNED_SHORT;
		case kDepthFormat24:		return GL_UNSIGNED_INT;
		case kDepthFormat32:		return GL_UNSIGNED_INT;
		case kDepthFormat32F:		return GL_FLOAT;

			// Stencil formats:
		case KStencilFormatIndex1:	return GL_UNSIGNED_BYTE;
		case KStencilFormatIndex4:	return GL_UNSIGNED_BYTE;
		case KStencilFormatIndex8:	return GL_UNSIGNED_BYTE;
		case KStencilFormatIndex16:	return GL_UNSIGNED_SHORT;
		}
		throw core::InvalidArgumentException();
		return GL_INVALID_ENUM;
	}

}}

#endif//GPU_WRAPPER_INTERNAL_ENUMS_H_
