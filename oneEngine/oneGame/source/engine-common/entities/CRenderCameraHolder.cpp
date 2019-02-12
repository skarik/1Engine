
#include "renderer/camera/RrCamera.h"
#include "CRenderCameraHolder.h"

CRenderCameraHolder::CRenderCameraHolder ( RrCamera* renderer )
	: CGameBehavior(), m_renderer( renderer )
{
	;
}

CRenderCameraHolder::~CRenderCameraHolder ( void )
{
	if ( m_renderer ) {
		delete m_renderer;
	}
	m_renderer = NULL;
}
void CRenderCameraHolder::Update ( void )
{
	;
}

RrCamera*	CRenderCameraHolder::GetRenderer ( void ) {
	return m_renderer;
}