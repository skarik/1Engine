
#include "CZoomBloomShader.h"
//#include "COglWindow.h"
#include "renderer/material/glMaterial.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

//#include "DayAndNightCycle.h"

CZoomBloomShader::CZoomBloomShader ( void )
	: CScreenShader()
{
	renderSettings.renderHints = RL_WORLD;
	/*vMaterials[0]->setShader( new glShader( ".res/shaders/screen/zoombloom.glsl" ) );
	vMaterials[0]->isScreenShader = true;*/
	//vMaterials[0]->isTransparent = true;
	//vMaterials[0]->iBlendMode = glMaterial::BM_ADD;

	//renderType = Renderer::Foreground;

	glMaterial *shaderMaterial = new glMaterial;
	shaderMaterial->m_isScreenShader = true;
	shaderMaterial->passinfo.push_back( glPass() );
	shaderMaterial->passinfo[0].shader = new glShader( ".res/shaders/screen/zoombloom.glsl" );
	shaderMaterial->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
	shaderMaterial->passinfo[0].m_face_mode = Renderer::FM_FRONTANDBACK;
	shaderMaterial->removeReference();
	SetMaterial( shaderMaterial );
}

void CZoomBloomShader::DrawOutput ( void )
{
	GL_ACCESS GLd_ACCESS
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
		/*if ( Daycycle::DominantCycle ) {
			Vector3d pos = Daycycle::DominantCycle->GetSunPosition( CCamera::activeCamera );
			m_material->setUniform( "gm_SunScreenPosition", Vector3d(pos.x,1-pos.y,pos.z) );
			m_material->setUniform( "gm_ZoomColorStrength", std::min<ftype>( 1.0f, Daycycle::DominantCycle->GetSkyobjectDiffuse().red*3 ) );
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
}