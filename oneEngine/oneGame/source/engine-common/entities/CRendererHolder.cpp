
#include "renderer/object/CRenderableObject.h"
#include "CRendererHolder.h"

CRendererHolder::CRendererHolder ( CRenderableObject* renderer )
	: CGameBehavior(), m_renderer( renderer )
{
	;
}

CRendererHolder::~CRendererHolder ( void )
{
	if ( m_renderer ) {
		delete m_renderer;
	}
	m_renderer = NULL;
}
void CRendererHolder::Update ( void )
{
	;
}

CRenderableObject*	CRendererHolder::GetRenderer ( void ) {
	return m_renderer;
}