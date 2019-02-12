
#ifndef _C_RENDERER_LOGIC_HOLDER_H_
#define _C_RENDERER_LOGIC_HOLDER_H_

#include "engine/behavior/CGameBehavior.h"

class RrLogicObject;

class CRenderLogicHolder : public CGameBehavior
{

public:
	ENGCOM_API explicit			CRenderLogicHolder ( RrLogicObject* renderer );
	ENGCOM_API					~CRenderLogicHolder ( void );
	ENGCOM_API void				Update ( void );

	ENGCOM_API RrLogicObject*	GetRenderer ( void );
private:
	RrLogicObject*	m_renderer;
};

#endif