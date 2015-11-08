// Expensive color filter shader.


#ifndef _C_COLOR_FILTER_SHADER_H_
#define _C_COLOR_FILTER_SHADER_H_

#include "../CScreenShader.h"

class CColorFilterShader : public CScreenShader
{
	RegisterRenderClassName( "ColorFilterShader" );
public:
	RENDER_API CColorFilterShader ( void );

	Color	m_midtone_blend;
	Color	m_highlight_blend;
	Color	m_shadow_blend;
protected:
	void DrawOutput ( void );
};

#endif//_C_COLOR_FILTER_SHADER_H_