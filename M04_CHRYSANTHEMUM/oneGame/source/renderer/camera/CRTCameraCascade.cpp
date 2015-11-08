
#include "CRTCameraCascade.h"
#include "renderer/state/CRenderState.h"
#include "renderer/types/ObjectSettings.h"
#include "renderer/texture/CRenderTexture.h"
#include "renderer/system/glMainSystem.h"

void CRTCameraCascade::RenderScene ( void )
{
	if ( !m_renderTexture ) {
		return;
	}
	GL_ACCESS;

	int iPixelSize= m_renderTexture->GetHeight();
	
	// Bind the frame buffer
	m_renderTexture->BindBuffer();

	// Clear depth
	glDepthMask( GL_TRUE );
	glClear( GL_DEPTH_BUFFER_BIT );

	// Replace w/ default hints (only render shadow layer)
	enabledHints = 0 | RL_SHADOW_COLOR;

	// Render all the cascades
	Vector2d old_ortho_size = ortho_size;
	for ( uint i = 0; i < 4; ++i )
	{
		// Update viewport
		viewport.pos.x = (ftype)iPixelSize*i;
		viewport.pos.y = 0;
		viewport.size.x = (ftype)iPixelSize;
		viewport.size.y = (ftype)iPixelSize;

		transform.position = m_renderPositions[i];

		//if ( i == 3 )
		{
			//CCamera::UpdateMatrix();
			CRTCamera::UpdateMatrix();
			m_renderMatrices[i] = textureMatrix;
			CCamera::RenderSet();
			GL.pushProjection( viewTransform * projTransform );
			SceneRenderer->RenderScene( enabledHints );
			GL.popProjection();
			CCamera::RenderUnset();
		}

		// Make the ortho smaller
		ortho_size *= 0.25f;
	}
	ortho_size = old_ortho_size;

	// Watch the render texture
	//Debug::RTInspector->AddWatch( m_renderTexture );

	// Unbind the frame buffer
	m_renderTexture->UnbindBuffer();

	//Debug::RTInspector->AddWatch( myRenderTexture );

	/*RenderSet();
	SceneRenderer->RenderScene();
	RenderUnset();*/
}