#include "CDeveloperConsoleUI.h"

#include "core/input/CInput.h"
#include "core/settings/CGameSettings.h"

#include "engine/utils/CDeveloperConsole.h"

#include "renderer/camera/RrCamera.h"
#include "renderer/texture/RrFontTexture.h"
#include "renderer/material/RrPass.h"
#include "renderer/material/RrShaderProgram.h"
#include "gpuw/Device.h"
#include "gpuw/Sampler.h"
#include "renderer/material/Material.h"

#include "renderer/object/immediate/immediate.h" // TODO: Rename this header.

CDeveloperConsoleUI*	ActiveConsoleUI = NULL;
CDeveloperCursor*		ActiveCursor = NULL;

CDeveloperConsoleUI::CDeveloperConsoleUI ( void )
	: RrRenderObject()
{
	ActiveConsoleUI = this;

	fntMenu	= RrFontTexture::Load( "Lekton-Bold.ttf", 16, kFW_Bold );
	fntMenu->AddReference();

	RrPass fontPass;
	fontPass.m_layer = renderer::kRenderLayerV2D;
	fontPass.m_orderOffset = kPassOrder_DebugTools;
	fontPass.m_type = kPassTypeForward;
	fontPass.m_surface.diffuseColor = Color( 1.0F, 1, 1 );
	fontPass.setTexture( TEX_MAIN, fntMenu );
	fontPass.utilSetupAs2D();
	fontPass.setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/v2d/default_vv.spv", "shaders/v2d/default_p.spv"}) );
	renderer::shader::Location t_vspecFont[] = {renderer::shader::Location::kPosition,
												renderer::shader::Location::kUV0,
												renderer::shader::Location::kColor,
												renderer::shader::Location::kNormal};
	fontPass.setVertexSpecificationByCommonList(t_vspecFont, 4);
	fontPass.m_primitiveType = gpu::kPrimitiveTopologyTriangleStrip;
	PassInitWithInput(1, &fontPass);

	RrPass shapesPass;
	shapesPass.m_layer = renderer::kRenderLayerV2D;
	fontPass.m_orderOffset = kPassOrder_DebugTools;
	shapesPass.m_type = kPassTypeForward;
	shapesPass.m_surface.diffuseColor = Color( 1.0F, 1, 1 );
	shapesPass.setTexture( TEX_MAIN, RrTexture::Load(renderer::kTextureWhite) );
	shapesPass.utilSetupAs2D();
	shapesPass.setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/v2d/default_vv.spv", "shaders/v2d/default_p.spv"}) );
	renderer::shader::Location t_vspecShap[] = {renderer::shader::Location::kPosition,
												renderer::shader::Location::kUV0,
												renderer::shader::Location::kColor,
												renderer::shader::Location::kNormal};
	shapesPass.setVertexSpecificationByCommonList(t_vspecShap, 4);
	shapesPass.m_primitiveType = gpu::kPrimitiveTopologyTriangleStrip;
	PassInitWithInput(0, &shapesPass);

	transform.world.position.z = -35;
}

CDeveloperConsoleUI::~CDeveloperConsoleUI ( void )
{
	if ( ActiveConsoleUI == this ){
		ActiveConsoleUI = NULL;
	}

	fntMenu->RemoveReference();

	m_meshBufferShapes.freeMeshBuffers();
	m_meshBufferText.freeMeshBuffers();
}

