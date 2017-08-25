

#include "Plhud_TextNotifier.h"
#include "core/math/Math.h"
#include "renderer/texture/CBitmapFont.h"
#include "renderer/material/glMaterial.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

CBitmapFont*	Plhud_TextNotifier::fntNotifier = NULL;
glMaterial*		Plhud_TextNotifier::matNotifierDrawer = NULL;

// ==Constructor==
Plhud_TextNotifier::Plhud_TextNotifier ( const char* nStringToDraw, const Vector2d& renderPosition, bool drawCentered )
	: CGameBehavior(), CRenderableObject()
{
	mString = nStringToDraw;
	mDrawPos = renderPosition;
	mLerpTimer = -2.0f;
	mDrawCentered = drawCentered;

	renderSettings.renderHints = RL_WORLD;
	renderType = Renderer::V2D;
	if ( fntNotifier == NULL ) {
		fntNotifier	= new CBitmapFont ( "HVD_Comic_Serif_Pro.otf", 25, FW_BOLD );
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
Plhud_TextNotifier::~Plhud_TextNotifier ( void )
{
	//delete_safe( fntNotifier );
	//delete_safe( matNotifierDrawer );
}

void Plhud_TextNotifier::Update ( void )
{
	if ( mLerpTimer < -1.0f ) {
		mLerpTimer += Time::deltaTime * 3.7f;
		if ( mLerpTimer >= -1.0f ) {
			mLerpTimer = -1.0f;
		}
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


bool Plhud_TextNotifier::Render ( const char pass )
{
	if ( pass != 0 )
		return false;

	GL_ACCESS;
	GLd_ACCESS;

	GL.beginOrtho();
	GLd.DrawSet2DScaleMode();

	//matNotifierDrawer->bind();
	//fntNotifier->Set();
	/*	ftype xoffset = mLerpTimer;
		if ( mLerpTimer < -1.0f ) {
			xoffset = -1.0f - Math.Smoothlerp( -1.0f-mLerpTimer, 0.0f, 18.0f );
		}
		else if ( mLerpTimer > 1.0f ) {
			xoffset = 1.0f + Math.Smoothlerp( mLerpTimer-1, 0.0f, 18.0f );
		}
		xoffset *= 0.5f/15.0f;
		// Draw the text
		glColor4f( 0.0f, 0.0f, 0.0f, 1.0f );
			GLd.DrawAutoTextCentered( 0.49f+xoffset, 0.16f, mString.c_str() );
			GLd.DrawAutoTextCentered( 0.52f-xoffset, 0.17f, mString.c_str() );

		glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
			GLd.DrawAutoTextCentered( 0.48f+xoffset, 0.15f, mString.c_str() );
	*/
		// Slide in offset
		ftype xoffset = mLerpTimer;
		if ( mLerpTimer < -1.0f ) {
			xoffset = -1.0f - Math.Smoothlerp( -1.0f-mLerpTimer, 0.0f, 18.0f );
		}
		else if ( mLerpTimer > 1.0f ) {
			xoffset = 1.0f + Math.Smoothlerp( mLerpTimer-1, 0.0f, 18.0f );
		}
		xoffset *= 0.001;
		// Alpha amount
		ftype aoffset = std::min<ftype>( 1, 2-fabs(mLerpTimer) );
		aoffset = Math.Smoothlerp( aoffset, 0, 1 );

		//glColor4f( 0.0f, 0.0f, 0.0f, aoffset );
		matNotifierDrawer->m_diffuse = Color(0.0f, 0.0f, 0.0f, aoffset);
		matNotifierDrawer->bindPass(0);
		if ( mDrawCentered ) {
			GLd.DrawAutoTextCentered( mDrawPos.x - 0.005f - xoffset, mDrawPos.y - 0.004f, mString.c_str() );
			GLd.DrawAutoTextCentered( mDrawPos.x + 0.015f + xoffset, mDrawPos.y + 0.004f, mString.c_str() );
		}
		else {
			GLd.DrawAutoTextWrapped( mDrawPos.x - 0.005f - xoffset, mDrawPos.y - 0.004f, 1, mString.c_str() );
			GLd.DrawAutoTextWrapped( mDrawPos.x + 0.015f + xoffset, mDrawPos.y + 0.004f, 1, mString.c_str() );
		}
			
		//glColor4f( 1.0f, 1.0f, 1.0f, aoffset );
		matNotifierDrawer->m_diffuse = Color(1.0f, 1.0f, 1.0f, aoffset);
		matNotifierDrawer->bindPass(0);
		if ( mDrawCentered ) { 
			GLd.DrawAutoTextCentered( mDrawPos.x - xoffset*0.5f, mDrawPos.y, mString.c_str() );
		}
		else {
			GLd.DrawAutoTextWrapped( mDrawPos.x - xoffset*0.5f, mDrawPos.y, 1, mString.c_str() );
		}

	//fntNotifier->Unbind();
	//matNotifierDrawer->unbind();

	GL.endOrtho();

	// Return success
	return true;
}