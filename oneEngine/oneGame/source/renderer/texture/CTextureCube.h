
#ifndef _C_TEXTURE_CUBE_H_
#define _C_TEXTURE_CUBE_H_

// ==Includes==
#include "CTexture.h"

// ===CLASS===
class CTextureCube : public CTexture
{
	TextureType( TextureClassCube );
public:
	explicit CTextureCube ( const string &sInIdentifier,
		const string& sInFilenameXPos, const string& sInFilenameXNeg,
		const string& sInFilenameYPos, const string& sInFilenameYNeg,
		const string& sInFilenameZPos, const string& sInFilenameZNeg,
		eTextureType	textureType		= TextureCube,
		eColorFormat	format			= RGBA8,
		eMipmapGenerationStyle	mipmapGeneration = MipmapNone
		);
	~CTextureCube ( void );

	//void GenerateMipmap3D ( eMipmapGenerationStyle generationStyle=MipmapNone );
protected:
	//void Load3DImageInfoFromAtlas ( const unsigned int atlasSizeX, const unsigned int atlasSizeY );
};


#endif//_C_TEXTURE_CUBE_H_