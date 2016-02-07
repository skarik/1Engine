
#ifndef _C_RENDERER_LOGIC_HOLDER_H_
#define _C_RENDERER_LOGIC_HOLDER_H_

#include "engine/behavior/CGameBehavior.h"

class CLogicObject;

class CRenderLogicHolder : public CGameBehavior
{

public:
	ENGCOM_API explicit			CRenderLogicHolder ( CLogicObject* renderer );
	ENGCOM_API					~CRenderLogicHolder ( void );
	ENGCOM_API void				Update ( void );

	ENGCOM_API CLogicObject*	GetRenderer ( void );
private:
	CLogicObject*	m_renderer;
};

#endif