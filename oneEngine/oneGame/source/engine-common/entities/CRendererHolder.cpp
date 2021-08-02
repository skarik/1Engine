
#include "renderer/object/RrRenderObject.h"
#include "CRendererHolder.h"

CRendererHolder::CRendererHolder ( RrRenderObject* renderer )
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

RrRenderObject*	CRendererHolder::GetRenderer ( void ) {
	return m_renderer;
}