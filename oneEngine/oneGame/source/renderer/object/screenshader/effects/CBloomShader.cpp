
#include "CBloomShader.h"
//#include "CTime.h"
#include "core/system/Screen.h"
#include "core/settings/CGameSettings.h"

#include "renderer/camera/CCamera.h"
#include "renderer/material/RrMaterial.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

CBloomShader::CBloomShader ( void )
	: CScreenShader()
{
	renderSettings.renderHints = RL_WORLD;

	RrMaterial *shaderMaterial = new RrMaterial;
	shaderMaterial->m_isScreenShader = true;
	shaderMaterial->passinfo.push_back( RrPassForward() );
	shaderMaterial->passinfo[0].shader = new RrShader( ".res/shaders/screen/bloom+hurt.glsl" );
	shaderMaterial->passinfo[0].b_depthmask = false;
	shaderMaterial->removeReference();
	SetMaterial( shaderMaterial );
	
	renderType = renderer::Foreground;

	half_buf = NULL;
	buf_4th = NULL;
	buf_16th = NULL;

	m_threshold = 0.94f;
	m_gradient	= 1.40f;
	m_blur		= 0.0f;
	m_red		= 0.0f;
}
CBloomShader::~CBloomShader ( void )
{
	delete_safe( half_buf );
	delete_safe( buf_4th );
	delete_safe( buf_16th );
}


void CBloomShader::SetParameters ( const Real threshold, const Real gradient )
{
	m_threshold = threshold;
	m_gradient = gradient;
}
void CBloomShader::SetBlur ( const Real amount )
{
	m_blur = amount;
}
void CBloomShader::SetRed ( const Real amount )
{
	m_red = amount;
}


void CBloomShader::UpdateBuffer ( void )
{
	CScreenShader::UpdateBuffer();
	// Create halfsize buffer
	/*if ( half_buf == NULL )
	{
		half_buf = new CRenderTexture( RGB16F, m_buf->GetWidth()/2, m_buf->GetHeight()/2, Clamp, Clamp, Texture2D, Depth16 );
	}
	else if (( half_buf->GetWidth() != m_buf->GetWidth()/2 )||( half_buf->GetHeight() != m_buf->GetHeight()/2 ))
	{
		delete half_buf;
		half_buf = new CRenderTexture( RGB16F, m_buf->GetWidth()/2, m_buf->GetHeight()/2, Clamp, Clamp, Texture2D, Depth16 );
	}*/
	// Create other small buffers
	if ( buf_4th == NULL ) {
		buf_4th = new CRenderTexture( m_buf->GetWidth()/4, m_buf->GetHeight()/4, Clamp, Clamp, m_buf->GetInternalFormat(), DepthNone, StencilNone );
	}
	else if (( buf_4th->GetWidth() != m_buf->GetWidth()/4 )||( buf_4th->GetHeight() != m_buf->GetHeight()/4 ))
	{
		delete buf_4th;
		buf_4th = new CRenderTexture( m_buf->GetWidth()/4, m_buf->GetHeight()/4, Clamp, Clamp, m_buf->GetInternalFormat(), DepthNone, StencilNone );
	}

	/*if ( buf_16th == NULL ) {
		buf_16th = new CRenderTexture( RGB16F, m_buf->GetWidth()/16, m_buf->GetHeight()/16, Clamp, Clamp, Texture2D, Depth16 );
	}
	else if (( buf_16th->GetWidth() != m_buf->GetWidth()/16 )||( buf_16th->GetHeight() != m_buf->GetHeight()/16 ))
	{
		delete buf_16th;
		buf_16th = new CRenderTexture( RGB16F, m_buf->GetWidth()/16, m_buf->GetHeight()/16, Clamp, Clamp, Texture2D, Depth16 );
	}*/
}

