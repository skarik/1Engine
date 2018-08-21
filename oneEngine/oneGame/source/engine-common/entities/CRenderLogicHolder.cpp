
#include "renderer/logic/RrLogicObject.h"
#include "CRendererHolder.h"

CRenderLogicHolder::CRenderLogicHolder ( RrLogicObject* renderer )
	: CGameBehavior(), m_renderer( renderer )
{
	;
}

CRenderLogicHolder::~CRenderLogicHolder ( void )
{
	if ( m_renderer ) {
		delete m_renderer;
	}
	m_renderer = NULL;
}
void CRenderLogicHolder::Update ( void )
{
	;
}

RrLogicObject*	CRenderLogicHolder::GetRenderer ( void ) {
	return m_renderer;
}