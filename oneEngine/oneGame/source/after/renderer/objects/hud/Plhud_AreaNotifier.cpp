
#include "Plhud_AreaNotifier.h"
#include "core/math/Math.h"
#include "renderer/texture/CBitmapFont.h"
#include "renderer/material/glMaterial.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

CBitmapFont*	Plhud_AreaNotifier::fntNotifier = NULL;
glMaterial*		Plhud_AreaNotifier::matNotifierDrawer = NULL;

// ==Constructor==
Plhud_AreaNotifier::Plhud_AreaNotifier ( const char* nStringToDraw, const char* nPretitle, const char* nPosttitle )
	: CGameBehavior(), CRenderableObject()
{
	mString = nStringToDraw;
	mPrestring = nPretitle;
	mPoststring = nPosttitle;
	mLerpTimer = -2.0f;

	renderSettings.renderHints = RL_WORLD;
	renderType = Renderer::V2D;
	if ( fntNotifier == NULL ) {
		fntNotifier	= new CBitmapFont ( "benegraphic.ttf", 72, FW_BOLD );
	}
	if ( matNotifierDrawer == NULL )
	{
		matNotifierDrawer = new glMaterial;
		matNotifierDrawer->m_diffuse = Color( 1.0f,1,1 );
		matNotifierDrawer->passinfo.push_back( glPass() );
		matNotifierDrawer->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
		matNotifierDrawer->setTexture( 0, fntNotifier );
		matNotifierDrawer->passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );
	}
	this->SetMaterial( matNotifierDrawer );
}	

// ==Destructor==
Plhud_AreaNotifier::~Plhud_AreaNotifier ( void )
{
	//delete_safe( fntNotifier );
	//delete_safe( matNotifierDrawer );
}

void Plhud_AreaNotifier::Update ( void )
{
	if ( mLerpTimer < -1.0f ) {
		mLerpTimer += Time::deltaTime * 1.5f;
	}
	else if ( mLerpTimer < 1.0f ) {
		mLerpTimer += Time::deltaTime/(0.07f * mString.length());
	}
	else {
		mLerpTimer += Time::deltaTime * 2.0f;
		if ( mLerpTimer > 2.0f ) {
			DeleteObject( this );
		}
	}
}


bool Plhud_AreaNotifier::Render ( const char pass )
{
	if ( pass != 0 )
		return false;

	GL_ACCESS;
	GLd_ACCESS;

	GL.beginOrtho();
	GLd.DrawSet2DScaleMode();

		ftype xoffset = mLerpTimer;
		if ( mLerpTimer < -1.0f ) {
			xoffset = -1.0f - Math.Smoothlerp( -1.0f-mLerpTimer, 0.0f, 18.0f );
		}
		else if ( mLerpTimer > 1.0f ) {
			xoffset = 1.0f + Math.Smoothlerp( mLerpTimer-1, 0.0f, 18.0f );
		}
		xoffset *= 0.5f/15.0f;
		// Draw the text
		//glColor4f( 0.0f, 0.0f, 0.0f, 1.0f );
		matNotifierDrawer->m_diffuse = Color(0,0,0,1);
		matNotifierDrawer->bindPass(0);
			GLd.DrawAutoTextCentered( 0.49f+xoffset, 0.16f, mString.c_str() );
			GLd.DrawAutoTextCentered( 0.52f-xoffset, 0.17f, mString.c_str() );

		//glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
		matNotifierDrawer->m_diffuse = Color(1,1,1,1);
		matNotifierDrawer->bindPass(0);
			GLd.DrawAutoTextCentered( 0.48f+xoffset, 0.15f, mString.c_str() );

		Matrix4x4 scale;
		scale.setScale(0.65f,0.65f,1.0f ); 
		GL.Transform( scale );
		//matNotifierDrawer->bindPass(0);
			GLd.DrawAutoTextCentered( 0.46f-xoffset, 0.12f, mPrestring.c_str() );
			GLd.DrawAutoTextCentered( 0.55f+xoffset, 0.18f, mPoststring.c_str() );
			

	//fntNotifier->Unbind();
	//matNotifierDrawer->unbind();

	GL.endOrtho();

	// Return success
	return true;
}