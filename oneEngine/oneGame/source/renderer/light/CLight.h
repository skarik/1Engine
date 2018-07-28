
#ifndef _C_LIGHT_H_
#define _C_LIGHT_H_

// Includes
//#include "CGameObject.h"
//#include "GLCommon.h"

#include "core/math/Color.h"
#include "core/math/Vector3d.h"

#include "renderer/logic/CLogicObject.h"

//#include "RrCamera.h"

// == Includes ==
#include <vector>
#include <queue>

// Usings
/*using std::vector;
using std::find;
using std::priority_queue;*/

// Class predefinition
class CLight;
class RrCamera;
class RrRTCamera;
class RrRenderTexture;
class CBillboard;

// == Comparison Structs ==
class CLightComparison {
public:
    bool operator() ( CLight*, CLight* );
};
class CLightComparisonExternal {
public:
    bool operator() ( CLight*, CLight* );
};

// Global External Comparison
extern CLightComparisonExternal LightComparator;

// == Class Definition ==
class CLight : public CLogicObject//: public CGameObject
{
public:
	RENDER_API CLight ( void );
	RENDER_API ~CLight( void );

	// Game step
	//void Update ( void );
	//void LateUpdate ( void );
	
	// Physics step
	//void FixedUpdate ( void ) {};
	void PreStepSynchronus ( void ) override;

	// Render step
	static void UpdateLights ( void );
	virtual void UpdateLight ( void );

	// Get light send vector
	virtual Color GetSendVector ( void );

	// Get the shadow camera and texture
	RrRTCamera*		GetShadowCamera ( void ) { return shadowCamera; };
	RrRenderTexture*	GetShadowTexture ( void ) { return shadowTexture; };

	// Get light list
	static std::vector<CLight*>* GetLightList ( void )
	{
		return (&lightList);
	}
	static std::vector<CLight*>* GetActiveLightList ( void )
	{
		return (&activeLightList);
	}

public:
	// == Publics Settings ==
	// Position & Rotaton
	Vector3d position;

	// Color
	Color diffuseColor;
	Color hdrColor;

	// Falloff
	float range;
	float falloff;
	float pass;

	// Position and Direction
	Color vLightPos;
	Color vLightDir;
	bool  isDirectional;

	// Shadow
	bool  generateShadows;
	float shadowStrength;
	int	  shadowResolution;
	
	// Effects
	bool  drawHalo;
	Real haloStrength;
protected:
	// Effects
	CBillboard*	mHalo;

protected:
	// Calculated attenuation for use in fallback-to-pipeline mode.
	Color attenuation;
	// Calculated index for lists
	char lightIndex;

	// Shadow instances
	RrRTCamera*		shadowCamera;
	RrRenderTexture*	shadowTexture;
	// Virtual function for specific shadow camera code
	virtual void UpdateShadowCamera ( void );
	// Creation of render targets
	void UpdateShadows ( void );

	// Friend classes for comparison
	friend CLightComparison;
	friend CLightComparisonExternal;

	// The is the position of the currently rendering camera.
	// All cameras, including shadow cameras will affect this value.
	static Vector3d vCameraPos;

private:
	// Comparison
	static void UpdateLightList ( void );

	// Light lists
	static bool bListReset;
	static std::vector<CLight*> lightList;
	static std::vector<CLight*> activeLightList;
	//static vector<CLight*> activeLightList;
	static std::priority_queue<CLight*, std::vector<CLight*>, CLightComparison> activeLightQueue;
	
	//char previousLightIndex;

};

#endif