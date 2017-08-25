
#include "CTestViewShader.h"

//#include "DayAndNightCycle.h"

#include "renderer/camera/CRTCamera.h"

#include "core/settings/CGameSettings.h"

#include "renderer/material/RrMaterial.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

CTestViewShader::CTestViewShader ( void )
	: CScreenShader()
{
	if ( CGameSettings::Active()->i_ro_RendererMode == RENDER_MODE_DEFERRED ) {
		throw std::exception();
	}

	renderSettings.renderHints = RL_WORLD;

	RrMaterial *shaderMaterial = new RrMaterial;
	shaderMaterial->m_isScreenShader = true;
	shaderMaterial->passinfo.push_back( RrPassForward() );
	shaderMaterial->passinfo[0].shader = new RrShader( ".res/shaders/screen/viewtest.glsl" );
	shaderMaterial->passinfo[0].m_transparency_mode = renderer::ALPHAMODE_TRANSLUCENT;
	shaderMaterial->passinfo[0].m_face_mode = renderer::FM_FRONTANDBACK;
	shaderMaterial->removeReference();
	SetMaterial( shaderMaterial );

	m_rtcam = new CRTCamera;
	m_rtcam->layerVisibility[renderer::Background]	= true;
	m_rtcam->layerVisibility[renderer::Secondary]	= false;
	m_rtcam->layerVisibility[renderer::World]		= true;
	m_rtcam->layerVisibility[renderer::Foreground]	= false;
	m_rtcam->layerVisibility[renderer::V2D]			= false;
	m_rtcam->enabledHints = RL_SKYGLOW;

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
	if ( CCamera::activeCamera )
	{
		/*if ( m_rtcam->transform.GetParent() != &CCamera::activeCamera->transform ) {
			m_rtcam->transform.Get( CCamera::activeCamera->transform );
			m_rtcam->transform.SetParent( &CCamera::activeCamera->transform );
		}*/
		m_rtcam->transform = CCamera::activeCamera->transform;
		m_rtcam->fov = CCamera::activeCamera->fov;
		m_rtcam->zNear = CCamera::activeCamera->zNear*2;
		m_rtcam->zFar = CCamera::activeCamera->zFar;
	}

	CScreenShader::UpdateBuffer();

	if ( m_rttex == NULL )
	{
		m_rttex = new CRenderTexture( m_buf->GetWidth()/4, m_buf->GetHeight()/4, Clamp, Clamp, m_buf->GetInternalFormat(), Depth16, StencilNone );
	}
	else if (( m_rttex->GetWidth() != m_buf->GetWidth()/4 )||( m_rttex->GetHeight() != m_buf->GetHeight()/4 ))
	{
		delete m_rttex;
		m_rttex = new CRenderTexture( m_buf->GetWidth()/4, m_buf->GetHeight()/4, Clamp, Clamp, m_buf->GetInternalFormat(), Depth16, StencilNone );
	}

	m_rtcam->SetTarget( m_rttex );
	m_rtcam->SetAutorender( true );
}

void CTestViewShader::DrawOutput ( void )
{
	GL_ACCESS GLd_ACCESS
	// Generate sum shit
	CRenderTexture* s_buf = GL.GetMainScreenBuffer();
	{
		// Draw screen with given material
		m_material->setTexture( TEX_SLOT0, s_buf );
		m_material->setTexture( TEX_SLOT1, m_rttex );
		m_material->bindPass(0);
		m_material->setShaderConstants( this );
		//vMaterials[0]->bindDepth( s_buf, "textureDepth" );
		//vMaterials[0]->setUniform( "zNear", CCamera::activeCamera->zNear );
		//vMaterials[0]->setUniform( "zFar", CCamera::activeCamera->zFar );
		/*if ( Daycycle::DominantCycle ) {
			Vector3d pos = Daycycle::DominantCycle->GetSunPosition( CCamera::activeCamera );
			m_material->setUniform( "gm_SunScreenPosition", Vector3d(pos.x,1-pos.y,pos.z) );
			m_material->setUniform( "gm_ZoomColorStrength", std::min<Real>( 1.0f, Daycycle::DominantCycle->GetSkyobjectDiffuse().red*3 ) );
		}
		else {
			m_material->setUniform( "gm_SunScreenPosition", Vector3d(0.5f,0.75f,1.0f) );
			m_material->setUniform( "gm_ZoomColorStrength", 1.0f );
		}*/

		{
			glDepthMask( false );
			glDepthFunc( GL_ALWAYS );
			//glDisable( GL_ALPHA_TEST );

			GLd.DrawScreenQuad();

			glDepthFunc( GL_LEQUAL );
		}
		//vMaterials[0]->unbind();
	}
	/*
		// Generate sum shit
	CRenderTexture* s_buf = GL.GetMainScreenBuffer();
	{
		// Draw screen with given material
		m_material->setTexture( 0, s_buf );
		m_material->bindPass(0);
		m_material->setShaderConstants( this );
		//vMaterials[0]->bindDepth( s_buf, "textureDepth" );
		//vMaterials[0]->setUniform( "zNear", CCamera::activeCamera->zNear );
		//vMaterials[0]->setUniform( "zFar", CCamera::activeCamera->zFar );
		if ( Daycycle::DominantCycle ) {
			Vector3d pos = Daycycle::DominantCycle->GetSunPosition( CCamera::activeCamera );
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