
#ifndef _C_LOAD_SCREEN_INJECTOR_H_
#define _C_LOAD_SCREEN_INJECTOR_H_

#include "renderer/object/CRenderableObject.h"

class CBitmapFont;

class CLoadScreenInjector : public CRenderableObject
{

public:
	RENDER_API CLoadScreenInjector ( void );
	RENDER_API ~CLoadScreenInjector ( void );

	RENDER_API void StepScreen ( void );

	bool PreRender ( void ) override;
	bool Render ( const char pass ) override;

private:
	RrMaterial*	screenMaterial;

	CBitmapFont*	fntNotifier;
	RrMaterial*		matNotifierDrawer;
};


#endif//_C_LOAD_SCREEN_INJECTOR_H_