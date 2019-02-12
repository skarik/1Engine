#include "RrRTCameraCube.h"
#include "renderer/state/RrRenderer.h"
#include "renderer/state/Settings.h"
#include "renderer/types/ObjectSettings.h"
#include "renderer/texture/RrRenderTexture.h"
#include "renderer/debug/RrDebugRTInspector.h"
#include "renderer/texture/RrRenderTextureCube.h"
//#include "renderer/material/RrMaterial.h"

//#include "renderer/system/glMainSystem.h"

// Constructor/destructor
RrRTCameraCube::RrRTCameraCube (
	renderer::rrInternalSettings* const targetSettings,
	Vector2i const& targetSize,
	Real renderFramerate,
	bool autoRender,
	bool staggerRender
	) : RrRTCamera( targetSettings, targetSize, renderFramerate, autoRender ),
	m_rendered(false),
	m_staggerRender(staggerRender), m_staggerTarget(0)
{
	;//m_type = CAMERA_TYPE_RT_CUBE;
}

//	PassCount() : Returns number of passes this camera will render
// Must be 1 or greater in order to render.
int RrRTCameraCube::PassCount ( void )
{
	return m_staggerRender ? 1 : 6;
}
//	PassRetrieve(array, array_size) : Writes pass information into the array given in
// Will write either PassCount() or maxPasses passes, whatever is smaller.
void RrRTCameraCube::PassRetrieve ( const rrCameraPassInput* input, rrCameraPass* passList )
{
	if (!m_staggerRender)
	{
		for (int i = 0; i < std::min<int>(6, input->m_maxPasses); ++i)
		{
			passList[i].m_bufferChain	= NULL;
			passList[i].m_passType		= kCameraRenderWorld;
			passList[i].m_viewport		= viewport;
			passList[i].m_viewTransform	= viewCubeMatrices[i];
			passList[i].m_projTransform	= projCubeMatrices[i];
			passList[i].m_viewprojTransform	= viewprojCubeMatrices[i];

			int cbuffer_index = i + input->m_bufferingIndex * 6;
			UpdateCBuffer(cbuffer_index, input->m_bufferingCount * 6, &passList[i]);
			passList[i].m_cbuffer = &m_cbuffers[cbuffer_index];
		}
	}
	else
	{
		passList[0].m_bufferChain	= NULL;
		passList[0].m_passType		= kCameraRenderWorld;
		passList[0].m_viewport		= viewport;
		passList[0].m_viewTransform	= viewCubeMatrices[m_staggerTarget];
		passList[0].m_projTransform	= projCubeMatrices[m_staggerTarget];
		passList[0].m_viewprojTransform	= viewprojCubeMatrices[m_staggerTarget];

		int cbuffer_index = input->m_bufferingIndex;
		UpdateCBuffer(cbuffer_index, input->m_bufferingCount, &passList[0]);
		passList[0].m_cbuffer = &m_cbuffers[cbuffer_index];
	}
}

//	RenderBegin() : Begins rendering, pushing the current camera params.
void RrRTCameraCube::RenderBegin ( void )
{
	RrRTCamera::RenderBegin();
}
//	RenderEnd() : Called at the end of render, cleans up any camera-specific objects.
void RrRTCameraCube::RenderEnd ( void )
{
	RrRTCamera::RenderEnd();

	m_rendered = true;
}

//	LateUpdate() : Pre-render update
// Updates active camera, viewports, and matrices.
// Called just before any sorting or rendering is done.
void RrRTCameraCube::LateUpdate ( void )
{
	if (m_staggerRender && m_rendered)
	{	// Go to next face if rendered something...
		m_staggerTarget = (m_staggerTarget + 1) % 6;
	}
	RrRTCamera::LateUpdate();
	
	// Reset rendered flag.
	m_rendered = false;
}

//	UpdateMatrix() : Updates the camera matrices.
void RrRTCameraCube::UpdateMatrix ( void ) 
{
	throw core::NotYetImplementedException();
	//
	//auto t_textureHeight = m_renderTexture->GetHeight();
	//auto t_baseRotation = transform.rotation;
	////const glEnum t_renderList [6] = { // Y and Z are flipped from cube map to engine
	////	GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
	////	GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Z
	////};
	//const Rotator t_rotatorList [6] = {
	//	Rotator( 0,0,0 ),	Rotator( 90,90,0 ),		Rotator( 0,0,-90 ),
	//	Rotator( 0,0,180 ),	Rotator( -90,-90,0 ),	Rotator( 0,0,90 )
	//};
	//
	//// Update parameters for generating the matrices needed.
	//fieldOfView = (Real)(2.0 * radtodeg( atan( t_textureHeight / (t_textureHeight-0.5) ) ));
	//mirrorView = true;

	//// Loop through all the matrices we need updated...
	//for ( int i = (m_staggerRender ? m_staggerTarget : 0); i < (m_staggerRender ? m_staggerTarget+1 : 6); ++i )
	//{
	//	transform.rotation = t_baseRotation * t_rotatorList[i];

	//	RrRTCamera::UpdateMatrix();

	//	// Save the matrices we're going to render with:
	//	viewCubeMatrices[i] = viewTransform;
	//	projCubeMatrices[i] = projTransform;
	//	viewprojCubeMatrices[i] = viewprojMatrix;
	//}

	//// Restore the rotation...
	//transform.rotation = t_baseRotation;
}

