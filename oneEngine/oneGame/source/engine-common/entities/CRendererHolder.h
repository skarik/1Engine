
#ifndef _C_RENDERER_HOLDER_H_
#define _C_RENDERER_HOLDER_H_

#include "engine/behavior/CGameBehavior.h"
#include "CRenderLogicHolder.h"
#include "CRenderCameraHolder.h"

class RrRenderObject;

class CRendererHolder : public CGameBehavior
{

public:
	ENGCOM_API explicit			CRendererHolder ( RrRenderObject* renderer );
	ENGCOM_API					~CRendererHolder ( void );
	ENGCOM_API void				Update ( void );

	ENGCOM_API RrRenderObject*	GetRenderer ( void );
private:
	RrRenderObject*	m_renderer;
};

#endif