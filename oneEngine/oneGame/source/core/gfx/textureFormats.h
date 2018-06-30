#ifndef CORE_GFX_TEXTURE_FORMATS_H_
#define CORE_GFX_TEXTURE_FORMATS_H_

#include "core/types/types.h"

namespace core {
namespace gfx {
namespace tex {

	// Enumerations for the texture type
	enum arTextureType : uint8_t
	{
		/*
		Texture2D = GL_TEXTURE_2D,
		//Texture1Darray = GL_TEXTURE_1D_ARRAY,
		Texture3D = GL_TEXTURE_3D,
		//Texture2Darray = GL_TEXTURE_2D_ARRAY,
		TextureCube = GL_TEXTURE_CUBE_MAP
		*/
		kTextureType1D,
		kTextureType2D,
		kTextureType3D,
		kTextureTypeCube,
	};

	// Enumerations for the internal format
	enum arColorFormat : uint8_t
	{
		kColorFormatNone = 0,

		__COLOR_FORMAT_RGB_MIN,
		kColorFormatRGB8,
		kColorFormatRGB16,
		kColorFormatRGB16F,
		__COLOR_FORMAT_RGB_MAX,

		__COLOR_FORMAT_RGBA_MIN,
		kColorFormatRGBA8,
		kColorFormatRGBA16F,
		kColorFormatRGBA16,
		__COLOR_FORMAT_RGBA_MAX,

		__COLOR_FORMAT_MAX
	};

	// Enumerations for the depth type
	enum arDepthFormat
	{
		kDepthFormatNone = __COLOR_FORMAT_MAX,

		kDepthFormat16,
		kDepthFormat24,
		kDepthFormat32,

		__DEPTH_FORMAT_MAX
	};

	// Enumerations for the stencil type
	enum arStencilFormat : uint8_t
	{
		KStencilFormatNone = __DEPTH_FORMAT_MAX,

		// Unsupported on textures
		KStencilFormatIndex1,
		// Unsupported on textures
		KStencilFormatIndex4,
		// This is the only supported value on textures
		KStencilFormatIndex8,
		// Unsupported on textures
		KStencilFormatIndex16,

		__STENCIL_FORMAT_MAX
	};

	// Enumerations for the texture wrap
	enum arWrappingType : uint8_t
	{
		/*
		Repeat			= GL_REPEAT,
		Clamp			= GL_CLAMP,
		MirroredRepeat	= GL_MIRRORED_REPEAT
		*/
		kWrappingRepeat,
		kWrappingClamp,
		kWrappingMirroredRepeat,
	};
	// Enumerations for import options
	enum arOrientation : uint8_t
	{
		kOrientationNoFlip,
		kOrientationFlip,
	};
	// Enumerations for mipmap generation options
	enum arMipmapGenerationStyle : uint8_t
	{
		kMipmapGenerationNormal = 0,
		kMipmapGenerationNone,
		kMipmapGenerationNearest
	};
	// Enumeration for the texture class
	enum arTextureClass : uint8_t
	{
		kTextureClassBase,
		kTextureClass3D,
		kTextureClassBitmapFont,
		kTextureClassCube,
		kTextureClassRenderTarget,
		kTextureClassRenderTarget_Cube,
		kTextureClassRenderTarget_MRT,
	};
	// Enumeration for texture sampling
	enum arSamplingFilter : uint8_t
	{
		kSamplingPoint = 0,
		kSamplingLinear,
		kSamplingAnisotropic,
	};

}}}

#endif//CORE_GFX_TEXTURE_FORMATS_H_