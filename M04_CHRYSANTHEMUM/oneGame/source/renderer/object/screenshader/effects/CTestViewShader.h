
#ifndef _C_TEST_VIEW_SHADER_H_
#define _C_TEST_VIEW_SHADER_H_

#include "../CScreenShader.h"

class CRTCamera;
class CRenderTexture;

class CTestViewShader : public CScreenShader
{
	RegisterRenderClassName( "TestViewShader" );
public:
	RENDER_API CTestViewShader ( void );
	RENDER_API ~CTestViewShader( void );

protected:

	void UpdateBuffer ( void );
	void DrawOutput ( void );

	CRTCamera*		m_rtcam;
	CRenderTexture*	m_rttex;

};

#endif