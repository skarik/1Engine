#include "CTestViewShader.h"

//#include "DayAndNightCycle.h"

#include "renderer/state/RrRenderer.h"
#include "renderer/camera/RrRTCamera.h"
#include "renderer/material/RrMaterial.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

CTestViewShader::CTestViewShader ( void )
	: CScreenShader()
{
	if ( RrRenderer::Active->GetRenderMode() == kRenderModeDeferred ) {
		throw std::exception();
	}

	renderSettings.renderHints = kRenderHintWorld;

	RrMaterial *shaderMaterial = new RrMaterial;
	shaderMaterial->m_isScreenShader = true;
	shaderMaterial->passinfo.push_back( RrPassForward() );
	shaderMaterial->passinfo[0].shader = new RrShader( ".res/shaders/screen/viewtest.glsl" );
	shaderMaterial->passinfo[0].m_transparency_mode = renderer::ALPHAMODE_TRANSLUCENT;
	shaderMaterial->passinfo[0].m_face_mode = renderer::FM_FRONTANDBACK;
	shaderMaterial->removeReference();
	SetMaterial( shaderMaterial );

	m_rtcam = new RrRTCamera;
	m_rtcam->layerVisibility[renderer::kRLBackground]	= true;
	m_rtcam->layerVisibility[renderer::kRLSecondary]	= false;
	m_rtcam->layerVisibility[renderer::kRLWorld]		= true;
	m_rtcam->layerVisibility[renderer::kRLForeground]	= false;
	m_rtcam->layerVisibility[renderer::kRLV2D]			= false;
	m_rtcam->enabledHints = kRenderHintSkyglow;

	m_rttex = NULL;
	m_rtcam->SetTarget( NULL );
	//m_rtcam->clearColor = true;
}

CTestViewShader::~CTestViewShader ( void )
{
	delete m_rttex;
	delete m_rtcam;
}

void CTestViewShader::UpdateBuffer ( void )
{
	// Copy over!
	if ( RrCamera::activeCamera )
	{
		/*if ( m_rtcam->transform.GetParent() != &RrCamera::activeCamera->transform ) {
			m_rtcam->transform.Get( RrCamera::activeCamera->transform );
			m_rtcam->transform.SetParent( &RrCamera::activeCamera->transform );
		}*/
		m_rtcam->transform = RrCamera::activeCamera->transform;
		m_rtcam->fov = RrCamera::activeCamera->fov;
		m_rtcam->zNear = RrCamera::activeCamera->zNear*2;
		m_rtcam->zFar = RrCamera::activeCamera->zFar;
	}

	CScreenShader::UpdateBuffer();

	if ( m_rttex == NULL )
	{
		m_rttex = new RrRenderTexture( m_buf->GetWidth()/4, m_buf->GetHeight()/4, Clamp, Clamp, m_buf->GetInternalFormat(), Depth16, StencilNone );
	}
	else if (( m_rttex->GetWidth() != m_buf->GetWidth()/4 )||( m_rttex->GetHeight() != m_buf->GetHeight()/4 ))
	{
		delete m_rttex;
		m_rttex = new RrRenderTexture( m_buf->GetWidth()/4, m_buf->GetHeight()/4, Clamp, Clamp, m_buf->GetInternalFormat(), Depth16, StencilNone );
	}

	m_rtcam->SetTarget( m_rttex );
	m_rtcam->SetAutorender( true );
}

void CTestViewShader::DrawOutput ( void )
{
	GL_ACCESS GLd_ACCESS;
	throw core::NotYetImplementedException();

	//// Generate sum shit
	//RrRenderTexture* s_buf = GL.GetMainScreenBuffer();
	//{
	//	// Draw screen with given material
	//	m_material->setTexture( TEX_SLOT0, s_buf );
	//	m_material->setTexture( TEX_SLOT1, m_rttex );
	//	m_material->bindPass(0);
	//	m_material->setShaderConstants( this );
	//	//vMaterials[0]->bindDepth( s_buf, "textureDepth" );
	//	//vMaterials[0]->setUniform( "zNear", RrCamera::activeCamera->zNear );
	//	//vMaterials[0]->setUniform( "zFar", RrCamera::activeCamera->zFar );
	//	/*if ( Daycycle::DominantCycle ) {
	//		Vector3d pos = Daycycle::DominantCycle->GetSunPosition( RrCamera::activeCamera );
	//		m_material->setUniform( "gm_SunScreenPosition", Vector3d(pos.x,1-pos.y,pos.z) );
	//		m_material->setUniform( "gm_ZoomColorStrength", std::min<Real>( 1.0f, Daycycle::DominantCycle->GetSkyobjectDiffuse().red*3 ) );
	//	}
	//	else {
	//		m_material->setUniform( "gm_SunScreenPosition", Vector3d(0.5f,0.75f,1.0f) );
	//		m_material->setUniform( "gm_ZoomColorStrength", 1.0f );
	//	}*/

	//	{
	//		glDepthMask( false );
	//		glDepthFunc( GL_ALWAYS );
	//		//glDisable( GL_ALPHA_TEST );

	//		GLd.DrawScreenQuad();

	//		glDepthFunc( GL_LEQUAL );
	//	}
	//	//vMaterials[0]->unbind();
	//}
	/*
		// Generate sum shit
	RrRenderTexture* s_buf = GL.GetMainScreenBuffer();
	{
		// Draw screen with given material
		m_material->setTexture( 0, s_buf );
		m_material->bindPass(0);
		m_material->setShaderConstants( this );
		//vMaterials[0]->bindDepth( s_buf, "textureDepth" );
		//vMaterials[0]->setUniform( "zNear", RrCamera::activeCamera->zNear );
		//vMaterials[0]->setUniform( "zFar", RrCamera::activeCamera->zFar );
		if ( Daycycle::DominantCycle ) {
			Vector3d pos = Daycycle::DominantCycle->GetSunPosition( RrCamera::activeCamera );
			m_material->setUniform( "gm_SunScreenPosition", Vector3d(pos.x,1-pos.y,pos.z) );
			m_material->setUniform( "gm_ZoomColorStrength", std::min<Real>( 1.0f, Daycycle::DominantCycle->GetSkyobjectDiffuse().red*3 ) );
		}
		else {
			m_material->setUniform( "gm_SunScreenPosition", Vector3d(0.5f,0.75f,1.0f) );
			m_material->setUniform( "gm_ZoomColorStrength", 1.0f );
		}

		{
			glDepthMask( false );
			glDepthFunc( GL_ALWAYS );
			glDisable( GL_ALPHA_TEST );

			GL.DrawScreenQuad();

			glDepthFunc( GL_LEQUAL );
		}
		//vMaterials[0]->unbind();
	}*/
}