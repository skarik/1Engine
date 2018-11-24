#include "RrRTCameraCascade.h"
#include "renderer/state/RrRenderer.h"
#include "renderer/types/ObjectSettings.h"
#include "renderer/texture/RrRenderTexture.h"
//#include "renderer/system/glMainSystem.h"
//#include "renderer/material/RrMaterial.h"

//void RrRTCameraCascade::RenderScene ( void )
//{
//	if ( !m_renderTexture ) {
//		return;
//	}
//	GL_ACCESS;
//
//	int iPixelSize= m_renderTexture->GetHeight();
//	
//	// Bind the frame buffer
//	m_renderTexture->BindBuffer();
//
//	// Clear depth
//	glDepthMask( GL_TRUE );
//	glClear( GL_DEPTH_BUFFER_BIT );
//
//	// Replace w/ default hints (only render shadow layer)
//	enabledHints = 0 | kRenderHintShadowColor;
//
//	// Render all the cascades
//	Vector2f old_ortho_size = ortho_size;
//	for ( uint i = 0; i < 4; ++i )
//	{
//		// Update viewport
//		viewport.pos.x = (Real)iPixelSize*i;
//		viewport.pos.y = 0;
//		viewport.size.x = (Real)iPixelSize;
//		viewport.size.y = (Real)iPixelSize;
//
//		transform.position = m_renderPositions[i];
//
//		//if ( i == 3 )
//		{
//			RrRTCamera::UpdateMatrix();
//			m_renderMatrices[i] = textureMatrix;
//			RrCamera::RenderBegin();
//			//GL.pushProjection( viewTransform * projTransform );
//			camera_VP = viewTransform * projTransform;
//			RrMaterial::pushConstantsPerCamera();
//			SceneRenderer->RenderSceneForward( enabledHints );
//			//GL.popProjection();
//			RrCamera::RenderEnd();
//		}
//
//		// Make the ortho smaller
//		ortho_size *= 0.25f;
//	}
//	ortho_size = old_ortho_size;
//
//	// Watch the render texture
//	//debug::RTInspector->AddWatch( m_renderTexture );
//
//	// Unbind the frame buffer
//	m_renderTexture->UnbindBuffer();
//
//	//debug::RTInspector->AddWatch( myRenderTexture );
//
//	/*RenderBegin();
//	SceneRenderer->RenderScene();
//	RenderEnd();*/
//}