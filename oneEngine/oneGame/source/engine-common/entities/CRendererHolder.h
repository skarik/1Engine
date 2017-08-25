
#ifndef _C_RENDERER_HOLDER_H_
#define _C_RENDERER_HOLDER_H_

#include "engine/behavior/CGameBehavior.h"
#include "CRenderLogicHolder.h"
#include "CRenderCameraHolder.h"

class CRenderableObject;

class CRendererHolder : public CGameBehavior
{

public:
	ENGCOM_API explicit			CRendererHolder ( CRenderableObject* renderer );
	ENGCOM_API					~CRendererHolder ( void );
	ENGCOM_API void				Update ( void );

	ENGCOM_API CRenderableObject*	GetRenderer ( void );
private:
	CRenderableObject*	m_renderer;
};

#endif