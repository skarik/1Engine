#ifndef RENDERER_TEXTURE_H_
#define RENDERER_TEXTURE_H_

#include "core/types/arBaseObject.h"
#include "core/math/vect2d_template.h"
#include "core/containers/arstring.h"
#include "core/gfx/textureFormats.h"
#include "core/gfx/textureStructures.h"
#include "core/gfx/pixelFormat.h"
#include "renderer/types/types.h"
#include "renderer/texture/rrSamplerState.h"

#include <stdio.h>
#include <string>

// ==Defines==
// A define to allow for a function to differentiate between different kinds of textures in the texture master class
#define TextureType(a) public: RENDER_API virtual core::gfx::tex::arTextureClass ClassType ( void ) { return a; };

class RrTextureMaster;
class CResourceManager;

//	class RrTexture : Base resource-aware texture class.
// Will attempt to load a texture from disk.
class RrTexture : public arBaseObject
{
	TextureType( core::gfx::tex::kTextureClassBase );
protected:
	//	Constructor.
	// Pass in an empty string for the filename to just initialize a texture instance without any data.
	RENDER_API explicit		RrTexture (
		const char* sInFilename,
		core::gfx::tex::arTextureType	textureType		= core::gfx::tex::kTextureType2D,
		core::gfx::tex::arColorFormat	format			= core::gfx::tex::kColorFormatRGBA8,
		uint16_t						maxTextureWidth	= 1024,
		uint16_t						maxTextureHeight= 1024,
		core::gfx::tex::arWrappingType	repeatX			= core::gfx::tex::kWrappingRepeat,
		core::gfx::tex::arWrappingType	repeatY			= core::gfx::tex::kWrappingRepeat,
		core::gfx::tex::arMipmapGenerationStyle	mipmapGeneration = core::gfx::tex::kMipmapGenerationNormal,
		core::gfx::tex::arSamplingFilter	/*ignored*/	= core::gfx::tex::kSamplingLinear
		);
	RENDER_API ~RrTexture ( void );

public:
	//	Load ( filename ) : Loads a texture from the disk.
	// May return a previously loaded instance of the texture.
	RENDER_API static RrTexture*	Load ( const char* resource_name );
	//	CreateUnitialized ( name ) : Creates an uninitialized texture object.
	// Can be used for procedural textures, with Upload(...) later.
	RENDER_API static RrTexture*	CreateUnitialized ( const char* name );

public:
	RENDER_API void Bind ( void );
	RENDER_API void Unbind ( void );
	RENDER_API static void Unbind ( char );

	RENDER_API unsigned int GetWidth ( void ) { return info.width; };
	RENDER_API unsigned int GetHeight( void ) { return info.height; };
	RENDER_API Vector2i GetSize ( void ) { return Vector2i((int32_t)info.width, (int32_t)info.height); }
	RENDER_API unsigned int GetDepth ( void ) { return info.depth; };
	RENDER_API core::gfx::tex::arTextureType GetType ( void ) { return info.type; };

	/*RENDER_API virtual glHandle GetColorSampler ( void ) {
		return info.index;
	}
	RENDER_API virtual eTextureType GetSamplerTarget ( void ) {
		return info.type;
	}*/
	RENDER_API virtual bool GetIsFont ( void ) {
		return false;
	}

	RENDER_API const core::gfx::tex::arImageInfo& GetIOImgInfo ( void ) const {
		return io_imginfo;
	}

	RENDER_API virtual void Reload ( void );

	RENDER_API void GenerateMipmap ( core::gfx::tex::arMipmapGenerationStyle generationStyle = core::gfx::tex::kMipmapGenerationNormal );

	RENDER_API void Upload (
		core::gfx::arPixel* data,
		uint16_t width,
		uint16_t height,
		core::gfx::tex::arWrappingType	repeatX			= core::gfx::tex::kWrappingRepeat,
		core::gfx::tex::arWrappingType	repeatY			= core::gfx::tex::kWrappingRepeat,
		core::gfx::tex::arMipmapGenerationStyle	mipmapGeneration = core::gfx::tex::kMipmapGenerationNormal,
		core::gfx::tex::arSamplingFilter	/*ignored*/	= core::gfx::tex::kSamplingLinear
		);

	//RENDER_API virtual void SetFilter ( core::gfx::tex::arSamplingFilter filter );
	RENDER_API virtual rrSamplerState& GetDefaultSamplerState ( void );
protected:
	friend RrTextureMaster;
	friend CResourceManager;

	//string			sFilename;	
	arstring256						resourceName;
	core::gfx::tex::arTextureInfo	info;
	core::gfx::tex::arTextureState	state;
	rrSamplerState					defaultSamplerState;

	core::gfx::arPixel*				pData;

	core::gfx::tex::arImageInfo		io_imginfo;
	
protected:
	RENDER_API void LoadImageInfo ( void );
	/*
	void loadTGA ( void );
	void loadPNG ( void );
	void loadJPG ( void );

	void loadDefault ( void );
	*/
};

#endif//RENDERER_TEXTURE_H_