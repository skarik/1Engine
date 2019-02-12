
#ifndef _C_RENDERER_CAMERA_HOLDER_H_
#define _C_RENDERER_CAMERA_HOLDER_H_

#include "engine/behavior/CGameBehavior.h"

class RrCamera;

class CRenderCameraHolder : public CGameBehavior
{

public:
	ENGCOM_API explicit			CRenderCameraHolder ( RrCamera* renderer );
	ENGCOM_API					~CRenderCameraHolder ( void );
	ENGCOM_API void				Update ( void );

	ENGCOM_API RrCamera*	GetRenderer ( void );
private:
	RrCamera*	m_renderer;
};

#endif