bool CDeveloperConsoleUI::BeginRender ( void )
{
	Vector2f screenSize ((Real)core::GetScreen(0).GetWidth(), (Real)core::GetScreen(0).GetHeight());
	const Real kLineHeight = (Real)fntMenu->GetFontInfo()->height + 3.0F;
	const Real kBottomMargin = 5.0F;
	const Real kLeftMargin = 5.0F;

	const Color kColorBackground = Color(0.16F, 0.16F, 0.16F, 0.80F);
	const Color kColorText = Color(0.16F, 0.32F, 0.96F, 1.00F);
	const Color kColorTextCorner = Color(1.00F, 1.00F, 1.00F, 0.32F);

	if ( RrCamera::activeCamera )
	{	// Modify console size based on render scale so it is always legible!
		screenSize *= RrCamera::activeCamera->renderScale;
	}
	core::math::Cubic screenMapping = core::math::Cubic::FromPosition(
		Vector3f(0, 0, -45.0F),
		Vector3f(screenSize, +45.0F)
	);

	rrMeshBuilder2D builder(screenMapping, 6);
	rrTextBuilder2D builder_text(fntMenu, screenMapping, 100);

	if ( engine::Console->GetIsOpen() )
	{
		// Console rect:
		builder.addRect(
			Rect( 0.0F, screenSize.y - kLineHeight - kBottomMargin * 2.0F, screenSize.x, kLineHeight + kBottomMargin * 2.0F),
			kColorBackground,
			false);

		auto l_matchingCommands = engine::Console->GetMatchingCommands();
		if ( !l_matchingCommands.empty() )
		{
			// Autocomplete rect:
			Real boxHeight = (l_matchingCommands.size() + 1) * kLineHeight + kBottomMargin * 2.0F;
			builder.addRect(
				Rect( 0.0F, screenSize.y - kLineHeight - boxHeight - kBottomMargin * 2.0F, screenSize.x, boxHeight),
				kColorBackground,
				false);
		}

		// Draw the current command:
		builder_text.addText(
			Vector2f(kLeftMargin, screenSize.y - kBottomMargin),
			kColorText,
			(">" + engine::Console->GetCommandString() + "_").c_str() );

		if ( !l_matchingCommands.empty() )
		{
			builder_text.addText(
				Vector2f(kLeftMargin, screenSize.y - kLineHeight - kBottomMargin * 2.0F - kLineHeight * l_matchingCommands.size()),
				kColorText,
				"POSSIBLE MATCHES");

			// Draw the autocomplete results:
			for ( uint i = 0; i < l_matchingCommands.size(); ++i )
			{	// Draw command list
				builder_text.addText(
					Vector2f(kLeftMargin, screenSize.y - kLineHeight - kBottomMargin * 2.0F - kLineHeight * i),
					kColorText,
					(" " + l_matchingCommands[i]).c_str() );
			}
		}
	}

	// Draw the developer string in the upper-left corner.
	builder_text.addText(
		Vector2f(4.0F, (Real)fntMenu->GetFontInfo()->height + 2.0F),
		kColorTextCorner,
		CGameSettings::Active()->sysprop_developerstring.c_str() );

	// Push both the meshes
	auto t_shapesMesh = builder.getModelData();
	if (t_shapesMesh.vertexNum > 0)
		m_meshBufferShapes.getToEdit()->StreamMeshBuffers(&t_shapesMesh);
	auto t_textMesh = builder_text.getModelData();
	if (t_textMesh.vertexNum > 0)
		m_meshBufferText.getToEdit()->StreamMeshBuffers(&t_textMesh);
	m_meshBufferShapes.incrementAfterEdit();
	m_meshBufferText.incrementAfterEdit();

	// save their vertex counts
	m_indexCountShapes = t_shapesMesh.indexNum;
	m_indexCountText = t_textMesh.indexNum;

	return true;
}

bool CDeveloperConsoleUI::PreRender ( rrCameraPass* cameraPass )
{
	PushCbufferPerObject(XrTransform(), cameraPass);
	return true;
}
bool CDeveloperConsoleUI::Render ( const rrRenderParams* params )
{
	uint16_t		t_indexCount	= (params->pass == 0) ? m_indexCountShapes : m_indexCountText;
	rrMeshBuffer	*t_meshBuffer	= (params->pass == 0) ? m_meshBufferShapes.getToRender() : m_meshBufferText.getToRender();

	// otherwise we will render the same way 3d meshes render
	{
		if ( t_indexCount == 0 || t_meshBuffer == NULL || !t_meshBuffer->m_mesh_uploaded )
			return true; // Only render when have a valid mesh and rendering enabled

		gpu::GraphicsContext* gfx = params->context_graphics;

		gpu::Pipeline* pipeline = GetPipeline( params->pass );
		gfx->setPipeline(pipeline);
		// Set up the material helper...
		renderer::Material(this, gfx, params->pass, pipeline)
			// set the depth & blend state registers
			.setDepthStencilState()
			.setRasterizerState()
			.setBlendState()
			// bind the samplers & textures
			.setTextures();
		// bind the vertex buffers
		for (int i = 0; i < renderer::shader::kVBufferSlotMaxCount; ++i)
			if (t_meshBuffer->m_bufferEnabled[i])
				gfx->setVertexBuffer(i, &t_meshBuffer->m_buffer[i], 0);
		// bind the index buffer
		gfx->setIndexBuffer(&t_meshBuffer->m_indexBuffer, gpu::kIndexFormatUnsigned16);
		// bind the cbuffers
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_OBJECT_MATRICES, &m_cbufPerObjectMatrices);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_OBJECT_EXTENDED, &m_cbufPerObjectSurfaces[params->pass]);
		gfx->setShaderCBuffer(gpu::kShaderStagePs, renderer::CBUFFER_PER_OBJECT_EXTENDED, &m_cbufPerObjectSurfaces[params->pass]);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_CAMERA_INFORMATION, params->cbuf_perCamera);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_PASS_INFORMATION, params->cbuf_perPass);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_FRAME_INFORMATION, params->cbuf_perFrame);
		// draw now
		gfx->drawIndexed(t_indexCount, 0, 0);
	}

	// Return success
	return true;
}

