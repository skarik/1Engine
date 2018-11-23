
// RrRTCameraCascade
// Speciallized render method to render the scene multiple times at different viewports to the same render target.

#ifndef RENDERER_RENDER_TARGET_CAMERA_CASCADE_H_
#define RENDERER_RENDER_TARGET_CAMERA_CASCADE_H_

#include "RrRTCamera.h"

class RrRTCameraCascade : public RrRTCamera
{
	//ClassName( "CameraRTCascade" );
	CameraType( kCameraClassRTCascade );
public:
	// Constructor/destructor
	/*explicit		RrRTCameraCascade (
		RrRenderTexture*	targetTexture	= NULL,
		Real			renderFramerate	= 30.0f,
		bool			autoRender		= false
		) : RrRTCamera( targetTexture, renderFramerate, autoRender )
	{
		;//m_type = CAMERA_TYPE_RT_CASCADE;
	}*/
	//				~RrRTCameraCascade ( void );

	// ======
	// === Rendering Queries ===
	// ======
	
	// == Main Render Routine ==
	//void			RenderScene ( void ) override;	// only one needing editing

public:
	Vector3d		m_renderPositions [4];
	Matrix4x4		m_renderMatrices [4];
};

#endif//RENDERER_RENDER_TARGET_CAMERA_CASCADE_H_