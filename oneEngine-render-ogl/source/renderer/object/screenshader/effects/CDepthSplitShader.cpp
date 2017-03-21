
#include "CDepthSplitShader.h"
#include "renderer/camera/CCamera.h"
#include "renderer/material/glMaterial.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"


CDepthSplitShader::CDepthSplitShader ( void )
	: CScreenShader()
{
	renderSettings.renderHints = RL_WORLD;

	glMaterial *shaderMaterial = new glMaterial;
	shaderMaterial->setTexture( TEX_SLOT2, new CTexture( "textures/hud/overlay_goggles_warp.jpg" ) );
	shaderMaterial->m_isScreenShader = true;
	shaderMaterial->passinfo.push_back( glPass() );
	shaderMaterial->passinfo[0].shader = new glShader( "shaders/screen/depth_split.glsl" );
	shaderMaterial->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
	shaderMaterial->passinfo[0].m_face_mode = Renderer::FM_FRONTANDBACK;
	shaderMaterial->removeReference();
	SetMaterial( shaderMaterial );
}

void CDepthSplitShader::DrawOutput ( void )
{
	GL_ACCESS GLd_ACCESS
	// Generate sum shit
	CRenderTexture* s_buf = GL.GetMainScreenBuffer();
	{
		// Draw screen with given material
		m_material->setTexture( TEX_SLOT0, s_buf );
		m_material->setSampler( TEX_SLOT1, s_buf->GetDepthSampler(), GL.Enum(Texture2D) );
		m_material->bindPass(0);
		m_material->setShaderConstants( this );
		m_material->setUniform( "zNear", CCamera::activeCamera->zNear );
		m_material->setUniform( "zFar", CCamera::activeCamera->zFar );
		m_material->setUniform( "zCenter", CCamera::activeCamera->focalDistance );
		{
			glDepthMask( false );
			glDepthFunc( GL_ALWAYS );

			GLd.DrawScreenQuad();

			glDepthFunc( GL_LEQUAL );
		}
	}
}