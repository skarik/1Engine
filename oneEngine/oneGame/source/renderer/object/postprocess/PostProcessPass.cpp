#include "PostProcessPass.h"
#include "core/system/Screen.h"
#include "core/settings/CGameSettings.h"

#include "renderer/camera/RrCamera.h"
//#include "renderer/material/RrMaterial.h"
//#include "renderer/system/glMainSystem.h"
//#include "renderer/system/glDrawing.h"

#include "renderer/object/immediate/immediate.h"

#include "gpuw/Device.h"

RrPostProcessPass::RrPostProcessPass ( void )
{
	m_buf = NULL;

	m_failrendercount = 0;
	m_readyrendercount = 0;
}
RrPostProcessPass::~RrPostProcessPass ( void )
{
	delete_safe( m_buf );
}

// CheckBuffer()
//  Checks for any change in the main source buffer
//  Default implementation copies main buffer over (well, it would, except there's no way to copy depth proper)
void RrPostProcessPass::UpdateBuffer ( void )
{
	////GL_ACCESS
	//// Generate sum shit
	//RrRenderTexture* s_buf = GL.GetMainScreenBuffer();
	//if ( s_buf == NULL ) {
	//	return;
	//}
	//if ( m_buf == NULL ) {
	//	m_buf = s_buf->GenerateCopy();
	//}
	//else if (( m_buf->GetWidth() != s_buf->GetWidth() )||( m_buf->GetHeight() != s_buf->GetHeight() ))
	//{
	//	delete m_buf;
	//	m_buf = s_buf->GenerateCopy();
	//}
}

// Prepares general uniform buffers
bool RrPostProcessPass::RenderBegin ( RrCamera* camera )
{
	//m_material->prepareShaderConstants();
	return true;
}

// Render()
//   Where the magic should be happening
bool RrPostProcessPass::Render ( RrCamera* camera )
{
	////GL_ACCESS
	//// Only render with the main scene or designated camera
	//if ( m_targetcam == NULL ) {
	//	// Skip shadow cameras, and only link to cameras that can render this shader.
	//	if ( (RrCamera::activeCamera->shadowCamera == false) && (RrCamera::activeCamera->enabledHints & renderSettings.renderHints) ) {
	//		m_targetcam = RrCamera::activeCamera; // This camera seems jiffy!
	//	}
	//}
	//if ( m_targetcam == NULL ) {
	//	return true;
	//}
	//if ( m_targetcam != RrCamera::activeCamera )
	//{
	//	// We can't check any options on m_targetcam, since it might refer to a deleted camera.
	//	m_failrendercount += 1;	// If it can't render, though, then reset the target camera
	//	if ( m_failrendercount > (short)(RrCamera::vCameraList.size()+2) )
	//	{
	//		m_failrendercount = 0;
	//		m_targetcam = NULL;
	//		m_readyrendercount = 0;
	//	}
	//	// If the camera is NULL, try to render again to look for another camera.
	//	if ( m_targetcam == NULL ) {
	//		return Render(pass);
	//	}
	//	// If the target camera is still lame, skip this shader for now.
	//	else if ( m_targetcam != RrCamera::activeCamera ) {
	//		return true;
	//	}
	//}

	////transform.world.position = RrCamera::activeCamera->transform.position;

	////GL.CheckError();

	//// Generate sum shit
	//RrRenderTexture* s_buf = GL.GetMainScreenBuffer();
	//if ( s_buf == NULL ) {
	//	m_readyrendercount = 0;
	//	return true;
	//}

	//// Don't make buffers until completely ready.
	//if ( m_readyrendercount < (short)(RrCamera::vCameraList.size()+2) ) {
	//	m_readyrendercount += 1;
	//	return true;
	//}
	//else {
	//	m_failrendercount = 0;
	//	UpdateBuffer();
	//}

	//GL.CheckError();

	//// Change the projection to orthographic
	//{
	//	//GL.beginOrtho(-1,-1,2,2,-1,1,false);
	//	core::math::Cubic::FromPosition( Vector3f(-1,-1,-1), Vector3f(+1,+1,+1) );
	//}

	//// Predraw output
	//PredrawOutput();

	//GL.CheckError();

	//GL.setupViewport(0,0,Screen::Info.width,Screen::Info.height);
	//
	//// Start drawing to m_buf
	//m_buf->BindBuffer();
	//// Draw output
	//DrawOutput();
	//m_buf->UnbindBuffer();
	//GL.CheckError();

	//// Copy what's in m_buf over
	//CopyResult();
	//GL.CheckError();
	//
	//// Get the old projection matrix
	//{
	//	//GL.endOrtho();
	//	//GL.setupViewport(0,0,Screen::Info.width,Screen::Info.height); //Only needed if interrupting main render routine (which shouldn't be done)
	//}

	// Return success
	return true;
}

