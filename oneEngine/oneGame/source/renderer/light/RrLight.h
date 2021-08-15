#ifndef RENDERER_LIGHT_H_
#define RENDERER_LIGHT_H_

#include "core/math/Color.h"
#include "core/math/Vector3.h"
#include "renderer/logic/RrLogicObject.h"
#include <vector>
#include <queue>

class RrLight;
class RrCamera;
class RrRTCamera;
class RrRenderTexture;
class CBillboard;

enum rrLightType : uint8
{
	kLightTypeOmni			= 0,
	kLightTypeSpotlight		= 1,
	kLightTypeDirectional	= 2,
};

enum rrLightAreaType : uint8
{
	kLightAreaTypePoint			= 0,
	kLightAreaTypeSphere		= 1,
	kLightAreaTypeRotatedTube	= 2,
	kLightAreaTypeRotatedQuad	= 3,
};

// Comparison class. Used to sort lights.
class RrLightComparison;

namespace renderer
{
	// Listing of all lights.
	extern std::vector<RrLight*> g_lightList;

	//	LightList() : Return the current list of lights.
	RENDER_API const std::vector<RrLight*>&
							LightList ( void );
}

// Renderer light.
class RrLight : public RrLogicObject
{
public:
	RENDER_API				RrLight ( void );
	RENDER_API				~RrLight( void );

	void					PreStepSynchronus ( void ) override;

public:
	// General Light Parameters

	// Light type
	rrLightType			type = kLightTypeOmni;
	// Light shape
	rrLightAreaType		area_type = kLightAreaTypePoint;

	// Position, ignored if directional
	Vector3f			position = Vector3f(0, 0, 0);
	// Direction, controls effective rotation
	Vector3f			direction = Vector3f(0, 0, -1.0F);
	// Size parameters, ignored if directional or point
	Vector3f			size = Vector3f(0.1F, 0.1F, 0.1F);
	// Color
	Color				color = Color(1.0F, 1.0F, 1.0F, 1.0F);
	// Distance in feet the light is forced to not effect beyond. Default is 10.
	float				falloff_range = 10.0F;
	// The power of the falloff. The default is 2.0, for inverse square.
	float				falloff_invpower = 2.0F;
	// Amount the light should not take normals into account. Default is 0.0.
	// This value may be ignored in some lighting models.
	float				falloff_passthru = 0.0F;

	struct ShadowOptions
	{
		// Are shadows enabled for this light?
		int				enabled : 1;

		// Are shadows rendered using shadow maps?
		int				use_shadow_maps : 1;
		// Are contact shadows used to render the maps?
		int				use_contact_shadows : 1;
		// Is only non-static geometry used to cast shadows from this light?
		int				only_cast_dynamic : 1;
		// Does this cast shadows into volumetrics?
		int				cast_into_volumetrics : 1;

		// Percentage of base resolution shadows should be. Default is 1.0.
		float			shadows_resolution_factor = 1.0F;

		ShadowOptions()
			: enabled(false)
			, use_contact_shadows(true)
			, use_shadow_maps(true)
			, only_cast_dynamic(false)
			, cast_into_volumetrics(true)
			{}

		bool				AreEnabled ( void )
			{ return enabled && (use_shadow_maps || use_contact_shadows); }
	};
	// All the shadow configuration options.
	ShadowOptions		shadows;

	// Density multiplier of the volumetrics around this light.
	float				volumetric_density = 1.0F;

protected:
	// Friend classes for comparison
	friend RrLightComparison;
};

namespace renderer
{
	namespace cbuffer
	{
		struct rrLight
		{
			Vector3f			position;
			float				falloff_range;
			Vector3f			direction;
			float				falloff_invpower;
			Vector3f			size;
			float				falloff_passthru;
			Vector3f			color;
			rrLightAreaType		area_type;
			rrLightType			light_type;
			uint16				padding;

			rrLight() = default;

			rrLight (RrLight* source)
				: position(source->position)
				, falloff_range(source->falloff_range)
				, direction(source->direction)
				, falloff_invpower(source->falloff_invpower)
				, size(source->size)
				, falloff_passthru(source->falloff_passthru)
				, color(source->color.r, source->color.g, source->color.b)
				, area_type(source->area_type)
				, light_type(source->type)
				, padding(0)
			{}
		};
	}
}

#endif