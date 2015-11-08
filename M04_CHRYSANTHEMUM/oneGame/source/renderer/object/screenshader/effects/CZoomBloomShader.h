
#ifndef _C_ZOOM_BLOOM_SHADER_H_
#define _C_ZOOM_BLOOM_SHADER_H_

#include "../CScreenShader.h"

class CZoomBloomShader : public CScreenShader
{
	RegisterRenderClassName( "ZoomBloomShader" );
public:
	RENDER_API CZoomBloomShader ( void );

protected:

	void DrawOutput ( void );

};

#endif