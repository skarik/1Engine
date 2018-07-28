
// RrRTCameraCube
// Specialized render method to render the scene multiple times at different viewports to the same camera cube.

#ifndef RENDERER_RENDER_TARGET_CAMERA_CUBE_H_
#define RENDERER_RENDER_TARGET_CAMERA_CUBE_H_

#include "RrRTCamera.h"
#include "renderer/texture/RrRenderTextureCube.h"

class RrRTCameraCube : public RrRTCamera
{
	CameraType( kCameraClassRTCube );
	//ClassName( "CameraRTCube" );
public:
	// Constructor/destructor
	explicit		RrRTCameraCube (
		RrRenderTextureCube*	targetTexture	= NULL,
		Real			renderFramerate	= 30.0f,
		bool			autoRender		= true,
		bool			staggerRender	= true
		) : RrRTCamera( targetTexture, renderFramerate, autoRender ), m_staggerRender(staggerRender), m_staggerTarget(0)
	{
		;//m_type = CAMERA_TYPE_RT_CUBE;
	}

	// ======
	// === Rendering Queries ===
	// ======
	
	// == Main Render Routine ==
	void			RenderScene ( void ) override;	// only one needing editing

	bool	m_staggerRender;
	uchar	m_staggerTarget;
};

#endif//RENDERER_RENDER_TARGET_CAMERA_CUBE_H_