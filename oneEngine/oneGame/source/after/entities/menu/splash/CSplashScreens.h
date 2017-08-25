
#ifndef _C_SPLASH_SCREENS_H_
#define _C_SPLASH_SCREENS_H_

#include "engine/behavior/CGameBehavior.h"

class CCamera;
class CRenderPlane;
class CTexture;
class glMaterial;
class Perlin;

class CStudioSplash;

class CSplashScreens : public CGameBehavior
{
	ClassName( "CSplashScreens" );
public:
	CSplashScreens ( void );
	~CSplashScreens ( void );

	void Update ( void );

private:
	// Substate routines
	void Splash_Load ( void );
	void Splash_Havok ( void );
	void Splash_Disclaimer ( void );

private:

	enum eCurrentSplashState
	{
		SPLASH_LOAD,
		SPLASH_STUDIO,
		SPLASH_HAVOK,
		SPLASH_DISCLAIMER
	};
	eCurrentSplashState iState;

	CStudioSplash* mStudioSplash;

	ftype	timer;	// For keeping track of time

	CCamera*	pCamera;	// Pointer to scene camera
	CRenderPlane*		pGroundPlane;
	CRenderPlane*		pSplashPlane;

	CTexture*	pTexHavok;
	CTexture*	pTexDisclaimer;

	glMaterial*	pMatSplash;

	Perlin*		noise;

};

#endif