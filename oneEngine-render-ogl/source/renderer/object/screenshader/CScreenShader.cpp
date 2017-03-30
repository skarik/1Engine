
#include "CScreenShader.h"
#include "core/system/Screen.h"
#include "core/settings/CGameSettings.h"

#include "renderer/camera/CCamera.h"
#include "renderer/material/RrMaterial.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

CScreenShader::CScreenShader ( CCamera* pCam )
	: CRenderableObject(), m_targetcam( pCam )
{
	m_buf = NULL;

	m_failrendercount = 0;
	m_readyrendercount = 0;
}
CScreenShader::~CScreenShader ( void )
{
	delete_safe( m_buf );
}

// CheckBuffer()
//  Checks for any change in the main source buffer
//  Default implementation copies main buffer over (well, it would, except there's no way to copy depth proper)
void CScreenShader::UpdateBuffer ( void )
{
	GL_ACCESS
	// Generate sum shit
	CRenderTexture* s_buf = GL.GetMainScreenBuffer();
	if ( s_buf == NULL ) {
		return;
	}
	if ( m_buf == NULL ) {
		m_buf = s_buf->GenerateCopy();
	}
	else if (( m_buf->GetWidth() != s_buf->GetWidth() )||( m_buf->GetHeight() != s_buf->GetHeight() ))
	{
		delete m_buf;
		m_buf = s_buf->GenerateCopy();
	}
}

// Render()
//   Where the magic should be happening
bool CScreenShader::Render ( const char pass )
{
	GL_ACCESS
	// Only render with the main scene or designated camera
	if ( m_targetcam == NULL ) {
		// Skip shadow cameras, and only link to cameras that can render this shader.
		if ( (CCamera::activeCamera->shadowCamera == false) && (CCamera::activeCamera->enabledHints & renderSettings.renderHints) ) {
			m_targetcam = CCamera::activeCamera; // This camera seems jiffy!
		}
	}
	if ( m_targetcam == NULL ) {
		return true;
	}
	if ( m_targetcam != CCamera::activeCamera )
	{
		// We can't check any options on m_targetcam, since it might refer to a deleted camera.
		m_failrendercount += 1;	// If it can't render, though, then reset the target camera
		if ( m_failrendercount > (short)(CCamera::vCameraList.size()+2) )
		{
			m_failrendercount = 0;
			m_targetcam = NULL;
			m_readyrendercount = 0;
		}
		// If the camera is NULL, try to render again to look for another camera.
		if ( m_targetcam == NULL ) {
			return Render(pass);
		}
		// If the target camera is still lame, skip this shader for now.
		else if ( m_targetcam != CCamera::activeCamera ) {
			return true;
		}
	}

	transform.world.position = CCamera::activeCamera->transform.position;

	GL.CheckError();

	// Generate sum shit
	CRenderTexture* s_buf = GL.GetMainScreenBuffer();
	if ( s_buf == NULL ) {
		m_readyrendercount = 0;
		return true;
	}

	// Don't make buffers until completely ready.
	if ( m_readyrendercount < (short)(CCamera::vCameraList.size()+2) ) {
		m_readyrendercount += 1;
		return true;
	}
	else {
		m_failrendercount = 0;
		UpdateBuffer();
	}

	GL.CheckError();

	// Change the projection to orthographic
	{
		GL.beginOrtho(-1,-1,2,2,-1,1,false);
	}

	// Predraw output
	PredrawOutput();

	GL.CheckError();

	GL.setupViewport(0,0,Screen::Info.width,Screen::Info.height);
	
	// Start drawing to m_buf
	m_buf->BindBuffer();
	// Draw output
	DrawOutput();
	m_buf->UnbindBuffer();
	GL.CheckError();

	// Copy what's in m_buf over
	CopyResult();
	GL.CheckError();
	
	// Get the old projection matrix
	{
		GL.endOrtho();
		//GL.setupViewport(0,0,Screen::Info.width,Screen::Info.height); //Only needed if interrupting main render routine (which shouldn't be done)
	}

	// Return success
	return true;
}

// Draws the output quad to m_buf
void CScreenShader::DrawOutput ( void )
{
	GL_ACCESS GLd_ACCESS
	// Generate sum shit
	CRenderTexture* s_buf = GL.GetMainScreenBuffer();
	if ( s_buf == NULL )
		return;
	{
		// Draw screen with given material
		m_material->setTexture( TEX_SLOT0, s_buf );
		m_material->bindPass(0);
		m_material->setShaderConstants( this );
		{
			glDepthMask( false );
			glDepthFunc( GL_ALWAYS );
				GLd.DrawScreenQuad();
			glDepthFunc( GL_LEQUAL );
		}
	}
}

// Copies what's in m_buf to s_buf, but only drawing the color
void CScreenShader::CopyResult ( void )
{
	GL_ACCESS GLd_ACCESS
	// Generate sum shit
	CRenderTexture* s_buf = GL.GetMainScreenBuffer();
	if ( s_buf == NULL )
		return;

	// Start drawing to s_buf
	if ( CGameSettings::Active()->b_ro_Enable30Blit )
	{
		glBindFramebuffer( GL_READ_FRAMEBUFFER, m_buf->GetRTInfo().findex );
		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, s_buf->GetRTInfo().findex );
		glBlitFramebuffer(
			0,0,Screen::Info.width,Screen::Info.height,
			0,0,Screen::Info.width,Screen::Info.height,
			GL_COLOR_BUFFER_BIT, GL_NEAREST );
	}
	else
	{
		s_buf->BindBuffer();
		RrMaterial::Copy->setTexture( TEX_SLOT0, m_buf );
		RrMaterial::Copy->bindPass(0);
		
		glDepthMask( false );
		glDepthFunc( GL_ALWAYS );
			GLd.DrawScreenQuad();
		glDepthFunc( GL_LEQUAL );
		s_buf->UnbindBuffer();
	}
}