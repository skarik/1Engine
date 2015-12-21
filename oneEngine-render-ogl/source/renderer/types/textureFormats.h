
#ifndef _RENDERER_TEXTURE_FORMATS_H_
#define _RENDERER_TEXTURE_FORMATS_H_

// ===ENUMERATIONS===
// Enumerations for the internal format
enum eInternalFormat
{
	/*	
	RGB8 = GL_RGB8,
	RGBA8 = GL_RGBA8,
	RGB16 = GL_RGB16,
	RGB16F = GL_RGB16F,
	RGBA16F = GL_RGBA16F,
	RGBA16 = GL_RGBA16,
	RGBc = GL_COMPRESSED_RGB,
    RGBAc = GL_COMPRESSED_RGBA
	*/
	RGB8,
	RGBA8,
	RGB16,
	RGB16F,
	RGBA16F,
	RGBA16,
	RGBc,
    RGBAc
};
// Enumerations for the texture type
enum eTextureType
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
// Enumerations for the depth type
enum eDepthFormat
{
	/*
	DepthNone = 0,
	Depth16 = GL_DEPTH_COMPONENT16,
	Depth24 = GL_DEPTH_COMPONENT24,
	Depth32 = GL_DEPTH_COMPONENT32
	*/
	DepthNone = 0,
	Depth16,
	Depth24,
	Depth32
};
// Enumerations for the stencil type
enum eStencilFormat
{
	/*
	StencilNone = 0,
	StencilIndex1 = GL_STENCIL_INDEX1,
	StencilIndex4 = GL_STENCIL_INDEX4,
	StencilIndex8 = GL_STENCIL_INDEX8,
	StencilIndex16 = GL_STENCIL_INDEX16
	*/
	StencilNone = 0,
	StencilIndex1,
	StencilIndex4,
	StencilIndex8,
	StencilIndex16
};
// Enumerations for the texture wrap
enum eWrappingType
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
enum eOrientation
{
	NoFlip,
	Flip
};
// Enumerations for mipmap generation options
enum eMipmapGenerationStyle
{
	MipmapNormal = 0,
	MipmapNone,
	MipmapNearest
};
// Enumeration for the texture class
enum eTextureClass
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
enum eSamplingFilter
{
	SamplingPoint = 0,
	SamplingLinear,
};

#endif//_RENDERER_TEXTURE_FORMATS_H_