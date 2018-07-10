
#ifndef _C_TEXTURE_CUBE_H_
#define _C_TEXTURE_CUBE_H_

// ==Includes==
#include "RrTexture.h"

// ===CLASS===
class RrTextureCube : public RrTexture
{
	TextureType( TextureClassCube );
public:
	explicit RrTextureCube ( const string &sInIdentifier,
		const string& sInFilenameXPos, const string& sInFilenameXNeg,
		const string& sInFilenameYPos, const string& sInFilenameYNeg,
		const string& sInFilenameZPos, const string& sInFilenameZNeg,
		eTextureType	textureType		= TextureCube,
		eColorFormat	format			= RGBA8,
		eMipmapGenerationStyle	mipmapGeneration = MipmapNone
		);
	~RrTextureCube ( void );

	//void GenerateMipmap3D ( eMipmapGenerationStyle generationStyle=MipmapNone );
protected:
	//void Load3DImageInfoFromAtlas ( const unsigned int atlasSizeX, const unsigned int atlasSizeY );
};


#endif//_C_TEXTURE_CUBE_H_