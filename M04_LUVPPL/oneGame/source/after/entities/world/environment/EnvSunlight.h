// The EnvSunlight class controls the sun flare and the fog color based on camera view dir and the position of the sun.
//  Just point, set, and shoot.

#ifndef _ENV_SUNLIGHT_H_
#define _ENV_SUNLIGHT_H_

#include "core/math/Color.h"
#include "engine/behavior/CGameBehavior.h"

class DirectionalLight;
class CBillboard;
class glMaterial;

class EnvSunlight : public CGameBehavior
{
public:
	// Constructor + Destructor
	EnvSunlight ( void );
	~EnvSunlight ( void );

	// LateUpdate (after camera has updated)
	void Update ( void ) { };
	void LateUpdate ( void );
	void PostUpdate ( void );

	// Public Member data (compatible with directionalLight)
	Color vLightDir;
	Color diffuseColor;

	// Public Member data (fog data)
	Color cFogSunColor;
	Color cFogAmbientColor;
	float fFogFalloff;

private:
	DirectionalLight* skyLight;
	CBillboard* pSunBillboard;
	CBillboard* pSunBillboardGlare;
	CBillboard* pSunBillboardForeground;
	glMaterial*	pSunMat;
	glMaterial*	pSunMatGlare;
	glMaterial* pSunMatForeground;
	float		fSunMatBrightness;
};

#endif