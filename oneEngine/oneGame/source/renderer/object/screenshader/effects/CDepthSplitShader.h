
#ifndef _C_DEPTH_SPLIT_SHADER_H_
#define _C_DEPTH_SPLIT_SHADER_H_

#include "../CScreenShader.h"

class CDepthSplitShader : public CScreenShader
{
	RegisterRenderClassName( "DepthSplitShader" );
public:
	RENDER_API CDepthSplitShader ( void );

protected:

	void DrawOutput ( void );

};

#endif//_C_DEPTH_SPLIT_SHADER_H_