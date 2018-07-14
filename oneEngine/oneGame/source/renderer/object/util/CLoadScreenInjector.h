#ifndef RENDERER_C_LOAD_SCREEN_INJECTOR_H_
#define RENDERER_C_LOAD_SCREEN_INJECTOR_H_

#include "renderer/object/CRenderableObject.h"

class RrFontTexture;

class CLoadScreenInjector : public CRenderableObject
{
public:
	RENDER_API explicit		CLoadScreenInjector ( void );
	RENDER_API				~CLoadScreenInjector ( void );

	//	StepScreen() : Displays the loading screen
	RENDER_API void			StepScreen ( void );
	//	setAlpha( alpha ) : Updates the alpha for the loading screen
	// Used for fading the screen out smoothly.
	RENDER_API void			setAlpha ( Real new_alpha );

	bool PreRender ( void ) override;
	bool Render ( const char pass ) override;
private:
	RrFontTexture*	m_fntNotifier;
	Real			m_currentAlpha;
};


#endif//RENDERER_C_LOAD_SCREEN_INJECTOR_H_