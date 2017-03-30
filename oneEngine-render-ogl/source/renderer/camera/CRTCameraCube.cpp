
#include "CRTCameraCube.h"
#include "renderer/state/CRenderState.h"
#include "renderer/state/Settings.h"
#include "renderer/types/ObjectSettings.h"
#include "renderer/texture/CRenderTexture.h"
#include "renderer/debug/CDebugRTInspector.h"

#include "renderer/system/glMainSystem.h"

void CRTCameraCube::RenderScene ( void )
{
	if ( !m_renderTexture ) {
		return;
	}
	int iPixelSize= m_renderTexture->GetHeight();
	
	// Bind the frame buffer
	//m_renderTexture->BindBuffer();
	CRenderTextureCube* m_cubeRT = (CRenderTextureCube*)m_renderTexture;
	m_cubeRT->m_renderPosition = transform.position;

	// Replace w/ default hints (only render shadow layer)
	//enabledHints = 0 | RL_SHADOW_COLOR;
	enabledHints = RL_WORLD;
	layerVisibility[renderer::World] = true;
	layerVisibility[renderer::Background] = true;
	layerVisibility[renderer::Secondary] = true;
	layerVisibility[renderer::Foreground] = true;


	glEnum t_renderList [6] = { // Y and Z are flipped from cube map to engine
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
		GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Z };
	Rotator t_rotatorList [6] = {
		Rotator( 0,0,0 ), Rotator( 90,90,0 ), Rotator( 0,0,-90 ),
		Rotator( 0,0,180 ), Rotator( -90,-90,0 ), Rotator( 0,0,90 ) };

	// Render all the faces
	if ( m_staggerRender ) {
		m_staggerTarget = (m_staggerTarget+1)%6;
	}
	for ( int i = ((m_staggerRender)?m_staggerTarget:0); i < ((m_staggerRender)?m_staggerTarget+1:6); ++i )
	{
		// Update viewport
		viewport.pos.x = 0;
		viewport.pos.y = 0;
		viewport.size.x = (ftype)iPixelSize;
		viewport.size.y = (ftype)iPixelSize;
		//fov = 90;
		fov = (Real)(2.0 * radtodeg( atan( iPixelSize / (iPixelSize-0.5) ) ));
		mirror_view = true;
		// Set proper rotation
		transform.rotation = t_rotatorList[i];

		// Bind the face to render to
		m_cubeRT->BindBufferFace( t_renderList[i] );

		// Clear depth
		glDepthMask( GL_TRUE );
		glClear( GL_DEPTH_BUFFER_BIT );

		//RenderSet();
		GL_ACCESS;
		CCamera::UpdateMatrix();
		CCamera::RenderSet();
		GL.pushProjection( viewTransform * projTransform );
		SceneRenderer->RenderSceneForward( enabledHints );
		GL.popProjection();
		CCamera::RenderUnset();

		// Unbind the frame buffer
		m_cubeRT->UnbindBufferFace( t_renderList[i] );
	}

	// Watch the render texture
	//debug::RTInspector->AddWatch( m_renderTexture );


	//debug::RTInspector->AddWatch( myRenderTexture );

	/*RenderSet();
	SceneRenderer->RenderScene();
	RenderUnset();*/

}
/*

// Render set
void CRTCamera::RenderSet ( void )
{
	// Set viewport percents
	if ( m_renderTexture )
	{
		unsigned int iPixelWidth;
		unsigned int iPixelHeight;

		// Grab render target sizes
		iPixelWidth	= m_renderTexture->GetWidth();
		iPixelHeight= m_renderTexture->GetHeight();

		// Update viewport
		viewport.pos.x = viewport_percent.pos.x * iPixelWidth;
		viewport.pos.y = viewport_percent.pos.y * iPixelHeight;
		viewport.size.x = viewport_percent.size.x * iPixelWidth;
		viewport.size.y = viewport_percent.size.y * iPixelHeight;

		// Bind frame buffer
		m_renderTexture->BindBuffer();
	}
	else
	{
		std::cout << "Rendering with no RT on RTCamera " << this << std::endl;
	}

	// Call the parent one
	CCamera::RenderSet();

	// Update the texture matrix
	//UpdateTextureMatrix();

	// Bind frame buffer
	//if ( myRenderTexture )
	//	myRenderTexture->BindBuffer();

}
*/