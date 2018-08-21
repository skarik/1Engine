#ifndef RENDERER_LIGHT_H_
#define RENDERER_LIGHT_H_

// Includes
//#include "CGameObject.h"
//#include "GLCommon.h"

#include "core/math/Color.h"
#include "core/math/Vector3d.h"

#include "renderer/logic/RrLogicObject.h"

//#include "RrCamera.h"

// == Includes ==
#include <vector>
#include <queue>

class RrLight;
class RrCamera;
class RrRTCamera;
class RrRenderTexture;
class CBillboard;

class RrLightComparison
{
public:
	RrCamera*			m_camera;

public:
	// Light comparison. Returns true if light1 is more important than light2
    bool					operator() ( RrLight*, RrLight* );
};

class RrLightComparisonExternal
{
public:
    bool operator() ( RrLight*, RrLight* );
};

// Global External Comparison
//extern RrLightComparisonExternal LightComparator;

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

// == Class Definition ==
class RrLight : public RrLogicObject
{
public:
	RENDER_API RrLight ( void );
	RENDER_API ~RrLight( void );

	// Game step
	//void Update ( void );
	//void LateUpdate ( void );
	
	// Physics step
	//void FixedUpdate ( void ) {};
	void					PreStepSynchronus ( void ) override;
	
	//void					UpdateLight ( void );

	// Get light send vector
	//virtual Color			GetSendVector ( void );

	//RrRTCamera*				GetShadowCamera ( void )
	//	{ return shadowCamera; };
	//RrRenderTexture*		GetShadowTexture ( void )
	//	{ return shadowTexture; };

public:
	//	UpdateLights( camera ) : Updates light listing for the currently rendering camera.
	static void				UpdateLights ( RrCamera* rendering_camera );

	// Get light list
	static std::vector<RrLight*>*
							GetLightList ( void )
		{ return (&lightList); }
	/*static std::vector<RrLight*>*
							GetActiveLightList ( void )
		{ return (&activeLightList); }*/

public:
	// General Light Parameters

	// Light type
	rrLightType		type;
	// Light shape
	rrLightAreaType	area_type;

	// Position, ignored if directional
	Vector3f		position;
	// Direction, controls effective rotation
	Vector3f		direction;
	// Size parameters, ignored if directional or point
	Vector3f		size;
	// Color
	Color			color;
	// Distance in feet the light is forced to not effect beyond. Default is 500.
	float			falloff_range;
	// The power of the falloff. The default is 2.0, for inverse square.
	float			falloff_invpower;
	// Amount the light should not take normals into account. Default is 0.0.
	// This value may be ignored in some lighting models.
	float			falloff_passthru;

	// Position and Direction
	//Color vLightPos;
	//Color vLightDir;
	//bool  isDirectional;

	// Shadow
	//bool  generateShadows;
	//float shadowStrength;
	//int	  shadowResolution;

	// Are shadows enabled for this light?
	bool			shadows_enabled;
	// Percentage of base resolution shadows should be. Default is 1.0.
	float			shadows_resolution_factor;
	
	// Effects
	//bool  drawHalo;
	//Real haloStrength;

	// Does this light force a volumetric halo on? Default is 0.0.
	float			effect_halo_strength;
protected:
	// Effects
	//CBillboard*	mHalo;

protected:
	// Calculated attenuation for use in fallback-to-pipeline mode.
	//Color attenuation;
	// Calculated index for lists
	//char lightIndex;
	uint16_t		m_lightIndex;

	// Shadow instances
	//RrRTCamera*		shadowCamera;
	//RrRenderTexture*shadowTexture;
	// Virtual function for specific shadow camera code
	//virtual void			UpdateShadowCamera ( void );
	// Creation of render targets
	//void					UpdateShadows ( void );

	// Friend classes for comparison
	friend RrLightComparison;
	friend RrLightComparisonExternal;

	// The is the position of the currently rendering camera.
	// All cameras, including shadow cameras will affect this value.
	//static Vector3d vCameraPos;

private:
	// Comparison
	//static void				UpdateLightList ( void );

	// Light lists
	//static bool			bListReset;
	static std::vector<RrLight*>
						lightList;
	/*static std::vector<RrLight*>
						activeLightList;*/
	//static vector<RrLight*> activeLightList;
	//static std::priority_queue<RrLight*, std::vector<RrLight*>, RrLightComparison>
	//					activeLightQueue;
	
	//char previousLightIndex;

};

#endif