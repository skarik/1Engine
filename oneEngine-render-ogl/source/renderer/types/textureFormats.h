#ifndef _RENDERER_TEXTURE_FORMATS_H_
#define _RENDERER_TEXTURE_FORMATS_H_

#include "core/types/types.h"

// ===ENUMERATIONS===

// Enumerations for the texture type
enum eTextureType : uint8_t
{
	/*
	Texture2D = GL_TEXTURE_2D,
	//Texture1Darray = GL_TEXTURE_1D_ARRAY,
	Texture3D = GL_TEXTURE_3D,
	//Texture2Darray = GL_TEXTURE_2D_ARRAY,
	TextureCube = GL_TEXTURE_CUBE_MAP
	*/
	Texture2D,
	Texture3D,
	TextureCube
};

// Enumerations for the internal format
enum eColorFormat : uint8_t
{
	ColorNone = 0,

	__COLOR_FORMAT_RGB_MIN,
	RGB8,
	RGB16,
	RGB16F,
	__COLOR_FORMAT_RGB_MAX,

	__COLOR_FORMAT_RGBA_MIN,
	RGBA8,
	RGBA16F,
	RGBA16,
	__COLOR_FORMAT_RGBA_MAX,

	__COLOR_FORMAT_MAX
};
// Enumerations for the depth type
enum eDepthFormat
{
	DepthNone = __COLOR_FORMAT_MAX,

	Depth16,
	Depth24,
	Depth32,

	__DEPTH_FORMAT_MAX
};
// Enumerations for the stencil type
enum eStencilFormat : uint8_t
{
	StencilNone = __DEPTH_FORMAT_MAX,

	// Unsupported on textures
	StencilIndex1,
	// Unsupported on textures
	StencilIndex4,
	// This is the only supported value on textures
	StencilIndex8,
	// Unsupported on textures
	StencilIndex16,

	__STENCIL_FORMAT_MAX
};

// Enumerations for the texture wrap
enum eWrappingType : uint8_t
{
	/*
	Repeat			= GL_REPEAT,
	Clamp			= GL_CLAMP,
	MirroredRepeat	= GL_MIRRORED_REPEAT
	*/
	Repeat,
	Clamp,
	MirroredRepeat
};
// Enumerations for import options
enum eOrientation : uint8_t
{
	NoFlip,
	Flip
};
// Enumerations for mipmap generation options
enum eMipmapGenerationStyle : uint8_t
{
	MipmapNormal = 0,
	MipmapNone,
	MipmapNearest
};
// Enumeration for the texture class
enum eTextureClass : uint8_t
{
	TextureClassBase,
	TextureClass3D,
	TextureClassBitmapFont,
	TextureClassCube,
	TextureClassRenderTarget,
	TextureClassRenderTarget_Cube,
	TextureClassRenderTarget_MRT,
};
// Enumeration for texture sampling
enum eSamplingFilter : uint8_t
{
	SamplingPoint = 0,
	SamplingLinear,
};

#endif//_RENDERER_TEXTURE_FORMATS_H_