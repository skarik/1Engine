#ifndef RENDERER_POSTPROCESS_PASS_H_
#define RENDERER_POSTPROCESS_PASS_H_

#include "../CRenderableObject.h"
#include "renderer/texture/RrRenderTexture.h"

class RrCamera;

// TODO: PostProcessPass should not be a CRenderableObject, but something hooked into cameras.
// It should be more akin to a live gameobject-type resource that can be referenced by multiple cameras.

class RrPostProcessPass : public CRenderableObject
{
	RegisterRenderClassName( "RrPostProcessPass" );
	RegisterRenderBaseClass( "RrPostProcessPass" );
public:
	// Constructor. Passing NULL or no argument will make the screen effect find the first valid camera to render in.
	RENDER_API explicit		RrPostProcessPass ( RrCamera* pCam = NULL );
	RENDER_API virtual		~RrPostProcessPass ( void );

	// Prepares general uniform buffers
	RENDER_API bool			PreRender ( rrCameraPass* cameraPass ) override;
	// Renders the screen up
	RENDER_API bool			Render ( const char pass );

	// Set the camera to apply the screen effect to
	RENDER_API void			SetTargetCamera ( RrCamera* pCam )
		{ m_targetcam = pCam; };

protected:
	// Checks the buffer for any change in the main buffer
	// Is called during rendering to make sure stuff works
	RENDER_API virtual void	UpdateBuffer ( void ); 

	// Empty function for extra things
	RENDER_API virtual void	PredrawOutput ( void ) {;}

	// Draws to m_buf
	// When using texture inputs, the Y coordinate is bottom to top (1 to 0 from the top)
	RENDER_API virtual void	DrawOutput ( void );

	// Copies what's in m_buf to s_buf, but only drawing the color
	RENDER_API virtual void	CopyResult ( void );

	RrRenderTexture* m_buf;

	//RrMaterial	m_copymat;

protected:
	RrCamera*	m_targetcam;
	short		m_failrendercount;
	short		m_readyrendercount;
};

#endif//RENDERER_POSTPROCESS_PASS_H_