
#ifndef _C_RT_CAMERA_H_
#define _C_RT_CAMERA_H_

#include "CCamera.h"
//#include "CRenderTexture.h"
class CRenderTexture;

class CRTCamera : public CCamera
{
	CameraType( CAMERA_TYPE_RT );
	//ClassName( "CameraRT" );
public:
	// Constructor/destructor
	explicit CRTCamera (
		CRenderTexture*	targetTexture	= NULL,
		Real			renderFramerate	= 30.0f,
		bool			autoRender		= false
		);
	~CRTCamera ( void );

	// Update
	void LateUpdate ( void ) override;

	// Set Render Position + Render State
	void RenderSet ( void ) override;
	// Reset render position + render state
	void RenderUnset ( void ) override;
	// Update camera matrices
	void UpdateMatrix ( void ) override;

	// == Public Setters ==
	void SetTarget ( CRenderTexture* );
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


#endif