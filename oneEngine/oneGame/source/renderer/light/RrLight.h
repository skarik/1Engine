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

enum rrLightType
{
	kLightTypeOmni,
	kLightTypeSpotlight,
	kLightTypeDirectional,
};

enum rrLightAreaType
{
	kLightAreaTypePoint,
	kLightAreaTypeSphere,
	kLightAreaTypeRotatedTube,
	kLightAreaTypeRotatedQuad,
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
	//	UpdateLights( camera ) : Updates light listing for the currently rendering camera.
	//static void				UpdateLights ( RrCamera* rendering_camera );

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
	// Distance in feet the light is forced to not effect beyond. Default is 500.
	float				falloff_range = 500.0F;
	// The power of the falloff. The default is 2.0, for inverse square.
	float				falloff_invpower = 2.0F;
	// Amount the light should not take normals into account. Default is 0.0.
	// This value may be ignored in some lighting models.
	float				falloff_passthru = 0.0F;

	// Are shadows enabled for this light?
	bool				shadows_enabled = false;
	// Percentage of base resolution shadows should be. Default is 1.0.
	float				shadows_resolution_factor = 1.0F;

	// Does this light force a volumetric halo on? Default is 0.0.
	float				effect_halo_strength = 0.0F;

protected:
	// Calculated index for lists
	//uint16_t			m_lightIndex;

	// Friend classes for comparison
	friend RrLightComparison;

private:
	//static std::vector<RrLight*>
	//					lightList;
	//static uint16_t		lightsActive;
};

#endif