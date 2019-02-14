#ifndef GPU_WRAPPER_INTERNAL_ENUMS_H_
#define GPU_WRAPPER_INTERNAL_ENUMS_H_

#include "core/types/types.h"
#include "core/exceptions.h"
#include "core/gfx/textureFormats.h"
#include "renderer/types/types.h"
#include "gpuw/Public/Enums.h"
#include "gpuw/Public/Formats.h"
#include "gpuw/Public/ShaderTypes.h"

#include "../ogl/GLCommon.h"

namespace gpu {
namespace internal {

	static FORCE_INLINE gpuEnum ArEnumToGL ( const core::gfx::tex::arTextureType format )
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
	
	static FORCE_INLINE gpuEnum ArEnumToGL ( const core::gfx::tex::arColorFormat format )
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

			// Packed Depth+Stencil formats:
		case kDepthFormat32FStencil8:	return GL_DEPTH32F_STENCIL8;
		case kDepthFormat24Stencil8:	return GL_DEPTH24_STENCIL8;
		}
		throw core::InvalidArgumentException();
		return GL_INVALID_ENUM;
	}

	static FORCE_INLINE gpuEnum ArColorFormatToGlComponentType ( const core::gfx::tex::arColorFormat format )
	{
		using namespace core::gfx::tex;
		switch ( format ) {
			// Color formats:
		case kColorFormatRGB8:
		case kColorFormatRGB16:
		case kColorFormatRGB16F:
		case kColorFormatRGB32:
		case kColorFormatRGB32F:	return GL_RGB;

		case kColorFormatRGBA8:
		case kColorFormatRGBA16:
		case kColorFormatRGBA16F:
		case kColorFormatRGBA32:
		case kColorFormatRGBA32F:	return GL_RGBA;

		case kColorFormatRG8:
		case kColorFormatRG16:
		case kColorFormatRG16F:
		case kColorFormatRG32:
		case kColorFormatRG32F:		return GL_RG;

		case kColorFormatR8:
		case kColorFormatR16:
		case kColorFormatR16F:
		case kColorFormatR32:
		case kColorFormatR32F:		return GL_RED;

			// Depth formats:
		case kDepthFormat16:
		case kDepthFormat24:
		case kDepthFormat32:
		case kDepthFormat32F:		return GL_DEPTH_COMPONENT;

			// Stencil formats:
		case KStencilFormatIndex1:
		case KStencilFormatIndex4:
		case KStencilFormatIndex8:
		case KStencilFormatIndex16:	return GL_STENCIL_INDEX;

			// Packed depth+stencil formats
		case kDepthFormat24Stencil8:
		case kDepthFormat32FStencil8: return GL_DEPTH_COMPONENT; // Default, return the depth when used as a texture.
		}
		throw core::InvalidArgumentException();
		return GL_INVALID_ENUM;
	}

	static FORCE_INLINE gpuEnum ArColorFormatToGlDataType ( const core::gfx::tex::arColorFormat format )
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

			// Packed depth+stencil formats
		case kDepthFormat24Stencil8:	return GL_UNSIGNED_INT; // Treat as depth target
		case kDepthFormat32FStencil8:	return GL_FLOAT; // Treat as depth target
		}
		throw core::InvalidArgumentException();
		return GL_INVALID_ENUM;
	}

	static FORCE_INLINE gpuEnum ArFormatToGlDataType ( const gpu::Format format )
	{
		switch ( format ) {
			// Byte formats
		case kFormatR8UInteger:
		case kFormatR8G8UInteger:
		case kFormatR8G8B8UInteger:
		case kFormatR8G8B8A8UInteger:		return GL_UNSIGNED_BYTE;
			// Short formats
		case kFormatR16UInteger:
		case kFormatR16G16UInteger:
		case kFormatR16G16B16UInteger:
		case kFormatR16G16B16A16UInteger:	return GL_UNSIGNED_SHORT;
			// Int formats
		case kFormatR32UInteger:			return GL_UNSIGNED_INT;
			// Float formats
		case kFormatR32SFloat:
		case kFormatR32G32SFloat:
		case kFormatR32G32B32SFloat:
		case kFormatR32G32B32A32SFloat:		return GL_FLOAT;
		}
		throw core::InvalidArgumentException();
		return GL_INVALID_ENUM;
	}

	static FORCE_INLINE gpuEnum ArEnumToGL ( const gpu::PrimitiveTopology type )
	{
		switch ( type ) {
			// Points
		case kPrimitiveTopologyPointList:			return GL_POINTS;
			// Lines
		case kPrimitiveTopologyLineList:			return GL_LINES;
		case kPrimitiveTopologyLineStrip:			return GL_LINE_STRIP;
			// Triangles
		case kPrimitiveTopologyTriangleList:		return GL_TRIANGLES;
		case kPrimitiveTopologyTriangleStrip:		return GL_TRIANGLE_STRIP;
		case kPrimitiveTopologyTriangleFan:			return GL_TRIANGLE_FAN;
			// GS types
		case kPrimitiveTopologyLineListAdjacency:	return GL_LINES_ADJACENCY;
		case kPrimitiveTopologyLineStripAdjacency:	return GL_LINE_STRIP_ADJACENCY;
		case kPrimitiveTopologyTriangleListAdjacency:
													return GL_LINE_STRIP_ADJACENCY;
		case kPrimitiveTopologyTriangleStripAdjacency:
													return GL_LINE_STRIP_ADJACENCY;
			// HS types
		case kPrimitiveTopologyLinePatchList:		return GL_PATCHES;
		}
		return GL_INVALID_ENUM;
	}

	static FORCE_INLINE gpuEnum ArEnumToGL ( const gpu::IndexFormat format )
	{
		switch ( format ) {
		case kIndexFormatUnsigned16:	return GL_UNSIGNED_SHORT;
		case kIndexFormatUnsigned32:	return GL_UNSIGNED_INT;
		}
		return GL_INVALID_ENUM;
	}

	static FORCE_INLINE gpuEnum ArEnumToGL ( const gpu::ShaderStage stage )
	{
		switch ( stage ) {
			// Normal GFX Pipeline
		case kShaderStageVs:	return GL_VERTEX_SHADER;
		case kShaderStageHs:	return GL_TESS_CONTROL_SHADER;
		case kShaderStageDs:	return GL_TESS_EVALUATION_SHADER;
		case kShaderStageGs:	return GL_GEOMETRY_SHADER;
		case kShaderStagePs:	return GL_FRAGMENT_SHADER;
			// Vega+ pipeline todo???
		// none
			// Compute
		case kShaderStageCs:	return GL_COMPUTE_SHADER;
		}
		return GL_INVALID_ENUM;
	}
}}

#endif//GPU_WRAPPER_INTERNAL_ENUMS_H_
