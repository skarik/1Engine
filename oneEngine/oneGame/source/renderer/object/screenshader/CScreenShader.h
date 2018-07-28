
#ifndef _C_SCREEN_SHADER_H_
#define _C_SCREEN_SHADER_H_

#include "../CRenderableObject.h"
#include "renderer/texture/RrRenderTexture.h"

class RrCamera;

class CScreenShader : public CRenderableObject
{
	RegisterRenderClassName( "CScreenShader" );
	RegisterRenderBaseClass( "CScreenShader" );
public:
	// Constructor. Passing NULL or no argument will make the screen effect find the first valid camera to render in.
	RENDER_API explicit CScreenShader ( RrCamera* pCam = NULL );
	RENDER_API virtual ~CScreenShader ( void );

	// Prepares general uniform buffers
	bool PreRender ( void ) override;
	// Renders the screen up
	bool Render ( const char pass );

	// Set the camera to apply the screen effect to
	void SetTargetCamera ( RrCamera* pCam ) { m_targetcam = pCam; };

protected:
	// Checks the buffer for any change in the main buffer
	// Is called during rendering to make sure stuff works
	virtual void UpdateBuffer ( void ); 

	// Empty function for extra things
	virtual void PredrawOutput ( void ) {;}

	// Draws to m_buf
	// When using texture inputs, the Y coordinate is bottom to top (1 to 0 from the top)
	virtual void DrawOutput ( void );

	// Copies what's in m_buf to s_buf, but only drawing the color
	virtual void CopyResult ( void );

	RrRenderTexture* m_buf;

	//RrMaterial	m_copymat;

protected:
	RrCamera*	m_targetcam;
	short		m_failrendercount;
	short		m_readyrendercount;
};

#endif