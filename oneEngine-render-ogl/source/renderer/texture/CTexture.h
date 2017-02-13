
#ifndef _C_TEXTURE_H_
#define _C_TEXTURE_H_

// ==Includes==
// Memory management
#include "core/types/arBaseObject.h"
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
#define TextureType(a) public: RENDER_API virtual eTextureClass ClassType ( void ) { return a; };

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
class CTexture : public arBaseObject
{
	//TextureType( "TextureBase" );
	TextureType( TextureClassBase );
public:
	//	Constructor.
	// Pass in an empty string for the filename to just initialize a texture instance without any data.
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

	RENDER_API void GenerateMipmap ( eMipmapGenerationStyle generationStyle = MipmapNormal );

	RENDER_API void Upload (
		pixel_t* data,
		uint width,
		uint height,
		eWrappingType	repeatX			= Repeat,
		eWrappingType	repeatY			= Repeat,
		eMipmapGenerationStyle	mipmapGeneration = MipmapNormal,
		eSamplingFilter	filter			= SamplingLinear
		);
protected:
	friend CTextureMaster;
	friend CResourceManager;

	string			sFilename;	
	tTextureInfo	info;
	tTextureState	state;

	pixel_t	*		pData;
	
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