
// CRTCameraCube
// Specialized render method to render the scene multiple times at different viewports to the same camera cube.

#ifndef _C_RT_CAMERA_CUBE_H_
#define _C_RT_CAMERA_CUBE_H_

#include "CRTCamera.h"
#include "renderer/texture/CRenderTextureCube.h"

class CRTCameraCube : public CRTCamera
{
	CameraType( CAMERA_TYPE_RT_CUBE );
	//ClassName( "CameraRTCube" );
public:
	// Constructor/destructor
	explicit		CRTCameraCube (
		CRenderTextureCube*	targetTexture	= NULL,
		Real			renderFramerate	= 30.0f,
		bool			autoRender		= true,
		bool			staggerRender	= true
		) : CRTCamera( targetTexture, renderFramerate, autoRender ), m_staggerRender(staggerRender), m_staggerTarget(0)
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

#endif//_C_RT_CAMERA_CUBE_H_