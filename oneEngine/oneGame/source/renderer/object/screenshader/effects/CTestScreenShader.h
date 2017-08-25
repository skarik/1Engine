
#ifndef _C_TEST_SCREEN_SHADER_H_
#define _C_TEST_SCREEN_SHADER_H_

#include "../CScreenShader.h"

class CTestScreenShader : public CScreenShader
{
	RegisterRenderClassName( "TestScreenShader" );
public:
	RENDER_API CTestScreenShader ( void );

protected:

	void DrawOutput ( void );

};

#endif