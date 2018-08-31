#include "core/time/time.h"
#include "CLoadScreenInjector.h"
#include "renderer/texture/RrFontTexture.h"
//#include "renderer/material/RrMaterial.h"
#include "renderer/camera/RrCamera.h"
#include "renderer/state/Settings.h"
#include "renderer/state/RrRenderer.h"
#include "renderer/material/RrPass.h"
#include "renderer/material/RrShaderProgram.h"
//#include "renderer/system/glMainSystem.h"
//#include "renderer/system/glDrawing.h"
#include "renderer/object/immediate/immediate.h"

void CLoadScreenInjector::StepScreen ( void )
{
	//GL_ACCESS;
	
	// Create camera to render with:
	RrCamera* mCamera = NULL;
	if ( RrCamera::activeCamera == NULL )
	{
		mCamera = new RrCamera;
		mCamera->SetActive();
		mCamera->LateUpdate();
	}

	CRenderableObject* l_renderList[] = {this};
	//RrRenderer::Active->RenderObjectList(mCamera, l_renderList, 1);
	RrRenderer::Active->Render();

	// Have to render twice because of the double buffering
	//GL.FullRedraw();
	// Update timer
	Time::Tick();
	// Render second time
	//GL.FullRedraw();

	// Clear up camera used to render
	if ( mCamera )
	{
		RrCamera::activeCamera = NULL;
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
	renderSettings.renderHints = 0 | kRenderHintBitmaskWorld;
	renderLayer	= renderer::kRLV2D;

	//m_fntNotifier	= new RrFontTexture ( "YanoneKaffeesatz-B.otf", 72, FW_NORMAL );
	m_fntNotifier	= RrFontTexture::Load( "YanoneKaffeesatz-B.otf", 72, kFW_Normal );
	m_fntNotifier->RemoveReference();

	m_currentAlpha	= 1.0F;

	//RrMaterial* matNotifierDrawer = new RrMaterial;
	//matNotifierDrawer->m_diffuse = Color(1.0F, 1.0F, 1.0F, 1.0F);
	//matNotifierDrawer->setTexture( TEX_MAIN, m_fntNotifier );
	//matNotifierDrawer->passinfo.push_back( RrPassForward() );
	//matNotifierDrawer->passinfo[0].shader = new RrShader( "shaders/v2d/default.glsl" );
	//matNotifierDrawer->passinfo[0].set2DCommon();
	//SetMaterial( matNotifierDrawer );
	//matNotifierDrawer->removeReference();

	RrPass notifierPass;
	notifierPass.m_type = kPassTypeForward;
	notifierPass.utilSetupAs2D();
	notifierPass.m_surface.diffuseColor = Color(1.0F, 1.0F, 1.0F, 1.0F);
	notifierPass.setTexture( TEX_MAIN, RrTexture::Load("null") );
	notifierPass.setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/v2d/default_vv.spv", "shaders/v2d/default_p.spv"}) );
	PassInitWithInput(0, &notifierPass);
}

CLoadScreenInjector::~CLoadScreenInjector ( void )
{
	;
}

bool CLoadScreenInjector::PreRender ( RrCamera* camera )
{
	//m_material->prepareShaderConstants();
	PushCbufferPerObject(XrTransform(), NULL); 
	return true;
}
bool CLoadScreenInjector::Render ( const char pass )
{
	const char* kScreenMessage = "LOADING";
	const int   kScreenMessageLen = strlen(kScreenMessage);

	rrTextBuilder2D builder (m_fntNotifier, 512);

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