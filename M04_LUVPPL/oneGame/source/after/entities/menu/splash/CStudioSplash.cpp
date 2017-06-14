
#include "CStudioSplash.h"
#include "renderer/material/glMaterial.h"
#include "renderer/texture/CTexture.h"
#include "core/system/Screen.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

CStudioSplash::CStudioSplash ( void ) : CGameBehavior(), CRenderableObject()
{
	texBase = new CTexture( ".res/textures/Logos/ehs_retro.png", Texture2D, RGBA8, 1024,1024, Clamp, Clamp, MipmapNearest );

	matBase = new glMaterial();
	matBase->setTexture( 0, texBase );
	matBase->passinfo.push_back( glPass() );
	matBase->passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );
	matBase->passinfo[0].m_face_mode = Renderer::FM_FRONTANDBACK;
	matBase->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;

	matPrim = new glMaterial();
	matPrim->setTexture( 0, new CTexture( ".res/textures/white.jpg" ) );
	matPrim->passinfo.push_back( glPass() );
	matPrim->passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );
	matPrim->passinfo[0].m_face_mode = Renderer::FM_FRONTANDBACK;
	matPrim->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;

	SetMaterial( matBase );

	renderType = Renderer::V2D;

	fader = 2;
	fadeState = 0;
}
CStudioSplash::~CStudioSplash ( void )
{
	//delete_safe( matBase );
	matPrim->removeReference();
	delete_safe( matPrim );
	delete_safe( texBase );
}

void CStudioSplash::Update ( void )
{
	switch ( fadeState )
	{
	case 0:
		fader -= Time::deltaTime*3;
		if ( fader <= 0 ) {
			fader = 0;
			fadeState = 1;
		}
		break;
	case 1:
		fader += Time::deltaTime;
		if ( fader > 2 ) {
			fader = 0;
			fadeState = 2;
		}
		break;
	case 2:
		fader += Time::deltaTime*3;
		if ( fader > 2 ) {
			fader = 1;
			fadeState = 3;
		}
		break;
	case 3:
		fader -= Time::deltaTime*3;
		break;
	}
}
bool CStudioSplash::Done ( void )
{
	return ((fadeState==3) && (fader < 0));
}

bool CStudioSplash::Render ( const char pass )
{
	GL_ACCESS;
	GLd_ACCESS;
	//GL.SetColor( 1,1,1,1 );
	GL.beginOrtho();
	GLd.DrawSet2DScaleMode( GLd.SCALE_HEIGHT );
	GLd.P_PushColor( Color(1,1,1,1) );

	if ( fadeState < 3 )
	{
		// Get the aspect
		Real aspect = (Real)texBase->GetWidth()/texBase->GetHeight();
		// Get the xoffset to draw at
		Real xoffset = (Screen::Info.height * aspect)/2;

		// Draw the bg
		matPrim->m_diffuse = Color( 0,0,0,1 );
		matPrim->bindPass(0);
		matPrim->setShaderConstants( this );
		GLd.DrawRectangle( 0,0, Screen::Info.width, Screen::Info.height );

		// Draw the logo rect
		matBase->bindPass(0);
		matBase->setShaderConstants( this );
		GLd.DrawRectangle( Screen::Info.width/2 - (int)xoffset, 0, (int)xoffset*2, Screen::Info.height );

		// Draw the lines
		matPrim->m_diffuse = Color( 0,0,0,0.1f );
		matPrim->bindPass(0);
		matPrim->setShaderConstants( this );
		for ( uint y = 0; y < texBase->GetHeight(); y += 2 )
		{
			Real pixelSize = (Screen::Info.height/(Real)texBase->GetHeight());
			//GLd.DrawRectangle( 0,(int)(y*pixelSize + 0.5f), Screen::Info.width, (int)(pixelSize/2 + 0.5f) );
			GLd.DrawRectangle( 0,(int)(y*pixelSize + 0.5f), Screen::Info.width, (int)(pixelSize + 0.5f) );
		}
	}

	// Draw overlay
	if ( fadeState != 1 ) {
		//matPrim->unbind();
		matPrim->m_diffuse = Color( 0,0,0,fader );
		matPrim->bindPass(0);
		GLd.DrawRectangle( 0,0, Screen::Info.width, Screen::Info.height );
	}

	GL.endOrtho();
	return true;
}