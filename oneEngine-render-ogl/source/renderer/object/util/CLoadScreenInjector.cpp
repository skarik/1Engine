

#include "core/time/time.h"

#include "CLoadScreenInjector.h"
#include "renderer/texture/CBitmapFont.h"
#include "renderer/material/glMaterial.h"
#include "renderer/camera/CCamera.h"
#include "renderer/state/Settings.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

void CLoadScreenInjector::StepScreen ( void )
{
	GL_ACCESS
	// Redraw window
	//aWindow.Redraw();
	CCamera* mCamera = NULL;
	if ( CCamera::activeCamera == NULL ) {
		mCamera = new CCamera;
		mCamera->SetActive();
		//mCamera->Update();
		mCamera->LateUpdate();
	}
	Renderer::Settings.clearColor = Color(0,0,0.1,1);

	// Clear up the message loop
	//while ( COglWindow::pActive->UpdateMessages() == false ) {
	//	;
	//}
	// Have to render twice because of the double buffering
	GL.FullRedraw();
	// Update timer
	Time::Tick();
	// Render second time
	GL.FullRedraw();

	// Clear up camera used to render
	if ( mCamera ) {
		CCamera::activeCamera = NULL;
		//mCamera->RemoveReference();
		delete mCamera;
	}
}



CLoadScreenInjector::CLoadScreenInjector ( void )
	: CRenderableObject (  )
{
	renderSettings.renderHints = RL_WORLD;
	renderType	= Renderer::V2D;

	screenMaterial = new glMaterial();
	screenMaterial->m_diffuse = Color(0,0,0);
	screenMaterial->setTexture( 0, new CTexture( ".res/textures/white.jpg" ) );
	screenMaterial->passinfo.push_back( glPass() );
	screenMaterial->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
	screenMaterial->passinfo[0].m_lighting_mode	= Renderer::LI_NONE;
	screenMaterial->passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );

	SetMaterial( screenMaterial );

	fntNotifier	= new CBitmapFont ( "benegraphic.ttf", 72, FW_BOLD );

	matNotifierDrawer = new glMaterial;
	matNotifierDrawer->m_diffuse = Color( 0.4f,0.4f,0.4f );
	matNotifierDrawer->setTexture( 0, fntNotifier );
	matNotifierDrawer->passinfo.push_back( glPass() );
	matNotifierDrawer->passinfo[0].m_lighting_mode	= Renderer::LI_NONE;
	matNotifierDrawer->passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );
}

CLoadScreenInjector::~CLoadScreenInjector ( void )
{
	//screenMaterial->removeReference();
	//delete screenMaterial;
	delete fntNotifier;
	matNotifierDrawer->removeReference();
	delete matNotifierDrawer;
}

bool CLoadScreenInjector::Render ( const char pass )
{
	GL_ACCESS GLd_ACCESS
	GL.beginOrtho();
		GL.Translate( Vector3d( 0,0,41 ) );
		GLd.DrawSet2DMode( GLd.D2D_FLAT );
		GLd.DrawSet2DScaleMode( GLd.SCALE_DEFAULT );
		//screenMaterial->diffuse.alpha = 1.0;//std::min<Real>( fAlpha, 1 );
		//screenMaterial->bind();
			//glColor4f( 1,1,1,1 );
		screenMaterial->bindPass(0);
			GLd.DrawRectangleA( 0,0, 1,1 );
		//screenMaterial->unbind();

		//matNotifierDrawer->bind();
		//fntNotifier->Set();
		//	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
		matNotifierDrawer->bindPass(0);
			GLd.DrawAutoTextCentered( 0.5f+cosf(Time::currentTime*0.2f)*0.02f, 0.8f+sinf(Time::currentTime*0.5f)*0.04f, "Loading" );
		//fntNotifier->Unbind();
		//matNotifierDrawer->unbind();

	GL.endOrtho();

	return true;
}