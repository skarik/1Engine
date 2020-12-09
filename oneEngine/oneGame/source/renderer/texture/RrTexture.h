#ifndef RENDERER_TEXTURE_H_
#define RENDERER_TEXTURE_H_

#include "core/types/arBaseObject.h"
#include "core/math/vect2d_template.h"
#include "core/math/vect3d_template.h"
#include "core/containers/arstring.h"
#include "core/gfx/textureFormats.h"
#include "core/gfx/textureStructures.h"
#include "core/gfx/pixelFormat.h"
#include "core-ext/resources/IArResource.h"
#include "renderer/types/types.h"
#include "renderer/texture/rrSamplerState.h"
#include "gpuw/Texture.h"

#include <stdio.h>
#include <string>

//
// Macros

// Required macro needed in texture classes. Allows a function to differentiate between different kinds of textures in the texture master class.
#define TextureType(a) public: RENDER_API virtual core::gfx::tex::arTextureClass ClassType ( void ) { return a; };

//
// Prototypes

class RrTextureMasterSubsystem;

//
// Types

//	rrTextureLoadState
// Constants for current loading state
enum rrTextureLoadState
{
	kTextureLoadState_Verfication = 0,
	kTextureLoadState_LoadSuperlow,
	kTextureLoadState_GPUSuperlow,
	kTextureLoadState_LoadImage,
	kTextureLoadState_GPUImage,
	kTextureLoadState_Done,
};

//	rrTextureLoadInfo
// Structure for current loading state
struct rrTextureLoadInfo;

//	rrTextureUploadInfo
// Structure for texture upload request
struct rrTextureUploadInfo
{
	core::gfx::tex::arTextureType
						type;
	void*				data;
	uint16_t			width;
	uint16_t			height;
	uint16_t			depth;
	core::gfx::tex::arColorFormat
						format;
};

//	rrTextureExtraInfo
// Structure for optional additional texture information
struct rrTextureExtraInfo
{
	core::gfx::tex::arAnimationInfo*
						animationInfo;
	core::gfx::tex::arSpriteInfo*
						spriteInfo;

	rrTextureExtraInfo ( void )
		: animationInfo(NULL), spriteInfo(NULL)
		{}
};

// constants: move these elsewhere later
namespace renderer
{
	static const char* kTextureBlack = "rr_black";
	static const char* kTextureWhite = "rr_white";
	static const char* kTextureGrayA0 = "rr_graya0";
	static const char* kTextureNormalN0 = "rr_normalN0";
}

//	class RrTexture : Base resource-aware texture class.
// Instantiate with either RrTexture::Load or RrTexture::CreateUnitialized.
class RrTexture : public arBaseObject, public IArResource
{
	TextureType( core::gfx::tex::kTextureClassBase );
protected:
	RENDER_API explicit		RrTexture (
		const char* s_resourceId,
		const char* s_resourcePath
		);

	RENDER_API virtual		~RrTexture ( void );

public: // Creation Interface

	//	Load ( filename ) : Loads a texture from the disk.
	// May return a previously loaded instance of the texture.
	// The reference count of the returned instance will not be incremented.
	RENDER_API static RrTexture*
							Load ( const char* resource_name );
	//	CreateUnitialized ( name ) : Creates an uninitialized texture object.
	// Can be used for procedural textures, with Upload(...) later.
	// The reference count of the returned instance will not be incremented.
	RENDER_API static RrTexture*
							CreateUnitialized ( const char* name );
	//	Find ( name ) : Finds a texture with the given resource handle.
	// Can be used to locate previously created textures, especially procedural ones.
	// The reference count of the returned instance will not be incremented.
	RENDER_API static RrTexture*
							Find ( const char* name );

public: // Resource Interface

	//	virtual ResourceType() : What type of resource is this?
	// Identifies the type of resource this is.
	RENDER_API core::arResourceType
							ResourceType ( void ) override { return core::kResourceTypeRrTexture; }
	//	virtual ResourceName() : Returns the resource name.
	// This is used to search for the resource. The smaller, the better.
	RENDER_API const char* const
							ResourceName ( void ) override { return resourceName.c_str(); }

