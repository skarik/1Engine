#ifndef RENDERER_ATLASED_TEXTURE_H_
#define RENDERER_ATLASED_TEXTURE_H_

#include "RrTexture.h"
#include <vector>

struct rrAtlasedTextureEntry
{
	core::arResourceHash
						hash;
	arstring256			resource_name;
	Vector2i			pixel_position;
	Vector2i			pixel_size;
};

//	class RrAtlasedTexture : Experimental atlased texture class, for 3D texture.
// Instantiate with RrAtlasedTexture::CreateEmpty, populate with texture->Add
class RrAtlasedTexture : public RrTexture
{
	TextureType( core::gfx::tex::kTextureClassBase );
protected:
	RENDER_API explicit		RrAtlasedTexture (
		const char* s_resourceId,
		const char* s_resourcePath
	);

	RENDER_API virtual		~RrAtlasedTexture ( void );

public: // Creation Interface

	//	CreateUnitialized ( name ) : Creates an empty atlas object.
	// This texture should be updated with Add(...) later.
	// The reference count of the returned instance will not be incremented.
	RENDER_API static RrAtlasedTexture*
							CreateEmpty ( const char* name );

public:

	//	Add ( filename ) : Adds a texture from the disk to the atlas.
	// May return information of a previously loaded instance of the texture.
	RENDER_API const rrAtlasedTextureEntry*
							Add ( const char* filename );

private:
	
	// Listing all textures that this atlas has
	std::vector<rrAtlasedTextureEntry>
						subtextures;

	// The texture data this texture has so far
	uint32_t*			textureData;

};

#endif//RENDERER_ATLASED_TEXTURE_H_