
// CRTCameraCascade
// Speciallized render method to render the scene multiple times at different viewports to the same render target.

#ifndef _C_RT_CAMERA_CASCADE_H_
#define _C_RT_CAMERA_CASCADE_H_

#include "CRTCamera.h"

class CRTCameraCascade : public CRTCamera
{
	//ClassName( "CameraRTCascade" );
	CameraType( CAMERA_TYPE_RT_CASCADE );
public:
	// Constructor/destructor
	explicit		CRTCameraCascade (
		RrRenderTexture*	targetTexture	= NULL,
		Real			renderFramerate	= 30.0f,
		bool			autoRender		= false
		) : CRTCamera( targetTexture, renderFramerate, autoRender )
	{
		;//m_type = CAMERA_TYPE_RT_CASCADE;
	}
	//				~CRTCameraCascade ( void );

	// ======
	// === Rendering Queries ===
	// ======
	
	// == Main Render Routine ==
	void			RenderScene ( void ) override;	// only one needing editing

public:
	Vector3d		m_renderPositions [4];
	Matrix4x4		m_renderMatrices [4];
};

#endif//_C_RT_CAMERA_CASCADE_H_