
#include "CScreenSpaceOutlineShader.h"
#include "renderer/camera/CCamera.h"
#include "renderer/material/glMaterial.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

CScreenSpaceOutlineShader::CScreenSpaceOutlineShader ( void )
	: CScreenShader()
{
	/*vMaterials[0]->setShader( new glShader( ".res/shaders/screen/colored_outline.glsl" ) );
	vMaterials[0]->isScreenShader = false;
	vMaterials[0]->isTransparent = true;*/

	renderSettings.renderHints = RL_WORLD;

	glMaterial *shaderMaterial = new glMaterial;
	shaderMaterial->m_isScreenShader = true;
	shaderMaterial->passinfo.push_back( glPass() );
	shaderMaterial->passinfo[0].shader = new glShader( ".res/shaders/screen/colored_outline.glsl" );
	shaderMaterial->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_NONE;
	shaderMaterial->passinfo[0].m_face_mode = Renderer::FM_FRONTANDBACK;
	shaderMaterial->removeReference();
	SetMaterial( shaderMaterial );
}

void CScreenSpaceOutlineShader::DrawOutput ( void )
{
	GL_ACCESS GLd_ACCESS
	// Generate sum shit
	CRenderTexture* s_buf = GL.GetMainScreenBuffer();
	{
		// Draw screen with given material
		m_material->setSampler( TEX_SLOT0, s_buf->GetColorSampler(), GL.Enum(Texture2D) );
		m_material->setSampler( TEX_SLOT1, s_buf->GetDepthSampler(), GL.Enum(Texture2D) );
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
	}
}