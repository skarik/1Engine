#ifndef RENDERER_RENDER_TARGET_CAMERA_H_
#define RENDERER_RENDER_TARGET_CAMERA_H_

#include "RrCamera.h"
//#include "RrRenderTexture.h"
class RrRenderTexture;

class RrRTCamera : public RrCamera
{
	CameraType( kCameraClassRTNormal );
	//ClassName( "CameraRT" );
public:
	// Constructor/destructor
	explicit RrRTCamera (
		RrRenderTexture*	targetTexture	= NULL,
		Real			renderFramerate	= 30.0f,
		bool			autoRender		= false
		);
	~RrRTCamera ( void );

	// Update
	void LateUpdate ( void ) override;

	// Set Render Position + Render State
	void RenderBegin ( void ) override;
	// Reset render position + render state
	void RenderEnd ( void ) override;
	// Update camera matrices
	void UpdateMatrix ( void ) override;

	// == Public Setters ==
	void SetTarget ( RrRenderTexture* );
	void SetAutorender ( bool=true );
	void SetUpdateFPS ( Real=30.0f );

protected:
	// == Update Texture Matrix ==
	void UpdateTextureMatrix ( void );

	// == Options + Properites ==
	bool			bAutoRender;
	Real			fRenderFramerate;

	// == States ==
	Real	fRenderCounter;
};


#endif//RENDERER_RENDER_TARGET_CAMERA_H_