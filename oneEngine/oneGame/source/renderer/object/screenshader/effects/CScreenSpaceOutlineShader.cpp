
#include "CScreenSpaceOutlineShader.h"
#include "renderer/camera/RrCamera.h"
#include "renderer/material/RrMaterial.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

CScreenSpaceOutlineShader::CScreenSpaceOutlineShader ( void )
	: CScreenShader()
{
	/*vMaterials[0]->setShader( new RrShader( ".res/shaders/screen/colored_outline.glsl" ) );
	vMaterials[0]->isScreenShader = false;
	vMaterials[0]->isTransparent = true;*/

	renderSettings.renderHints = kRenderHintWorld;

	RrMaterial *shaderMaterial = new RrMaterial;
	shaderMaterial->m_isScreenShader = true;
	shaderMaterial->passinfo.push_back( RrPassForward() );
	shaderMaterial->passinfo[0].shader = new RrShader( ".res/shaders/screen/colored_outline.glsl" );
	shaderMaterial->passinfo[0].m_transparency_mode = renderer::ALPHAMODE_NONE;
	shaderMaterial->passinfo[0].m_face_mode = renderer::FM_FRONTANDBACK;
	shaderMaterial->removeReference();
	SetMaterial( shaderMaterial );
}

void CScreenSpaceOutlineShader::DrawOutput ( void )
{
	GL_ACCESS GLd_ACCESS;
	throw core::NotYetImplementedException();

	//// Generate sum shit
	//RrRenderTexture* s_buf = GL.GetMainScreenBuffer();
	//{
	//	// Draw screen with given material
	//	m_material->setSampler( TEX_SLOT0, s_buf->GetColorSampler(), GL.Enum(Texture2D) );
	//	m_material->setSampler( TEX_SLOT1, s_buf->GetDepthSampler(), GL.Enum(Texture2D) );
	//	m_material->bindPass(0);
	//	m_material->setShaderConstants( this );
	//	m_material->setUniform( "zNear", RrCamera::activeCamera->zNear );
	//	m_material->setUniform( "zFar", RrCamera::activeCamera->zFar );
	//	{
	//		glDepthMask( false );
	//		glDepthFunc( GL_ALWAYS );

	//		GLd.DrawScreenQuad();

	//		glDepthFunc( GL_LEQUAL );
	//	}
	//}
}