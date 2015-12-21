
#ifndef _C_TEXTURE_H_
#define _C_TEXTURE_H_

// ==Includes==
// OpenGL interface and game link
//#include "renderer/system/glMainSystem.h"
#include "renderer/types/types.h"
#include "renderer/types/textureFormats.h"
#include "renderer/types/pixelFormat.h"
// Standard libaries
#include <stdio.h>
#include <string>

// Using the string
using std::string;

// ==Defines==
// A define to allow for a function to differentiate between different kinds of textures in the texture master class
//#define TextureType(a) public: virtual string GetTypeName ( void ) { return string(a); };
#define TextureType(a) public: RENDER_API virtual eTextureClass ClassType ( void ) { return a; };
/*
// ===ENUMERATIONS===
// Enumerations for the internal format
enum eInternalFormat
{
	RGB8 = GL_RGB8,
	RGBA8 = GL_RGBA8,
	RGB16 = GL_RGB16,
	RGB16F = GL_RGB16F,
	RGBA16F = GL_RGBA16F,
	RGBA16 = GL_RGBA16,
	RGBc = GL_COMPRESSED_RGB,
    RGBAc = GL_COMPRESSED_RGBA
};
// Enumerations for the texture type
enum eTextureType
{
	Texture2D = GL_TEXTURE_2D,
	//Texture1Darray = GL_TEXTURE_1D_ARRAY,
	Texture3D = GL_TEXTURE_3D,
	//Texture2Darray = GL_TEXTURE_2D_ARRAY,
	TextureCube = GL_TEXTURE_CUBE_MAP
};
// Enumerations for the depth type
enum eDepthFormat
{
	DepthNone = 0,
	Depth16 = GL_DEPTH_COMPONENT16,
	Depth24 = GL_DEPTH_COMPONENT24,
	Depth32 = GL_DEPTH_COMPONENT32
};
// Enumerations for the stencil type
enum eStencilFormat
{
	StencilNone = 0,
	StencilIndex1 = GL_STENCIL_INDEX1,
	StencilIndex4 = GL_STENCIL_INDEX4,
	StencilIndex8 = GL_STENCIL_INDEX8,
	StencilIndex16 = GL_STENCIL_INDEX16
};
// Enumerations for the texture wrap
enum eWrappingType
{
	Repeat			= GL_REPEAT,
	Clamp			= GL_CLAMP,
	MirroredRepeat	= GL_MIRRORED_REPEAT
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
*/

//===============================================================================================//
// Texture info structures
//===============================================================================================//

// Texture info struct
struct tTextureInfo
{	
	glHandle		index;

	eTextureType	type;
	eInternalFormat internalFormat;
	eMipmapGenerationStyle	mipmapStyle;
	eSamplingFilter	filter;

	unsigned int width;
	unsigned int height;
	unsigned int depth;
	unsigned int levels;	// mipmap levels

	eWrappingType repeatX;
	eWrappingType repeatY;
	eWrappingType repeatZ;

	// TODO: Implement:
	eOrientation flipX;
	eOrientation flipY;

	unsigned int userdata;
	void*		userpdata;

	tTextureInfo ( void )
		: index(0)
	{
		;
	}
};
// Texture state struct
struct tTextureState
{
	unsigned int level_base;
	unsigned int level_max;
};

class CTextureMaster;
class CResourceManager;

// ===CLASS===
// Class definition
class CTexture
{
	//TextureType( "TextureBase" );
	TextureType( TextureClassBase );
public:
	//CTexture ( void );
	RENDER_API explicit CTexture ( string sInFilename,
		eTextureType	textureType		= Texture2D,
		eInternalFormat	format			= RGBA8,
		unsigned int	maxTextureWidth	= 1024,
		unsigned int	maxTextureHeight= 1024,
		eWrappingType	repeatX			= Repeat,
		eWrappingType	repeatY			= Repeat,
		eMipmapGenerationStyle	mipmapGeneration = MipmapNormal,
		eSamplingFilter	filter			= SamplingLinear
		);
	RENDER_API ~CTexture ( void );

	RENDER_API void Bind ( void );
	RENDER_API void Unbind ( void );
	RENDER_API static void Unbind ( char );

	RENDER_API unsigned int GetWidth ( void ) { return info.width; };
	RENDER_API unsigned int GetHeight( void ) { return info.height; };
	RENDER_API unsigned int GetDepth ( void ) { return info.depth; };

	RENDER_API eTextureType GetType ( void ) { return info.type; };

	RENDER_API virtual glHandle GetColorSampler ( void ) {
		return info.index;
	}
	RENDER_API virtual eTextureType GetSamplerTarget ( void ) {
		return info.type;
	}
	RENDER_API virtual bool GetIsFont ( void ) {
		return false;
	}

	RENDER_API virtual void Reload ( void );

	RENDER_API void GenerateMipmap ( eMipmapGenerationStyle generationStyle=MipmapNormal );
protected:
	friend CTextureMaster;
	friend CResourceManager;

	string			sFilename;	
	tTextureInfo	info;
	tTextureState	state;

	tPixel	*		pData;
	
protected:
	RENDER_API void LoadImageInfo ( void );
	/*
	void loadTGA ( void );
	void loadPNG ( void );
	void loadJPG ( void );

	void loadDefault ( void );
	*/
};



#endif