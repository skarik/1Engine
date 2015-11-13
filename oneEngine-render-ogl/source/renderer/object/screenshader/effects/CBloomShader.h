
#ifndef _C_BLOOM_SHADER_H_
#define _C_BLOOM_SHADER_H_

#include "../CScreenShader.h"

class CBloomShader : public CScreenShader
{
	RegisterRenderClassName( "BloomShader" );
public:
	RENDER_API CBloomShader ( void );
	RENDER_API ~CBloomShader ( void );

	RENDER_API void SetParameters ( const ftype threshold, const ftype gradient );
	RENDER_API void SetBlur ( const ftype amount );
	RENDER_API void SetRed ( const ftype amount );

protected:
	// Checks the buffer for any change in the main buffer
	// Is called during rendering to make sure stuff works
	virtual void UpdateBuffer ( void ); 
		
	void DrawOutput ( void );
	
	void PredrawOutput ( void );

	void Copy ( void );

	CRenderTexture* half_buf;
	CRenderTexture* buf_4th;
	CRenderTexture* buf_16th;

	ftype m_threshold;
	ftype m_gradient;
	ftype m_blur;
	ftype m_red;

};

#endif//_C_BLOOM_SHADER_H_