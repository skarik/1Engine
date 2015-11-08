
#ifndef _C_SCREEN_SPACE_OUTLINE_SHADER_H_
#define _C_SCREEN_SPACE_OUTLINE_SHADER_H_

#include "../CScreenShader.h"

class CScreenSpaceOutlineShader : public CScreenShader
{
	RegisterRenderClassName( "ScreenSpaceOutlineShader" );
public:
	RENDER_API CScreenSpaceOutlineShader ( void );

protected:

	void DrawOutput ( void );

};

#endif//_C_SCREEN_SPACE_OUTLINE_SHADER_H_