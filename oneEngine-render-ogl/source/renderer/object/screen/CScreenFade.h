
#ifndef _C_SCREEN_FADE_H_
#define _C_SCREEN_FADE_H_

//#include "CRenderableObject.h"
//#include "CGameBehavior.h"
#include "renderer/logic/CLogicObject.h"
#include "renderer/object/CRenderableObject.h"

class CCamera;

class CScreenFade : public CRenderableObject, public CLogicObject
{
public:
	RENDER_API explicit		CScreenFade ( bool inbFadeIn, float infFadeTime = 1.0f, float infFadeDelay = 0.0f, Color incFadeColor = Color(0.0f,0,0) );
	RENDER_API /*Destructor*/	~CScreenFade ( void );

	RENDER_API void PreStepSynchronus ( void ) override;
	RENDER_API bool Render ( const char pass ) override;

private:
	bool		bFadeIn;	// Fade in starts the alpha at 1. Fade out starts the alpha at 0
	float		fFadeDelay;
	float		fFadeTime;
	CCamera*	pTargetCamera;
	Color		cFadeColor;

	float		fCurrentTime;
	float		fAlpha;

	RrMaterial*	screenMaterial;

};

#endif