
#include "CColorFilterShader.h"
#include "renderer/camera/CCamera.h"
#include "renderer/material/glMaterial.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

CColorFilterShader::CColorFilterShader ( void )
	: CScreenShader()
{
	renderSettings.renderHints = RL_WORLD;
	//vMaterials[0]->isTransparent = true;
	//vMaterials[0]->iBlendMode = glMaterial::BM_ADD;
	glMaterial *shaderMaterial = new glMaterial;
	shaderMaterial->m_isScreenShader = true;
	shaderMaterial->passinfo.push_back( glPass() );
	shaderMaterial->passinfo[0].shader = new glShader( ".res/shaders/screen/colorfilter.glsl" );
	//shaderMaterial->passinfo[0].b_depthmask = false;
	shaderMaterial->removeReference();
	SetMaterial( shaderMaterial );

	//renderType = Renderer::Foreground;

	m_midtone_blend		= Color( 1,1,1,-0.05 );
	m_highlight_blend	= Color( 1,1,1,0.05 ); 
	m_shadow_blend		= Color( 0.98,0.98,1.02,0.08 );
}

void CColorFilterShader::DrawOutput ( void )
{
	GL_ACCESS GLd_ACCESS
	// Generate sum shit
	CRenderTexture* s_buf = GL.GetMainScreenBuffer();
	{
		// Draw screen with given material
		m_material->setTexture( 0, s_buf );
		m_material->bindPass(0);
		m_material->setShaderConstants( this );
		m_material->setUniform( "blendMidtone", m_midtone_blend );
		m_material->setUniform( "blendHighlights", m_highlight_blend );
		m_material->setUniform( "blendShadows", m_shadow_blend );
		{
			glDepthMask( false );
			glDepthFunc( GL_ALWAYS );
			//glDisable( GL_ALPHA_TEST );
			
			GLd.DrawScreenQuad();

			glDepthFunc( GL_LEQUAL );
		}
		//vMaterials[0]->unbind();
	}
}