
#ifndef _C_TEXTURE_MASTER_H_
#define _C_TEXTURE_MASTER_H_

// Includes
//#include "CRenderState.h"
#include "CTexture.h"
#include <vector>
#include <map>
#include <string>
//using std::vector;
//using std::map;
using std::string;

// Texture info storage class definition
class glTexture
{
public:
	tTextureInfo info;
};

// Storage Class Definition
class TexmapSetReference
{
public:
	unsigned int referenceCount;
	glTexture texture;
};

// Class definition
class CTextureMaster
{
public:
	CTextureMaster ( void );
	~CTextureMaster ( void );

	glTexture* AddReference ( CTexture* ptex );
	glTexture* AddReference ( string texMapSId, CTexture* ptex );

	void RemoveReference ( CTexture* ptex );
	void RemoveReference ( string texMapSId, CTexture* ptex );

	const glTexture* GetReference ( CTexture* ptex );
	const glTexture* GetReference ( string texMapSId, CTexture* ptex );

	void ReloadAll ( void );

	void Cleanup ( void );

	void UpdateTextureIndex ( const uint n_old, const uint n_new );
private:
	std::map<string,TexmapSetReference> textureMap;
	std::vector<std::pair<CTexture*,uint>>	textureList;
};

// Global Class
extern CTextureMaster TextureMaster;

#endif