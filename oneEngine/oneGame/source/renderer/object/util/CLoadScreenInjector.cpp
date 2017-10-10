

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
	GL_ACCESS;
	
	// Create camera to render with:
	CCamera* mCamera = NULL;
	if ( CCamera::activeCamera == NULL )
	{
		mCamera = new CCamera;
		mCamera->SetActive();
		mCamera->LateUpdate();
	}

	// Have to render twice because of the double buffering
	GL.FullRedraw();
	// Update timer
	Time::Tick();
	// Render second time
	GL.FullRedraw();

	// Clear up camera used to render
	if ( mCamera )
	{
		CCamera::activeCamera = NULL;
		delete mCamera;
	}
}

void CLoadScreenInjector::setAlpha ( Real new_alpha )
{
	m_currentAlpha = new_alpha;
}

CLoadScreenInjector::CLoadScreenInjector ( void )
	: CRenderableObject (  )
{
	renderSettings.renderHints = RL_WORLD;
	renderType	= renderer::V2D;

	m_fntNotifier	= new CBitmapFont ( "YanoneKaffeesatz-B.otf", 72, FW_NORMAL );
	m_fntNotifier->RemoveReference();

	m_currentAlpha	= 1.0F;

	RrMaterial* matNotifierDrawer = new RrMaterial;
	matNotifierDrawer->m_diffuse = Color(1.0F, 1.0F, 1.0F, 1.0F);
	matNotifierDrawer->setTexture( TEX_MAIN, m_fntNotifier );
	matNotifierDrawer->passinfo.push_back( RrPassForward() );
	matNotifierDrawer->passinfo[0].shader = new RrShader( "shaders/v2d/default.glsl" );
	matNotifierDrawer->passinfo[0].set2DCommon();
	SetMaterial( matNotifierDrawer );
	matNotifierDrawer->removeReference();
}

CLoadScreenInjector::~CLoadScreenInjector ( void )
{
	;
}

bool CLoadScreenInjector::PreRender ( void )
{
	m_material->prepareShaderConstants();
	return true;
}
bool CLoadScreenInjector::Render ( const char pass )
{
	rrTextBuilder2D builder (m_fntNotifier, 512);

	//GL_ACCESS GLd_ACCESS
	////GL.beginOrtho();
	//core::math::Cubic::FromPosition( Vector3d(0, 0, -45.0F), Vector3d((Real)Screen::Info.width, (Real)Screen::Info.height, +45.0F) );
	//	//GL.Translate( Vector3d( 0,0,41 ) );
	//	GLd.DrawSet2DMode( GLd.D2D_FLAT );
	//	GLd.DrawSet2DScaleMode( GLd.SCALE_DEFAULT );
	//	//screenMaterial->diffuse.alpha = 1.0;//std::min<Real>( fAlpha, 1 );
	//	//screenMaterial->bind();
	//		//glColor4f( 1,1,1,1 );
	//	screenMaterial->bindPass(0);
	//		GLd.DrawRectangleA( 0,0, 1,1 );
	//	//screenMaterial->unbind();

	//	//matNotifierDrawer->bind();
	//	//fntNotifier->Set();
	//	//	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
	//	matNotifierDrawer->bindPass(0);
	//	matNotifierDrawer->m_diffuse.alpha = 0.75F + sinf(Time::currentTime * 2.0F) * 0.25F;
	//	//GLd.DrawAutoTextCentered( 0.5f+cosf(Time::currentTime*0.2f)*0.02f, 0.8f+sinf(Time::currentTime*0.5f)*0.04f, "Loading" );
	//	GLd.DrawAutoTextCentered( 0.5F, 0.8F, "L  O  A  D  I  N  G" );
	//	//fntNotifier->Unbind();
	//	//matNotifierDrawer->unbind();

	////GL.endOrtho();
	const char* kScreenMessage = "LOADING";
	const int kScreenMessageLen = strlen(kScreenMessage);

	builder.addRect(
		Rect(-(Real)Screen::Info.width, -(Real)Screen::Info.width, (Real)Screen::Info.width * 3.0F, (Real)Screen::Info.width * 3.0F),
		Color(0.1F, 0.1F, 0.1F, std::max<Real>(0.0F,m_currentAlpha * 0.5F)),
		false );

	for (int i = 0; i < kScreenMessageLen; ++i)
	{
		Real alpha = std::max<Real>(0.0F,
			m_currentAlpha * 0.75F + sinf(Time::currentTime * 5.0F - i * 0.7F) * 0.25F
		);
		char str [2] = {kScreenMessage[i], 0};
		builder.addText(
			Vector2f(64 + i * ((Screen::Info.width-128)/(Real)(kScreenMessageLen - 1)), Screen::Info.height * 0.5F),
			Color(1.0F, 1.0F, 1.0F, alpha),
			str);
	};

	RrScopedMeshRenderer renderer;
	renderer.render(this, m_material, 0, builder);

	return true;
}