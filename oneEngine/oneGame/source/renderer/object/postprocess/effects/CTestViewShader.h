
#ifndef _C_TEST_VIEW_SHADER_H_
#define _C_TEST_VIEW_SHADER_H_

#include "../CScreenShader.h"

class RrRTCamera;
class RrRenderTexture;

class CTestViewShader : public CScreenShader
{
	RegisterRenderClassName( "TestViewShader" );
public:
	RENDER_API CTestViewShader ( void );
	RENDER_API ~CTestViewShader( void );

protected:

	void UpdateBuffer ( void );
	void DrawOutput ( void );

	RrRTCamera*		m_rtcam;
	RrRenderTexture*	m_rttex;

};

#endif