void CBloomShader::Copy ( void )
{
	GL_ACCESS GLd_ACCESS

	// Generate sum shit
	CRenderTexture* s_buf = GL.GetMainScreenBuffer();
	if ( s_buf == NULL )
		return;

	if ( CGameSettings::Active()->b_ro_Enable30Blit )
	{
		glBindFramebuffer( GL_READ_FRAMEBUFFER, s_buf->GetRTInfo().findex );

		/*glBindFramebuffer( GL_DRAW_FRAMEBUFFER, half_buf->GetRTInfo().findex );
		glBlitFramebuffer(
			0,0,Screen::Info.width,Screen::Info.height,
			0,0,Screen::Info.width/2,Screen::Info.height/2,
			GL_COLOR_BUFFER_BIT, GL_LINEAR );*/

		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, buf_4th->GetRTInfo().findex );
		glBlitFramebuffer(
			0,0,Screen::Info.width,Screen::Info.height,
			0,0,Screen::Info.width/4,Screen::Info.height/4,
			GL_COLOR_BUFFER_BIT, GL_LINEAR );

		/*glBindFramebuffer( GL_DRAW_FRAMEBUFFER, buf_16th->GetRTInfo().findex );
		glBlitFramebuffer(
			0,0,Screen::Info.width,Screen::Info.height,
			0,0,Screen::Info.width/16,Screen::Info.height/16,
			GL_COLOR_BUFFER_BIT, GL_LINEAR );*/
	}
	else
	{	
		RrMaterial::Copy->setTexture( TEX_MAIN, s_buf );
		RrMaterial::Copy->bindPass( 0 );

		glDepthMask( false );
		glDepthFunc( GL_ALWAYS );

		// Start drawing to half_buf
		//half_buf->BindBuffer();
		//	GL.setupViewport(0,0,Screen::Info.width/2,Screen::Info.height/2);
		//	GL.DrawScreenQuad();
		//half_buf->UnbindBuffer();

		buf_4th->BindBuffer();
			GL.setupViewport(0,0,Screen::Info.width/4,Screen::Info.height/4);
			GLd.DrawScreenQuad();
		buf_4th->UnbindBuffer();

		//buf_16th->BindBuffer();
		//	GL.setupViewport(0,0,Screen::Info.width/16,Screen::Info.height/16);
		//	GL.DrawScreenQuad();
		//buf_16th->UnbindBuffer();

		GL.setupViewport(0,0,Screen::Info.width,Screen::Info.height);

		glDepthFunc( GL_LEQUAL );
	}
}

void CBloomShader::PredrawOutput ( void )
{
	Copy();
}

void CBloomShader::DrawOutput ( void )
{
	GL_ACCESS GLd_ACCESS

	// Generate sum shit
	CRenderTexture* s_buf = GL.GetMainScreenBuffer();
	{
		// Copy buffer down to half size texture
		//Copy();

		// Draw screen with given material
		m_material->setTexture( TEX_SLOT0, s_buf );
		/*vMaterials[0]->bindTexture( half_buf, "textureHalfDiffuse" );
		vMaterials[0]->bindTexture( buf_4th, "texture4thDiffuse" );
		vMaterials[0]->bindTexture( buf_16th, "texture16thDiffuse" );*/
		m_material->setTexture( TEX_SLOT1, half_buf );
		m_material->setTexture( TEX_SLOT2, buf_4th );
		m_material->setTexture( TEX_SLOT3, buf_16th );
		m_material->bindPass(0);
		m_material->setShaderConstants( this );
		m_material->setUniform( "lightThreshold", m_threshold );
		m_material->setUniform( "lightGradient", m_gradient );
		m_material->setUniform( "blurAmount", m_blur );
		m_material->setUniform( "redDesaturate", m_red );
		//vMaterials[0]->setUniform( "sys_Time", Color( CTime::currentTime, CTime::currentTime/20.0f, CTime::currentTime*2.0f, CTime::currentTime*3.0f ) );
		{
			glDepthMask( false );
			glDepthFunc( GL_ALWAYS );
				GLd.DrawScreenQuad();
			glDepthFunc( GL_LEQUAL );
		}
		//vMaterials[0]->unbind();
	}
}