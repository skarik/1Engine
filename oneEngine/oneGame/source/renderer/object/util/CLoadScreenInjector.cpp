#include "core/time/time.h"
#include "CLoadScreenInjector.h"
#include "renderer/texture/RrFontTexture.h"
//#include "renderer/material/RrMaterial.h"
#include "renderer/camera/RrCamera.h"
#include "renderer/state/Settings.h"
#include "renderer/state/RrRenderer.h"
#include "renderer/material/RrPass.h"
#include "renderer/material/RrShaderProgram.h"
#include "renderer/object/immediate/immediate.h"

void CLoadScreenInjector::StepScreen ( void )
{
	// Create camera to render with, if necessary
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
	//RrRenderer::Active->RenderScene(mCamera);

	// Update timer
	Time::Tick();

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

	// Create the font being used:
	m_fntNotifier	= RrFontTexture::Load( "YanoneKaffeesatz-B.otf", 72, kFW_Normal );

	m_currentAlpha	= 1.0F;

	// Setting up the pass to render with:
	RrPass notifierPass;
	notifierPass.m_type = kPassTypeForward;
	notifierPass.m_layer = renderer::kRenderLayerV2D;
	notifierPass.m_orderOffset = kPassOrder_PostProcess + 1; // Draw on top of everything.
	notifierPass.utilSetupAs2D();
	notifierPass.m_surface.diffuseColor = Color(1.0F, 1.0F, 1.0F, 1.0F);
	notifierPass.setTexture( TEX_MAIN, m_fntNotifier );
	notifierPass.setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/v2d/default_vv.spv", "shaders/v2d/default_p.spv"}) );
	renderer::shader::Location t_vspec[] = {renderer::shader::Location::kPosition,
											renderer::shader::Location::kUV0,
											renderer::shader::Location::kColor,
											renderer::shader::Location::kNormal};
	notifierPass.setVertexSpecificationByCommonList(t_vspec, 4);
	PassInitWithInput(0, &notifierPass);

	// Create the text builder:
	m_meshBuilder = new rrTextBuilder2D(m_fntNotifier, 512);
}

CLoadScreenInjector::~CLoadScreenInjector ( void )
{
	m_fntNotifier->RemoveReference();

	delete_safe(m_meshBuilder);

	// Mesh buffers will be freed by its destructor.
}

//	BeginRender() : Create the mesh & stream to the GPU
bool CLoadScreenInjector::BeginRender ( void )
{
	const char*		kScreenMessage = "LOADING";
	const size_t	kScreenMessageLen = strlen(kScreenMessage);

	rrTextBuilder2D builder (m_fntNotifier, 512);

	builder.addRect(
		Rect(-(Real)Screen::Info.width, -(Real)Screen::Info.width, (Real)Screen::Info.width * 3.0F, (Real)Screen::Info.width * 3.0F),
		Color(0.1F, 0.1F, 0.1F, std::max<Real>(0.0F,m_currentAlpha * 0.5F)),
		false );

	for (size_t i = 0; i < kScreenMessageLen; ++i)
	{
		Real alpha = std::max<Real>(0.0F,
			m_currentAlpha * 0.75F + sinf(Time::currentTime * 5.0F - i * 0.7F) * 0.25F
			);
		char str [2] = {kScreenMessage[i], 0};
		builder.addText(
			Vector2f(64 + i * 1.0F * ((Screen::Info.width-128)/(Real)(kScreenMessageLen - 1)), Screen::Info.height * 0.5F),
			Color(1.0F, 1.0F, 1.0F, alpha),
			str);
	};

	// Push the mesh to an rrMeshBuffer now
	m_mesh = builder.getModelData();
	m_meshBuffer.StreamMeshBuffers(&m_mesh);

	return true;
}

//	PreRender() : update cbuffers
bool CLoadScreenInjector::PreRender ( rrCameraPass* pass )
{
	// Update the alpha:
	//PassGetSurface(0).diffuseColor.w = m_currentAlpha; // Handled in vertex colors.

	// Push no transformations
	PushCbufferPerObject(XrTransform(), NULL); 
	return true;
}

//	Render() : render the previously created mesh now
bool CLoadScreenInjector::Render ( const rrRenderParams* params )
{
	gpu::GraphicsContext* gfx = gpu::getDevice()->getContext();

	gpu::Pipeline* pipeline = GetPipeline( params->pass );
	gfx->setPipeline(pipeline);
	// bind the vertex buffers
	for (int i = 0; i < renderer::shader::kVBufferSlotMaxCount; ++i)
		if (m_meshBuffer.m_bufferEnabled[i])
			gfx->setVertexBuffer(i, &m_meshBuffer.m_buffer[i], 0);
	// bind the index buffer
	gfx->setIndexBuffer(&m_meshBuffer.m_indexBuffer, gpu::kIndexFormatUnsigned16);
	// bind the cbuffers: TODO
	gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_OBJECT_MATRICES, &m_cbufPerObjectMatrices);
	gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_OBJECT_EXTENDED, &m_cbufPerObjectSurfaces[params->pass]);
	gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_CAMERA_INFORMATION, params->cbuf_perCamera);
	gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_PASS_INFORMATION, params->cbuf_perPass);
	gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_FRAME_INFORMATION, params->cbuf_perFrame);
	// draw now
	gfx->drawIndexed(m_mesh.indexNum, 0, 0);
	
	return true;
}