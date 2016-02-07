
#include "renderer/camera/CCamera.h"
#include "CRenderCameraHolder.h"

CRenderCameraHolder::CRenderCameraHolder ( CCamera* renderer )
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

CCamera*	CRenderCameraHolder::GetRenderer ( void ) {
	return m_renderer;
}