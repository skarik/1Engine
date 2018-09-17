#ifndef RENDERER_RENDER_TARGET_CAMERA_H_
#define RENDERER_RENDER_TARGET_CAMERA_H_

#include "RrCamera.h"

class RrRTCamera : public RrCamera
{
	CameraType( kCameraClassRTNormal );
	//ClassName( "CameraRT" );
public:
	// Constructor/destructor
	RENDER_API explicit		RrRTCamera (
		RrRenderTexture*	targetTexture	= NULL,
		Real				renderFramerate	= 30.0f,
		bool				autoRender		= false
		);
	RENDER_API				~RrRTCamera ( void );

	// Update
	void					LateUpdate ( void ) override;

	void					RenderBegin ( void ) override;
	void					RenderEnd ( void ) override;
	void					UpdateMatrix ( void ) override;

	// == Public Setters ==
	RENDER_API void			SetAutorender ( bool=true );
	RENDER_API void			SetUpdateFPS ( Real=30.0f );

protected:
	// == Update Texture Matrix ==
	void					UpdateTextureMatrix ( void );

	// == Options + Properites ==
	bool			m_autoRender;
	Real			m_renderStepTime;

	// == States ==
	Real			m_renderCounter;

	Matrix4x4		textureMatrix;
};

#endif//RENDERER_RENDER_TARGET_CAMERA_H_