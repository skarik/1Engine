
#include "CTestScreenShader.h"
#include "renderer/camera/CCamera.h"
#include "renderer/material/glMaterial.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

CTestScreenShader::CTestScreenShader ( void )
	: CScreenShader()
{
	/*vMaterials[0]->setShader( new glShader( "shaders\\sstest.glsl" ) );
	vMaterials[0]->isScreenShader = false;
	vMaterials[0]->isTransparent = true;*/
	renderSettings.renderHints = RL_WORLD;

	glMaterial *shaderMaterial = new glMaterial;
	shaderMaterial->m_isScreenShader = false;
	shaderMaterial->passinfo.push_back( glPass() );
	shaderMaterial->passinfo[0].shader = new glShader( ".res/shaders/sstest.glsl" );
	shaderMaterial->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
	shaderMaterial->removeReference();
	SetMaterial( shaderMaterial );
}

void CTestScreenShader::DrawOutput ( void )
{
	GL_ACCESS GLd_ACCESS
	// Generate sum shit
	CRenderTexture* s_buf = GL.GetMainScreenBuffer();
	{
		// Draw screen with given material
		m_material->setTexture( 0, s_buf );
		//m_material->bindDepth( s_buf, "textureDepth" );
		m_material->setSampler( 1, s_buf->GetDepthSampler(), GL.Enum(Texture2D) );
		m_material->bindPass(0);
		m_material->setShaderConstants( this );
		m_material->setUniform( "zNear", CCamera::activeCamera->zNear );
		m_material->setUniform( "zFar", CCamera::activeCamera->zFar );
		{
			glDepthMask( false );
			glDepthFunc( GL_ALWAYS );

			GLd.DrawScreenQuad();

			glDepthFunc( GL_LEQUAL );
		}
		//vMaterials[0]->unbind();
	}
}