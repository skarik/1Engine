
#include "renderer/logic/CLogicObject.h"
#include "CRendererHolder.h"

CRenderLogicHolder::CRenderLogicHolder ( CLogicObject* renderer )
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

CLogicObject*	CRenderLogicHolder::GetRenderer ( void ) {
	return m_renderer;
}