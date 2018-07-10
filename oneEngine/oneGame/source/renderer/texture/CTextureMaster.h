
#ifndef _C_TEXTURE_MASTER_H_
#define _C_TEXTURE_MASTER_H_

// Includes
//#include "CRenderState.h"
#include "RrTexture.h"
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
class RrTextureMaster
{
public:
	RrTextureMaster ( void );
	~RrTextureMaster ( void );

	textureEntry_t* AddReference ( RrTexture* ptex );
	textureEntry_t* AddReference ( string texMapSId, RrTexture* ptex );

	void RemoveReference ( RrTexture* ptex );
	void RemoveReference ( string texMapSId, RrTexture* ptex );

	const textureEntry_t* GetReference ( RrTexture* ptex );
	const textureEntry_t* GetReference ( string texMapSId, RrTexture* ptex );

	void ReloadAll ( void );

	void Cleanup ( void );

	void UpdateTextureIndex ( const uint n_old, const uint n_new );
private:
	std::map<string,TexmapSetReference> textureMap;
	std::vector<std::pair<RrTexture*,uint>>	textureList;
};

// Global Class
extern RrTextureMaster TextureMaster;

#endif