// Prepares general uniform buffers
bool RrPostProcessPass::RenderEnd ( RrCamera* camera )
{
	//m_material->prepareShaderConstants();
	return true;
}

// Draws the output quad to m_buf
void RrPostProcessPass::DrawOutput ( void )
{
	////GL_ACCESS GLd_ACCESS
	//// Generate sum shit
	//RrRenderTexture* s_buf = GL.GetMainScreenBuffer();
	//if ( s_buf == NULL )
	//	return;
	//{
	//	// Draw screen with given material
	//	m_material->setTexture( TEX_SLOT0, s_buf );
	//	m_material->bindPass(0);
	//	//m_material->setShaderConstants( this );
	//	{
	//		glDepthMask( false );
	//		glDepthFunc( GL_ALWAYS );
	//			GLd.DrawScreenQuad(m_material);
	//		glDepthFunc( GL_LEQUAL );
	//	}
	//}
}

// Copies what's in m_buf to s_buf, but only drawing the color
void RrPostProcessPass::CopyResult ( void )
{
	//gpu::GraphicsContext* gfx = gpu::getDevice()->getContext();

	////GL_ACCESS GLd_ACCESS
	//// Generate sum shit
	//RrRenderTexture* s_buf = GL.GetMainScreenBuffer();
	//if ( s_buf == NULL )
	//	return;

	//// Start drawing to s_buf
	//if ( CGameSettings::Active()->b_ro_Enable30Blit )
	//{
	//	/*glBindFramebuffer( GL_READ_FRAMEBUFFER, m_buf->GetRTInfo().findex );
	//	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, s_buf->GetRTInfo().findex );
	//	glBlitFramebuffer(
	//		0,0,Screen::Info.width,Screen::Info.height,
	//		0,0,Screen::Info.width,Screen::Info.height,
	//		GL_COLOR_BUFFER_BIT, GL_NEAREST );*/

	//	gpu::BlitTarget l_source;
	//	l_source.renderTarget = &m_buf->GetRenderTarget();
	//	l_source.target = gpu::kRenderTargetSlotColor0;
	//	l_source.rect = gpu::Rect2(0, 0, Screen::Info.width, Screen::Info.height);

	//	gpu::BlitTarget l_target;
	//	l_target.renderTarget = &s_buf->GetRenderTarget();
	//	l_target.target = gpu::kRenderTargetSlotColor0;
	//	l_target.rect = gpu::Rect2(0, 0, Screen::Info.width, Screen::Info.height);

	//	gfx->blit(l_source, l_target);
	//}
	//else
	//{
	//	/*s_buf->BindBuffer();
	//	RrMaterial::Copy->setTexture( TEX_SLOT0, m_buf );
	//	RrMaterial::Copy->bindPass(0);
	//	
	//	glDepthMask( false );
	//	glDepthFunc( GL_ALWAYS );
	//		GLd.DrawScreenQuad(RrMaterial::Copy);
	//	glDepthFunc( GL_LEQUAL );
	//	s_buf->UnbindBuffer();*/

	//	// Render the current result to the screen
	//	gfx->setRenderTarget(s_buf->getRenderTarget());
	//	gfx->setViewport(0, 0, Screen::Info.width, Screen::Info.height);
	//	{
	//		gpu::DepthStencilState ds;
	//		ds.depthTestEnabled   = false;
	//		ds.depthWriteEnabled  = false;
	//		ds.stencilTestEnabled = false;
	//		ds.stencilWriteMask   = 0x00;
	//		gfx->setDepthStencilState(ds);

	//		gpu::BlendState bs;
	//		bs.enable = false;
	//		bs.src = gpu::kBlendModeOne;
	//		bs.dst = gpu::kBlendModeZero;
	//		bs.srcAlpha = gpu::kBlendModeOne;
	//		bs.dstAlpha = gpu::kBlendModeZero;
	//		gfx->setBlendState(bs);

	//		gfx->setPipeline(&pipelinePasses->m_pipelineScreenQuadCopy);
	//		gfx->setVertexBuffer(0, &pipelinePasses->m_vbufScreenQuad, 0); // see RrPipelinePasses.cpp
	//		gfx->setVertexBuffer(1, &pipelinePasses->m_vbufScreenQuad, 0); // there are two binding slots defined with different stride
	//		gfx->setShaderSamplerAuto(gpu::kShaderStagePs, 0,
	//			m_buf->getRenderTarget()->getAttachment(gpu::kRenderTargetSlotColor0));
	//		gfx->draw(4, 0);
	//	}

	//}
}