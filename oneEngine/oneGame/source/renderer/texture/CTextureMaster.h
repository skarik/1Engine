
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
struct textureEntry_t
{
public:
	tTextureInfo info;
};

// Storage Class Definition
struct TexmapSetReference
{
	unsigned int referenceCount;
	textureEntry_t texture;
};

// Class definition
class CTextureMaster
{
public:
	CTextureMaster ( void );
	~CTextureMaster ( void );

	textureEntry_t* AddReference ( CTexture* ptex );
	textureEntry_t* AddReference ( string texMapSId, CTexture* ptex );

	void RemoveReference ( CTexture* ptex );
	void RemoveReference ( string texMapSId, CTexture* ptex );

	const textureEntry_t* GetReference ( CTexture* ptex );
	const textureEntry_t* GetReference ( string texMapSId, CTexture* ptex );

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