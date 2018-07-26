
#include "CColorFilterShader.h"
#include "renderer/camera/CCamera.h"
#include "renderer/material/RrMaterial.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

CColorFilterShader::CColorFilterShader ( void )
	: CScreenShader()
{
	renderSettings.renderHints = kRenderHintWorld;
	//vMaterials[0]->isTransparent = true;
	//vMaterials[0]->iBlendMode = RrMaterial::BM_ADD;
	RrMaterial *shaderMaterial = new RrMaterial;
	shaderMaterial->m_isScreenShader = true;
	shaderMaterial->passinfo.push_back( RrPassForward() );
	shaderMaterial->passinfo[0].shader = new RrShader( ".res/shaders/screen/colorfilter.glsl" );
	//shaderMaterial->passinfo[0].b_depthmask = false;
	shaderMaterial->removeReference();
	SetMaterial( shaderMaterial );

	//renderLayer = renderer::kRLForeground;

	m_midtone_blend		= Color( 1,1,1,-0.05 );
	m_highlight_blend	= Color( 1,1,1,0.05 ); 
	m_shadow_blend		= Color( 0.98,0.98,1.02,0.08 );
}

void CColorFilterShader::DrawOutput ( void )
{
	GL_ACCESS GLd_ACCESS;
	throw core::NotYetImplementedException();

	//// Generate sum shit
	//RrRenderTexture* s_buf = GL.GetMainScreenBuffer();
	//{
	//	// Draw screen with given material
	//	m_material->setTexture( TEX_MAIN, s_buf );
	//	m_material->bindPass(0);
	//	m_material->setShaderConstants( this );
	//	m_material->setUniform( "blendMidtone", m_midtone_blend );
	//	m_material->setUniform( "blendHighlights", m_highlight_blend );
	//	m_material->setUniform( "blendShadows", m_shadow_blend );
	//	{
	//		glDepthMask( false );
	//		glDepthFunc( GL_ALWAYS );
	//		//glDisable( GL_ALPHA_TEST );
	//		
	//		GLd.DrawScreenQuad();

	//		glDepthFunc( GL_LEQUAL );
	//	}
	//	//vMaterials[0]->unbind();
	//}
}