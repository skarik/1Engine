#ifndef RENDERER_LIGHT_H_
#define RENDERER_LIGHT_H_

#include "core/math/Color.h"
#include "core/math/Vector3d.h"
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
	kLightTypeArea,
	kLightTypeAreaSpotlight,
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
class RrLightComparison
{
public:
	RrCamera*			m_camera;

public:
	// Light comparison. Returns true if light1 is more important than light2
	bool					operator() ( RrLight*, RrLight* );
};

// Renderer light.
class RrLight : public RrLogicObject
{
public:
	RENDER_API				RrLight ( void );
	RENDER_API				~RrLight( void );

	void					PreStepSynchronus ( void ) override;

public:
	//	UpdateLights( camera ) : Updates light listing for the currently rendering camera.
	static void				UpdateLights ( RrCamera* rendering_camera );

	// Get light list
	static std::vector<RrLight*>*
							GetLightList ( void )
		{ return (&lightList); }

public:
	// General Light Parameters

	// Light type
	rrLightType			type;
	// Light shape
	rrLightAreaType		area_type;

	// Position, ignored if directional
	Vector3f			position;
	// Direction, controls effective rotation
	Vector3f			direction;
	// Size parameters, ignored if directional or point
	Vector3f			size;
	// Color
	Color				color;
	// Distance in feet the light is forced to not effect beyond. Default is 500.
	float				falloff_range;
	// The power of the falloff. The default is 2.0, for inverse square.
	float				falloff_invpower;
	// Amount the light should not take normals into account. Default is 0.0.
	// This value may be ignored in some lighting models.
	float				falloff_passthru;

	// Are shadows enabled for this light?
	bool				shadows_enabled;
	// Percentage of base resolution shadows should be. Default is 1.0.
	float				shadows_resolution_factor;

	// Does this light force a volumetric halo on? Default is 0.0.
	float				effect_halo_strength;

protected:
	// Calculated index for lists
	uint16_t			m_lightIndex;

	// Friend classes for comparison
	friend RrLightComparison;

private:
	static std::vector<RrLight*>
						lightList;
	static uint16_t		lightsActive;


};

#endif