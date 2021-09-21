#ifndef CORE_PHYSICAL_MATERIAL_H_
#define CORE_PHYSICAL_MATERIAL_H_

#include "core/containers/arstring.h"
#include "core/math/Vector2.h"
#include "core/math/Vector3.h"
#include "physical/material/PhysMaterial.h"

// Prototypes for use in other modules
class RrShaderProgram;
class RrTexture;

// For other information translation, see queries defined in engine & renderer

enum class ArRenderMode
{
	// Does this render as an opaque deferred object?
	kLitOpaque,
	// Render as a deferred decal?
	kFastDecal,

	// The following modes are not currently essential and are TODO:

	//// Render as transparent object? Chiefly used for glass.
	//kLitTransparent,
	//// Render as a warping-only object.
	//kWarpOnly,
	//// Render as a transparent object that warps objects drawn behind it. Used for stained glass, water...
	//kLitTransparentWarp,
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

#endif//CORE_PHYSICAL_MATERIAL_H_