	//	virtual IsStreamedLoad() : Is this a streamed resource?
	// Returns:
	//	bool:	True when the resource should be added to the streaming list.
	//			False when the resource should be loaded immediately.
	RENDER_API bool			IsStreamedLoad ( void ) override { return streamed; }

	//	virtual OnStreamStep() : Callback for streaming load.
	// Arguments:
	//	sync_client: True when performing a synchronized stream step.
	//               When synchronized, some remote commands (GPU) will need to finish before continuing.
	// Returns:
	//	bool:	False when still loading, True when done.
	//          The resource will stay in the "loading" list until it is done.
	RENDER_API bool			OnStreamStep ( bool sync_client ) override;

protected:

	//	OnStreamStepProcedural() : Step for streaming load of procedural textures.
	RENDER_API bool			OnStreamStepProcedural ( bool sync_client );

	//	OnStreamStepDisk() : Step for streaming load of textures from disk.
	RENDER_API bool			OnStreamStepDisk ( bool sync_client );

public: // Kitchen Sink Interface

	RENDER_API unsigned int	GetWidth ( void ) { return info.width; };
	RENDER_API unsigned int	GetHeight( void ) { return info.height; };
	RENDER_API unsigned int	GetDepth ( void ) { return info.depth; };
	RENDER_API Vector3i		GetSize ( void ) { return Vector3i((int32_t)info.width, (int32_t)info.height, (int32_t)info.depth); }
	RENDER_API core::gfx::tex::arTextureType
							GetType ( void ) { return info.type; };

	RENDER_API virtual bool	GetIsFont ( void ) { return false; }
	RENDER_API virtual bool GetIsProcedural ( void ) { return procedural; }

	RENDER_API const rrTextureExtraInfo
							GetExtraInfo ( void ) { return extra_info; }

	//	Reload() : Forces resource system to add self back into the streaming list.
	RENDER_API virtual void	Reload ( void );

	//	GenerateMipmap( gen_style ) : Generates mipmaps for the texture.
	// Call will be ignored if not actually needed.
	RENDER_API gpu::Texture&
							GetTexture ( void ) { return m_texture; } 

	//	Upload(...) : Upload data to the texture, initializing it.
	// Use for procedural textures.
	// This will add the object to the resource manager, but will NOT add to the object's reference count.
	RENDER_API void			Upload (
		bool							streamed,
		void*							data,
		uint16_t						width,
		uint16_t						height,
		core::gfx::tex::arColorFormat	format,
		core::gfx::tex::arWrappingType	repeatX			= core::gfx::tex::kWrappingRepeat,
		core::gfx::tex::arWrappingType	repeatY			= core::gfx::tex::kWrappingRepeat,
		core::gfx::tex::arMipmapGenerationStyle	mipmapGeneration = core::gfx::tex::kMipmapGenerationNormal,
		core::gfx::tex::arSamplingFilter	defaultSamplerFilter = core::gfx::tex::kSamplingLinear
		);

	//RENDER_API virtual void SetFilter ( core::gfx::tex::arSamplingFilter filter );
	RENDER_API virtual rrSamplerState&
							GetDefaultSamplerState ( void ) { return defaultSamplerState; }
protected:
	friend RrTextureMasterSubsystem;

	// procedurally created by either cpu or gpu
	bool				procedural;
	// is this texture streamed and loaded over a period of time, or loaded immediately?
	bool				streamed;

	// gpu Texture object
	gpu::Texture		m_texture;

	// texture resource named used for resource management & pointing to file
	arstring256			resourceName;
	// texture general information
	core::gfx::tex::arTextureInfo
						info;
	// texture state, containing mips that can be bound
	core::gfx::tex::arTextureState
						state;
	// default sampler information
	rrSamplerState		defaultSamplerState;

	// temp pointer for data that is provided by Upload()
	rrTextureUploadInfo*
						upload_request;

	rrTextureExtraInfo	extra_info;
	
protected:
	//	StreamingReset() : Resets streaming state.
	RENDER_API void			StreamingReset ( void );

	rrTextureLoadState	loadState;
	rrTextureLoadInfo*	loadInfo;
	arstring256			resourceFilename;
};

#endif//RENDERER_TEXTURE_H_