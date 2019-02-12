#ifndef RR_TEXTURE_3D_H_
#define RR_TEXTURE_3D_H_

#include "RrTexture.h"

//	class RrTexture : Resource-aware texture class, for 3D texture.
// Instantiate with either RrTexture::Load or RrTexture::CreateUnitialized.
class RrTexture3D : public RrTexture
{
	TextureType( core::gfx::tex::kTextureClass3D );
protected:
	RENDER_API explicit		RrTexture3D (
		const char* s_resourceId,
		const char* s_resourcePath
	);

	RENDER_API virtual		~RrTexture3D ( void );

public: // Creation Interface

	//	Load ( filename ) : Loads a texture from the disk.
	// May return a previously loaded instance of the texture.
	// The reference count of the returned instance will not be incremented.
	RENDER_API static RrTexture*
							Load3D ( const char* resource_name );

	//	Load ( filename ) : Loads a texture from the disk.
	// May return a previously loaded instance of the texture.
	// The reference count of the returned instance will not be incremented.
	RENDER_API static RrTexture*
							LoadAtlas ( const char* resource_name, const int atlas_sx, const int atlas_sy );

	//	CreateUnitialized ( name ) : Creates an uninitialized texture object.
	// Can be used for procedural textures, with Upload(...) later.
	// The reference count of the returned instance will not be incremented.
	RENDER_API static RrTexture*
							CreateUnitialized ( const char* name );

public:

	//	Upload(...) : Upload data to the texture, initializing it.
	// Use for procedural textures.
	// This will add the object to the resource manager, but will NOT add to the object's reference count.
	RENDER_API void			Upload (
		bool							streamed,
		core::gfx::arPixel*				data,
		uint16_t						width,
		uint16_t						height,
		uint16_t						depth,
		core::gfx::tex::arWrappingType	repeatX			= core::gfx::tex::kWrappingRepeat,
		core::gfx::tex::arWrappingType	repeatY			= core::gfx::tex::kWrappingRepeat,
		core::gfx::tex::arWrappingType	repeatZ			= core::gfx::tex::kWrappingRepeat,
		core::gfx::tex::arMipmapGenerationStyle	mipmapGeneration = core::gfx::tex::kMipmapGenerationNormal,
		core::gfx::tex::arSamplingFilter	/*ignored*/	= core::gfx::tex::kSamplingLinear
	);

};

#endif//RR_TEXTURE_3D_H_