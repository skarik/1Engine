#ifndef CORE_PHYSICAL_MATERIAL_H_
#define CORE_PHYSICAL_MATERIAL_H_

#include "core/types/arBaseObject.h"
#include "core/containers/arstring.h"
#include "core/math/Vector2.h"
#include "core/math/Vector3.h"
#include "core/math/Vector4.h"
#include "core/gfx/textureFormats.h"
#include "physical/material/PhysMaterial.h"

class RrShaderProgram;
class RrTexture;
class ArMaterial;
class ArMaterialContainer;
	
//	ArLoadMaterial( resource_name ) : Loads the given material from file.
// If already loaded, returns the previously loaded material.
// The information is added to the resource system.
PHYS_API ArMaterialContainer*
							ArLoadMaterial ( const char* resource_name );

//	ArFindMaterial( resource_name ) : Finds a previously loaded material.
PHYS_API ArMaterialContainer*
							ArFindMaterial ( const char* resource_name );

// For other information translation, see queries defined in engine & renderer

enum class ArRenderMode
{
	// Does this render as an opaque deferred object?
	kLitOpaque,
	// Render as a deferred decal?
	kFastDecal,
	// Render as alpha-tested foliage?
	kLitFoliage,

	// The following modes are not currently essential and are TODO:

	//// Render as transparent object? Chiefly used for glass.
	//kLitTransparent,
	//// Render as a warping-only object.
	//kWarpOnly,
	//// Render as a transparent object that warps objects drawn behind it. Used for stained glass, water...
	//kLitTransparentWarp,
};

enum class ArFacingCullMode
{
	kNone,
	kFront,
	kBack,
};

class ArMaterial
{
public:
	struct RenderInfo
	{
		ArRenderMode	render_mode = ArRenderMode::kLitOpaque;

		arstring128		shader_vv = "shaders/deferred_env/simple_vv";
		arstring128		shader_h = nullptr;
		arstring128		shader_d = nullptr;
		arstring128		shader_g = nullptr;
		arstring128		shader_p = "shaders/deferred_env/simple_p";
		RrShaderProgram*
						shader_pipeline = nullptr;

		ArFacingCullMode
						cull = ArFacingCullMode::kBack;
		float			alpha_test = 0.0F;

		Vector4f		diffuse_color = Vector4f(1, 1, 1, 1);
		float			smoothness_bias = 0.0F;
		float			smoothness_scale = 1.0F;
		float			metallicness_bias = 0.0F;
		float			metallicness_scale = 1.0F;

		struct TextureEntry
		{
			RrTexture*	cached_texture = nullptr;
			arstring256	resource_name = nullptr;
		};
		TextureEntry	texture_diffuse;
		TextureEntry	texture_normals;
		TextureEntry	texture_surface;
		TextureEntry	texture_overlay;
		TextureEntry	texture_detail;

		core::gfx::tex::arSamplingFilter
						sampling = core::gfx::tex::kSamplingLinear;

		Vector3f		repeat_factor = Vector3f(1, 1, 1);
	};

	struct LightingInfo
	{
		bool			emits = false;
		Vector3f		emissive = Vector3f(0, 0, 0);
	};

public:
	// Material name used for editing & debugging purposes
	arstring128			name = "Unnamed";

	// Physics material info:
	PrPhysMaterialType	physMat = kPhysMatDefault;
	
	// Rendering material info:
	RenderInfo			render_info;

	// Slow Rendering material info:
	LightingInfo		lighting_info;
};

#include "core-ext/resources/IArResource.h"

class ArMaterialContainer : public arBaseObject, public IArResource
{
public:
	PHYS_API explicit		ArMaterialContainer ( ArMaterial* material, const char* resourceFilename )
		: arBaseObject()
		, m_material(material)
		, m_resourceFilename(resourceFilename)
		{}

	//	virtual ResourceType() : What type of resource is this?
	// Identifies the type of resource this is.
	PHYS_API virtual core::arResourceType
							ResourceType ( void ) override 
		{ return core::kResourceTypeArMaterial; }

	//	virtual ResourceName() : Returns the resource name.
	// This is used to search for the resource. The smaller, the better.
	PHYS_API virtual const char* const
							ResourceName ( void ) override
		{ return m_resourceFilename; }

public:
	// The material that's being cached by this container
	ArMaterial*			m_material = nullptr;

protected:
	// The filename the material was cached from
	arstring256			m_resourceFilename;
};

#endif//CORE_PHYSICAL_MATERIAL_H_