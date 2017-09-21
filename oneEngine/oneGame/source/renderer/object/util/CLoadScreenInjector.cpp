

#include "core/time/time.h"

#include "CLoadScreenInjector.h"
#include "renderer/texture/CBitmapFont.h"
#include "renderer/material/RrMaterial.h"
#include "renderer/camera/CCamera.h"
#include "renderer/state/Settings.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"
#include "renderer/object/immediate/immediate.h"

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
	renderer::Settings.clearColor = Color(0,0,0.1,1);

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
	renderType	= renderer::V2D;

	screenMaterial = new RrMaterial();
	screenMaterial->m_diffuse = Color(0,0,0);
	screenMaterial->setTexture( TEX_MAIN, new CTexture( "textures/white.jpg" ) );
	screenMaterial->passinfo.push_back( RrPassForward() );
	screenMaterial->passinfo[0].m_transparency_mode = renderer::ALPHAMODE_TRANSLUCENT;
	screenMaterial->passinfo[0].m_lighting_mode	= renderer::LI_NONE;
	screenMaterial->passinfo[0].shader = new RrShader( "shaders/v2d/default.glsl" );
	screenMaterial->removeReference();
	SetMaterial( screenMaterial );

	//fntNotifier	= new CBitmapFont ( "benegraphic.ttf", 72, FW_BOLD );
	fntNotifier	= new CBitmapFont ( "YanoneKaffeesatz-B.otf", 72, FW_NORMAL );

	matNotifierDrawer = new RrMaterial;
	matNotifierDrawer->m_diffuse = Color( 0.4f,0.4f,0.4f );
	matNotifierDrawer->setTexture( TEX_MAIN, fntNotifier );
	matNotifierDrawer->passinfo.push_back( RrPassForward() );
	matNotifierDrawer->passinfo[0].m_lighting_mode	= renderer::LI_NONE;
	matNotifierDrawer->passinfo[0].shader = new RrShader( "shaders/v2d/default.glsl" );
}

CLoadScreenInjector::~CLoadScreenInjector ( void )
{
	matNotifierDrawer->removeReference();
	delete matNotifierDrawer;
	delete fntNotifier;
}

bool CLoadScreenInjector::PreRender ( void )
{
	//GL.Translate( Vector3d( 0,0,41 ) );
	screenMaterial->prepareShaderConstants();
	matNotifierDrawer->prepareShaderConstants();
	return true;
}
bool CLoadScreenInjector::Render ( const char pass )
{
	GL_ACCESS GLd_ACCESS
	//GL.beginOrtho();
	core::math::Cubic::FromPosition( Vector3d(0, 0, -45.0F), Vector3d((Real)Screen::Info.width, (Real)Screen::Info.height, +45.0F) );
		//GL.Translate( Vector3d( 0,0,41 ) );
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
		matNotifierDrawer->m_diffuse.alpha = 0.75F + sinf(Time::currentTime * 2.0F) * 0.25F;
		//GLd.DrawAutoTextCentered( 0.5f+cosf(Time::currentTime*0.2f)*0.02f, 0.8f+sinf(Time::currentTime*0.5f)*0.04f, "Loading" );
		GLd.DrawAutoTextCentered( 0.5F, 0.8F, "L  O  A  D  I  N  G" );
		//fntNotifier->Unbind();
		//matNotifierDrawer->unbind();

	//GL.endOrtho();

	return true;
}