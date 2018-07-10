
#ifndef _C_TEXTURE_3D_H_
#define _C_TEXTURE_3D_H_

// ==Includes==
#include "RrTexture.h"

// ===CLASS===
class RrTexture3D : public RrTexture
{
	TextureType( TextureClass3D );
public:
	RENDER_API explicit RrTexture3D ( string sInFilename,
		eTextureType	textureType		= Texture3D,
		eColorFormat	format			= RGBA8,
		unsigned int	atlasSizeX		= 4,
		unsigned int	atlasSizeY		= 4,
		eWrappingType	repeatX			= Repeat,
		eWrappingType	repeatY			= Repeat,
		eWrappingType	repeatZ			= Repeat,
		eMipmapGenerationStyle	mipmapGeneration = MipmapNone
		);
	RENDER_API ~RrTexture3D ( void );

	RENDER_API void GenerateMipmap3D ( eMipmapGenerationStyle generationStyle=MipmapNone );

	RENDER_API void Upload (
		pixel_t* data,
		uint width,
		uint height,
		uint depth,
		eWrappingType	repeatX			= Repeat,
		eWrappingType	repeatY			= Repeat,
		eWrappingType	repeatZ			= Repeat,
		eMipmapGenerationStyle	mipmapGeneration = MipmapNormal,
		eSamplingFilter	filter			= SamplingLinear
	);
protected:
	void Load3DImageInfoFromAtlas ( const unsigned int atlasSizeX, const unsigned int atlasSizeY );
};

#endif//_C_TEXTURE_3D_H_