CDeveloperCursor::CDeveloperCursor ( void )
	: RrRenderObject()
{
	ActiveCursor = this;

	transform.world.position.z = -44;
	
	texCursor = RrTexture::Load( "textures/system/cursor.png" );
	texCursor->AddReference();
	RrPass cursorPass;
	cursorPass.m_layer = renderer::kRenderLayerV2D;
	cursorPass.m_orderOffset = kPassOrder_DebugTools + kPassOrder_Step;
	cursorPass.m_type = kPassTypeForward;
	cursorPass.m_surface.diffuseColor = Color( 1.0F, 1, 1 );
	cursorPass.setTexture( TEX_MAIN, texCursor );
	gpu::SamplerCreationDescription samplerCursor;
	samplerCursor.mipmaps = false;
	samplerCursor.magFilter = core::gfx::tex::kSamplingPoint;
	samplerCursor.minFilter = core::gfx::tex::kSamplingPoint;
	cursorPass.setSampler( TEX_MAIN, &samplerCursor );
	cursorPass.utilSetupAs2D();
	cursorPass.m_alphaMode = renderer::kAlphaModeTranslucent;
	cursorPass.m_program = RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/sys/fullbright_vv.spv", "shaders/sys/fullbright_p.spv"});
	renderer::shader::Location t_vspec[] = {renderer::shader::Location::kPosition,
											renderer::shader::Location::kUV0,
											renderer::shader::Location::kColor,
											renderer::shader::Location::kNormal};
	cursorPass.setVertexSpecificationByCommonList(t_vspec, 4);
	cursorPass.m_primitiveType = gpu::kPrimitiveTopologyTriangleStrip;
	PassInitWithInput(0, &cursorPass);

	// Build the mouse mesh:
	rrMeshBuilder2D builder(core::GetScreen(0).GetSize(), 6);
	builder.setScreenMapping(core::math::Cubic::FromPosition(Vector3f(0, 0, 0), Vector3f(1, 1, 1)));
	builder.addRect(
		Rect( 0, 0, 32, 32 ),
		Color(1.0F, 1.0F, 1.0F, 1.0F),
		false);

	// Push the mouse mesh
	auto t_Mesh = builder.getModelData();
	m_meshBuffer.InitMeshBuffers(&t_Mesh);
	// save their vertex counts
	m_indexCount = t_Mesh.indexNum;
}
CDeveloperCursor::~CDeveloperCursor ( void )
{
	if ( ActiveCursor == this ) {
		ActiveCursor = NULL;
	}
	texCursor->RemoveReference();

	m_meshBuffer.FreeMeshBuffers();
}
bool CDeveloperCursor::PreRender ( rrCameraPass* cameraPass )
{
	// Manually create transform
	XrTransform cursorTransform;
	cursorTransform.scale = Vector3f(1.0F / cameraPass->m_viewport.size.x, 1.0F / cameraPass->m_viewport.size.y, 1.0F);
	cursorTransform.position = Vector2f((Real)core::Input::MouseX() - cameraPass->m_viewport.size.x * 0.5F, (Real)core::Input::MouseY() - cameraPass->m_viewport.size.y * 0.5F);
	cursorTransform.position = cursorTransform.position.mulComponents(cursorTransform.scale * 2.0F);
	cursorTransform.position.y = -cursorTransform.position.y;

	PushCbufferPerObject(cursorTransform, NULL); // Pass NULL camera for an Identity matrix in View-Projection

	return true;
}
bool CDeveloperCursor::Render ( const rrRenderParams* params )
{
	// otherwise we will render the same way 3d meshes render
	{
		if ( m_indexCount == 0 || !m_meshBuffer.m_mesh_uploaded )
			return true; // Only render when have a valid mesh and rendering enabled

		gpu::GraphicsContext* gfx = params->context_graphics;

		gpu::Pipeline* pipeline = GetPipeline( params->pass );
		gfx->setPipeline(pipeline);
		// Set up the material helper...
		renderer::Material(this, gfx, params->pass, pipeline)
			// set the depth & blend state registers
			.setDepthStencilState()
			.setRasterizerState()
			// bind the samplers & textures
			.setBlendState()
			.setTextures();
		// bind the vertex buffers
		for (int i = 0; i < renderer::shader::kVBufferSlotMaxCount; ++i)
			if (m_meshBuffer.m_bufferEnabled[i])
				gfx->setVertexBuffer(i, &m_meshBuffer.m_buffer[i], 0);
		// bind the index buffer
		gfx->setIndexBuffer(&m_meshBuffer.m_indexBuffer, gpu::kIndexFormatUnsigned16);
		// bind the cbuffers
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_OBJECT_MATRICES, &m_cbufPerObjectMatrices);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_OBJECT_EXTENDED, &m_cbufPerObjectSurfaces[params->pass]);
		gfx->setShaderCBuffer(gpu::kShaderStagePs, renderer::CBUFFER_PER_OBJECT_EXTENDED, &m_cbufPerObjectSurfaces[params->pass]);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_CAMERA_INFORMATION, params->cbuf_perCamera);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_PASS_INFORMATION, params->cbuf_perPass);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_FRAME_INFORMATION, params->cbuf_perFrame);
		// draw now
		gfx->drawIndexed(m_indexCount, 0, 0);
	}

	// Return success
	return true;
}