//void RrRTCameraCube::RenderScene ( void )
//{
//	if ( !m_renderTexture ) {
//		return;
//	}
//	int iPixelSize= m_renderTexture->GetHeight();
//	
//	// Bind the frame buffer
//	//m_renderTexture->BindBuffer();
//	RrRenderTextureCube* m_cubeRT = (RrRenderTextureCube*)m_renderTexture;
//	m_cubeRT->m_renderPosition = transform.position;
//
//	// Replace w/ default hints (only render shadow layer)
//	//enabledHints = 0 | kRenderHintShadowColor;
//	enabledHints = kRenderHintWorld;
//	layerVisibility[renderer::kRLWorld] = true;
//	layerVisibility[renderer::kRLBackground] = true;
//	layerVisibility[renderer::kRLSecondary] = true;
//	layerVisibility[renderer::kRLForeground] = true;
//
//
//	glEnum t_renderList [6] = { // Y and Z are flipped from cube map to engine
//		GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
//		GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Z };
//	Rotator t_rotatorList [6] = {
//		Rotator( 0,0,0 ), Rotator( 90,90,0 ), Rotator( 0,0,-90 ),
//		Rotator( 0,0,180 ), Rotator( -90,-90,0 ), Rotator( 0,0,90 ) };
//
//	// Render all the faces
//	if ( m_staggerRender ) {
//		m_staggerTarget = (m_staggerTarget+1)%6;
//	}
//	for ( int i = ((m_staggerRender)?m_staggerTarget:0); i < ((m_staggerRender)?m_staggerTarget+1:6); ++i )
//	{
//		// Update viewport
//		viewport.pos.x = 0;
//		viewport.pos.y = 0;
//		viewport.size.x = (Real)iPixelSize;
//		viewport.size.y = (Real)iPixelSize;
//		//fov = 90;
//		mirror_view = true;
//		// Set proper rotation
//		transform.rotation = t_rotatorList[i];
//
//		// Bind the face to render to
//		m_cubeRT->BindBufferFace( t_renderList[i] );
//
//		// Clear depth
//		glDepthMask( GL_TRUE );
//		glClear( GL_DEPTH_BUFFER_BIT );
//
//		//RenderBegin();
//		GL_ACCESS;
//		RrCamera::UpdateMatrix();
//		RrCamera::RenderBegin();
//		//GL.pushProjection( viewTransform * projTransform );
//		camera_VP = viewTransform * projTransform;
//		RrMaterial::pushConstantsPerCamera();
//		SceneRenderer->RenderSceneForward( enabledHints );
//		//GL.popProjection();
//		RrCamera::RenderEnd();
//
//		// Unbind the frame buffer
//		m_cubeRT->UnbindBufferFace( t_renderList[i] );
//	}
//
//	// Watch the render texture
//	//debug::RTInspector->AddWatch( m_renderTexture );
//
//
//	//debug::RTInspector->AddWatch( myRenderTexture );
//
//	/*RenderBegin();
//	SceneRenderer->RenderScene();
//	RenderEnd();*/
//
//}
///*
//
//// Render set
//void RrRTCamera::RenderBegin ( void )
//{
//	// Set viewport percents
//	if ( m_renderTexture )
//	{
//		unsigned int iPixelWidth;
//		unsigned int iPixelHeight;
//
//		// Grab render target sizes
//		iPixelWidth	= m_renderTexture->GetWidth();
//		iPixelHeight= m_renderTexture->GetHeight();
//
//		// Update viewport
//		viewport.pos.x = viewport_percent.pos.x * iPixelWidth;
//		viewport.pos.y = viewport_percent.pos.y * iPixelHeight;
//		viewport.size.x = viewport_percent.size.x * iPixelWidth;
//		viewport.size.y = viewport_percent.size.y * iPixelHeight;
//
//		// Bind frame buffer
//		m_renderTexture->BindBuffer();
//	}
//	else
//	{
//		std::cout << "Rendering with no RT on RTCamera " << this << std::endl;
//	}
//
//	// Call the parent one
//	RrCamera::RenderBegin();
//
//	// Update the texture matrix
//	//UpdateTextureMatrix();
//
//	// Bind frame buffer
//	//if ( myRenderTexture )
//	//	myRenderTexture->BindBuffer();
//
//}
//*/