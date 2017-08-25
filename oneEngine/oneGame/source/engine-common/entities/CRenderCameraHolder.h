
#ifndef _C_RENDERER_CAMERA_HOLDER_H_
#define _C_RENDERER_CAMERA_HOLDER_H_

#include "engine/behavior/CGameBehavior.h"

class CCamera;

class CRenderCameraHolder : public CGameBehavior
{

public:
	ENGCOM_API explicit			CRenderCameraHolder ( CCamera* renderer );
	ENGCOM_API					~CRenderCameraHolder ( void );
	ENGCOM_API void				Update ( void );

	ENGCOM_API CCamera*	GetRenderer ( void );
private:
	CCamera*	m_renderer;
};

#endif