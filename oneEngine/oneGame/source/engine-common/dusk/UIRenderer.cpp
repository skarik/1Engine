#include "UIRenderer.h"

#include "core/math/Math.h"
#include "core/system/Screen.h"

#include "core-ext/settings/SessionSettings.h"

#include "gpuw/Device.h"
#include "gpuw/Public/Error.h"
#include "gpuw/RenderTarget.h"
#include "gpuw/Texture.h"
#include "renderer/material/RrPass.h"
#include "renderer/material/RrShaderProgram.h"
#include "renderer/material/Material.h"
#include "renderer/texture/RrTexture.h"
#include "renderer/texture/RrFontTexture.h"
#include "renderer/texture/RrRenderTexture.h"
#include "renderer/utils/rrMeshBuilder2D.h"
#include "renderer/utils/rrTextBuilder2D.h"
#include "renderer/state/RaiiHelpers2.h"

#include "engine-common/dusk/UI.h"
#include "engine-common/dusk/Element.h"

static core::settings::SessionSetting<bool> ui_DuskUpdateFullRect ("ui_DuskUpdateFullRect", true);

dusk::UIRenderer::UIRenderer (UserInterface* ui)
	: RrRenderObject(),
	m_interface(ui)
{
	m_renderTargetSize = Vector2i(0, 0);

	memset(&m_modeldata, 0, sizeof(m_modeldata));
	m_modeldata.indexNum	= 8192;
	m_modeldata.indices		= new uint16_t [m_modeldata.indexNum];
	m_modeldata.vertexNum	= 8192;
	m_modeldata.position	= new Vector3f [m_modeldata.vertexNum];
	m_modeldata.texcoord0	= new Vector3f [m_modeldata.vertexNum];
	m_modeldata.color		= new Vector4f [m_modeldata.vertexNum];
	m_modeldata.normal		= new Vector3f [m_modeldata.vertexNum];

	// Set up the copy pass
	RrPass copyPass;
	copyPass.m_layer = renderer::kRenderLayerV2D;
	copyPass.m_type = kPassTypeForward;
	copyPass.m_primitiveType = gpu::kPrimitiveTopologyTriangleStrip;
	copyPass.utilSetupAs2D();
	copyPass.m_surface.diffuseColor = Color(1.0F, 1.0F, 1.0F, 1.0F);
	copyPass.setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/sys/copy_buffer_simple_vv.spv", "shaders/sys/copy_buffer_simple_p.spv"}) );
	renderer::shader::Location t_vspecCopyPass[] = {renderer::shader::Location::kPosition,
													renderer::shader::Location::kUV0};
	copyPass.setVertexSpecificationByCommonList(t_vspecCopyPass, 2);
	copyPass.m_vertexSpecification[1].dataOffset = sizeof(Vector3f) * 4;
	PassInitWithInput(0, &copyPass);

	// Load up the default font used
	m_fontTexture	= RrFontTexture::Load( "Lekton-Bold.ttf", 16, kFW_Normal );

	// Set the actual UI pass
	RrPass dguiPass;
	dguiPass.m_layer = renderer::kRenderLayerV2D;
	dguiPass.m_type = kPassTypeJob;
	dguiPass.m_primitiveType = gpu::kPrimitiveTopologyTriangleStrip;
	dguiPass.utilSetupAs2D();
	dguiPass.m_surface.diffuseColor = Color(1.0F, 1.0F, 1.0F, 1.0F);
	dguiPass.setTexture( TEX_MAIN, m_fontTexture );
	dguiPass.setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/v2d/duskui_default_vv.spv", "shaders/v2d/duskui_default_p.spv"}) );
	renderer::shader::Location t_vspecDguiPass[] = {
		renderer::shader::Location::kPosition,
		renderer::shader::Location::kUV0,
		renderer::shader::Location::kColor,
		renderer::shader::Location::kNormal, // Normals used for controlling text or shapes.
		renderer::shader::Location::kUV1, // UV1 used for controlling scissor.
		renderer::shader::Location::kUV2 // UV2 used for controlling mouse-glow
		};
	dguiPass.setVertexSpecificationByCommonList(t_vspecDguiPass, 6);
	PassInitWithInput(1, &dguiPass);

	// Create the render targets
	m_renderTargetTexture = new gpu::Texture [1];
	m_renderTarget = new gpu::RenderTarget [1];

	// Create the vertex buffer
	m_vbufScreenQuad = new gpu::Buffer [1];
	Vector3f screenquad [] = {
		// positions
		Vector2f(1, 1),
		Vector2f(-1, 1),
		Vector2f(1, -1),
		Vector2f(-1, -1),
		// uvs
		Vector2f(1, 0),
		Vector2f(0, 0),
		Vector2f(1, 1),
		Vector2f(0, 1),
	};
	m_vbufScreenQuad->initAsVertexBuffer(NULL, gpu::kFormatR32G32B32SFloat, sizeof(screenquad)/sizeof(Vector3f));
	m_vbufScreenQuad->upload(NULL, screenquad, sizeof(screenquad), gpu::kTransferWriteDiscardPrevious);
}

dusk::UIRenderer::~UIRenderer (void)
{
	m_renderTargetTexture->free();
	delete[] m_renderTargetTexture;

	m_renderTarget->destroy(NULL);
	delete[] m_renderTarget;

	m_vbufScreenQuad->free(NULL);
	delete[] m_vbufScreenQuad;

	m_fontTexture->RemoveReference();

	delete[] m_modeldata.indices;
	delete[] m_modeldata.position;
	delete[] m_modeldata.texcoord0;
	delete[] m_modeldata.color;
	delete[] m_modeldata.normal;
}


// Can be called multiple times per frame, but generally only once per camera.
bool dusk::UIRenderer::CreateConstants ( rrCameraPass* cameraPass )
{
	PushCbufferPerObject(transform.world, cameraPass);
	return true;
}
bool dusk::UIRenderer::Render ( const rrRenderParams* params )
{
	if (params->pass == 1)
	{
		P1Render(params->context);
	}
	else if (params->pass == 0)
	{
		if (m_renderTargetTexture->valid())
		{
			gpu::GraphicsContext* gfx = params->context->context_graphics;

			gpu::Pipeline* pipeline = GetPipeline( params->pass );
			// Set up the material helper...
			renderer::Material(this, params->context, params, pipeline)
				// set the pipeline
				.setStart()
				// set the depth & rasterizer state registers
				.setDepthStencilState()
				.setRasterizerState()
				// bind the blend state
				.setBlendState();
			gfx->setShaderTextureAuto(gpu::kShaderStagePs, TEX_MAIN, m_renderTargetTexture);

			gfx->setVertexBuffer(0, m_vbufScreenQuad, 0); // Position
			gfx->setVertexBuffer(1, m_vbufScreenQuad, 0); // UV0
			gfx->draw(4, 0);
		}
	}
	else
	{
		ARCORE_ERROR("Invalid pass given to UIRenderer.");
	}
	return true;
}

// Called once per frame.
bool dusk::UIRenderer::BeginRender ( void )
{
	return true;
}

// Called once per frame.
bool dusk::UIRenderer::P1Render ( rrRenderContext* render_context )
{
	// Create the update area
	Rect t_updateArea;
	if ( m_interface->m_forcedUpdateAreas.empty() ) 
	{
		t_updateArea = Rect(m_interface->m_cursor, Vector2f(0,0));
	}
	else
	{
		t_updateArea = m_interface->m_forcedUpdateAreas[0];
		for ( Rect& rect : m_interface->m_forcedUpdateAreas) {
			t_updateArea.Expand(rect);
		}
		m_interface->m_forcedUpdateAreas.clear();
	}

	// Update the render target
	bool t_requireFullUpdate = P1UpdateRenderTarget();
	if (t_requireFullUpdate) {
		t_updateArea = Rect(0, 0, (Real)m_interface->GetScreen().GetWidth(), (Real)m_interface->GetScreen().GetHeight());
	}

	// Check update rect. Don't need to re-render if there's nothing to update
	if ( t_updateArea.size.x < 1 && t_updateArea.size.y < 1 ) {
		return true;
	}

	// Get the render list
	std::vector<Element*> t_renderList;
	t_renderList.reserve(m_interface->m_elements.size());
	P1UpdateRenderList(&t_renderList);

	// Render the elements
	P1RenderElements(render_context, t_renderList, t_updateArea);

	return true;
}

//	ERUpdateRenderTarget() : Updates & resizes render target.
bool dusk::UIRenderer::P1UpdateRenderTarget ( void )
{
	if ( m_renderTargetSize.x != m_interface->GetScreen().GetWidth() || m_renderTargetSize.y != m_interface->GetScreen().GetHeight() )
	{
		m_renderTarget->destroy(NULL);
		m_renderTargetTexture->free();

		// create the texture
		m_renderTargetTexture->allocate(core::gfx::tex::kTextureType2D, core::gfx::tex::kColorFormatRGBA8, m_interface->GetScreen().GetWidth(), m_interface->GetScreen().GetHeight(), 1, 1);

		// create the render target
		m_renderTarget->create(NULL);
		m_renderTarget->attach(gpu::kRenderTargetSlotColor0, m_renderTargetTexture);
		auto result = m_renderTarget->assemble();
		ARCORE_ASSERT(result == gpu::kError_SUCCESS);

		// update the target size
		m_renderTargetSize.x = m_interface->GetScreen().GetWidth();
		m_renderTargetSize.y = m_interface->GetScreen().GetHeight();

		return true;
	}

	return ui_DuskUpdateFullRect;
}

//	ERUpdateRenderList() : Updates the render list.
void dusk::UIRenderer::P1UpdateRenderList ( std::vector<Element*>* renderList )
{
	renderList->clear();

	// Clear out previous render state of all items
	for (uint32_t i = 0; i < m_interface->m_elements.size(); ++i)
	{
		Element* element = m_interface->m_elements[i];
		if (element != NULL)
		{
			element->m_wasDrawn = false;
		}
	}

	// Hold a dialogue render list in case we need it
	std::list<UserInterface::ElementNode*> delayedUpdateList;

	// Update positions, going down the tree.
	std::list<UserInterface::ElementNode*> updateList;
	updateList.push_front(m_interface->m_elementTreeBase);

	while (!updateList.empty())
	{
		bool bDrawChildren = false;

		UserInterface::ElementNode* elementNode = updateList.front();
		updateList.pop_front();

		if (elementNode->index != kElementHandleInvalid)
		{
			Element* element = m_interface->m_elements[elementNode->index];
			ARCORE_ASSERT(element->m_index == elementNode->index);

			if (m_interface->m_currentDialogue != elementNode->index)
			{
				// Add them to the rendering list
				if (element->m_elementType == ElementType::kControl
					&& element->m_visible)
				{
					renderList->push_back(element);
					bDrawChildren = true;
				}
				else if (element->m_elementType != ElementType::kControl
					&& element->m_visible)
				{
					bDrawChildren = true;
				}
			}
			else
			{
				// Add this to the delayed render list
				delayedUpdateList.push_back(elementNode);
			}
		}
		else
		{
			bDrawChildren = true;
		}

		// Render children of this item after all the current depth's items are added.
		if (bDrawChildren)
		{
			for (UserInterface::ElementNode* child : elementNode->children)
			{
				updateList.push_back(child);
			}
		}
	}

	// Any dialogues need their own loop to push their children onto the render list after everything else
	while (!delayedUpdateList.empty())
	{
		bool bDrawChildren = false;

		UserInterface::ElementNode* elementNode = delayedUpdateList.front();
		delayedUpdateList.pop_front();

		if (elementNode->index != kElementHandleInvalid)
		{
			Element* element = m_interface->m_elements[elementNode->index];
			ARCORE_ASSERT(element->m_index == elementNode->index);

			// Add them to the rendering list
			if (element->m_elementType == ElementType::kControl
				&& element->m_visible)
			{
				renderList->push_back(element);
				bDrawChildren = true;
			}
			else if (element->m_elementType != ElementType::kControl
				&& element->m_visible)
			{
				bDrawChildren = true;
			}
		}
		else
		{
			bDrawChildren = true;
		}

		// Render children of this item after all the current depth's items are added.
		if (bDrawChildren)
		{
			for (UserInterface::ElementNode* child : elementNode->children)
			{
				delayedUpdateList.push_back(child);
			}
		}
	}
}


void dusk::UIRenderer::P1RenderElements ( rrRenderContext* render_context, const std::vector<Element*>& renderList, const Rect& scissorArea )
{
	UIRendererContext l_ctx;
	l_ctx.m_uir = this;
	l_ctx.m_colors = &m_colors;

	// ================================
	// Update color state

	m_colors.setSize(m_interface->m_elements.size());
	m_colors.update();

	// ================================
	// Mesh generation

	// Set up the mesh builder now
	rrTextBuilder2D meshBuilder (m_fontTexture, m_interface->GetScreen().GetSize(), &m_modeldata);
	meshBuilder.enableAttribute(renderer::shader::kVBufferSlotNormal); // Require normals for the given mode
	l_ctx.m_mb2 = &meshBuilder;
	l_ctx.m_modeldata = &m_modeldata;

	// Loop through the elements and build their mesh
	for (uint32_t i = 0; i < renderList.size(); ++i)
	{
		l_ctx.setScissor(Rect((float)-0x7FFF, (float)-0x7FFF, (float)0xFFFF, (float)0xFFFF));
		renderList[i]->Render(&l_ctx);
		renderList[i]->m_wasDrawn = true;
	}

	// Upload the mesh
	arModelData t_modeldataDrawn = meshBuilder.getModelData();
	if (t_modeldataDrawn.vertexNum > 0)
	{
		m_meshBuffer.InitMeshBuffers(&t_modeldataDrawn);
	}
	else
	{
		// Nothing was drawn, end here. Consider freeing up the render texture.
		return;
	}

	// ================================
	// Parameter Setup

	// Set up the constant buffer
	struct DuskUI_Params
	{
		Vector4f positionTransform;
		Vector4f scissorCoords;
		Vector2f glowPosition;
		float glowSize;
		float glowStrength;
	} duskUI_Params;

	Vector2f offset, multiplier;
	meshBuilder.getScreenMapping(multiplier, offset);

	duskUI_Params.positionTransform = Vector4f( Vector2f(-offset.x / multiplier.x, -offset.y / multiplier.y), Vector2f(1.0F / multiplier.x, 1.0F / multiplier.y) );
	duskUI_Params.scissorCoords = Vector4f(scissorArea.pos, scissorArea.size);
	duskUI_Params.glowPosition = m_glowPosition;
	duskUI_Params.glowSize = 150.0F;
	duskUI_Params.glowStrength = 0.32F;

	// Upload the draw params
	gpu::Buffer cbDuskUI_Params = render_context->constantBuffer_pool->Allocate( sizeof(duskUI_Params) );
	cbDuskUI_Params.upload(render_context->context_graphics, &duskUI_Params, sizeof(duskUI_Params), gpu::kTransferWriteDiscardPrevious);


	// ================================
	// Rendering

	gpu::GraphicsContext* gfx = render_context->context_graphics;
	gfx->debugGroupPush("dusk::ERRenderElements");

	// Set up the target buffer
	gfx->setRenderTarget(m_renderTarget);

	// Enable and set up scissor
	gpu::RasterizerState rs;
	rs.scissorEnabled = true;
	gfx->setRasterizerState(rs);
	gfx->setViewport(0, 0, m_interface->GetScreen().GetWidth(), m_interface->GetScreen().GetHeight());
	gfx->setScissor(
		(int)scissorArea.pos.x,
		(int)(m_interface->GetScreen().GetHeight() - scissorArea.pos.y - scissorArea.size.y),
		(int)scissorArea.size.x,
		(int)scissorArea.size.y);

	// Clear the color of where we're rendering
	float t_clearColor[] = {0.0F, 0.0F, 0.0F, 0.0F};
	gfx->clearColor(t_clearColor);

	// TODO: add a fence for both the mesh buffer copy & the render target rendering
	const uint8_t kPassId = 1;
	gpu::Pipeline* pipeline = GetPipeline( kPassId );
	// Set up the material helper...
	renderer::Material(this, render_context, kPassId, kPassTypeForward, pipeline)
		// set the pipeline
		.setStart()
		// set almost everything else
		.setDepthStencilState()
		.setRasterizerState()
		.setBlendState()
		.setTextures();
	// Set the constant buffer used
	gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_OBJECT_EXTENDED, &m_cbufPerObjectSurfaces[1]);
	gfx->setShaderCBuffer(gpu::kShaderStagePs, renderer::CBUFFER_PER_OBJECT_EXTENDED, &m_cbufPerObjectSurfaces[1]);
	gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_USER0, &cbDuskUI_Params);
	gfx->setShaderCBuffer(gpu::kShaderStagePs, renderer::CBUFFER_USER0, &cbDuskUI_Params);

	// bind the index buffer
	gfx->setIndexBuffer(&m_meshBuffer.m_indexBuffer, gpu::kIndexFormatUnsigned16);
	// bind the vertex buffers
	auto passAccess = PassAccess(kPassId);
	for (int i = 0; i < passAccess.getVertexSpecificationCount(); ++i)
	{
		int buffer_index = (int)passAccess.getVertexSpecification()[i].location;
		int buffer_binding = (int)passAccess.getVertexSpecification()[i].binding;
		if (m_meshBuffer.m_bufferEnabled[buffer_index])
			gfx->setVertexBuffer(buffer_binding, &m_meshBuffer.m_buffer[buffer_index], 0);
	}

	// draw now
	gfx->drawIndexed(t_modeldataDrawn.indexNum, 0, 0);

	// switch back to normal scissor mode
	gfx->setRasterizerState(gpu::RasterizerState());

	//gfx->submit(); // TODO: remove this
	gfx->clearPipelineAndWait();

	gfx->debugGroupPop();
}

//===============================================================================================//

void dusk::UIRendererBlendedColor::setColor ( const Color& next_color, const float blend_time )
{
	if (main_color_next != next_color)
	{
		main_color_prev = main_color_prev.Lerp(main_color_next, math::saturate(main_color_blend));;
		main_color_next = next_color;
		main_color_blend = 0.0F;
		main_color_blend_time = blend_time;
	}
}

void dusk::UIRendererBlendedColor::setPulse ( const Color& in_pulse_color )
{
	pulse_color = in_pulse_color;
	pulse_up = true;
}

void dusk::UIRendererBlendedColor::update ( void )
{
	// Main blend
	if (main_color_blend_time <= 0.0F)
	{
		main_color_blend = 1.0F;
	}
	else
	{
		main_color_blend += Time::deltaTime / main_color_blend_time;
	}
	color = main_color_prev.Lerp(main_color_next, math::saturate(main_color_blend));

	// Add the pulse blend
	if (pulse_up)
	{
		pulse_blend += Time::deltaTime / 0.02F;
	}
	else
	{
		pulse_blend -= Time::deltaTime / 0.2F;
	}
	if (pulse_blend > 1.0F)
	{
		pulse_up = false;
	}
	pulse_blend = math::saturate(pulse_blend);
	color = color.Lerp(pulse_color, pulse_blend);
}

//===============================================================================================//

void dusk::UIRendererElementColors::setSize ( const size_t size )
{
	if (m_elementBackgroundColors.size() < size)
	{
		m_elementBackgroundColors.resize(size);
	}
}

void dusk::UIRendererElementColors::update ( void )
{
	// Update all the blends
	for (auto& elementColors : m_elementBackgroundColors)
	{
		for (UIRendererBlendedColor& element : elementColors)
		{
			element.update();
		}
	}
}

void dusk::UIRendererElementColors::setBackgroundColor ( Element* element, size_t subelement, const Color& color )
{
	auto& elementColors = m_elementBackgroundColors[element->m_index];
	if (elementColors.size() <= subelement)
	{
		elementColors.resize(subelement + 1);
	}
	UIRendererBlendedColor& blend = elementColors[subelement];
	blend.setColor(color, 0.05F);
}

void dusk::UIRendererElementColors::setBackgroundClickPulse ( Element* element, size_t subelement, const Color& click_pulse )
{
	auto& elementColors = m_elementBackgroundColors[element->m_index];
	if (elementColors.size() <= subelement)
	{
		elementColors.resize(subelement + 1);
	}
	UIRendererBlendedColor& blend = elementColors[subelement];
	blend.setPulse(click_pulse);
}

Color dusk::UIRendererElementColors::getBackgroundColor ( Element* element, size_t subelement )
{
	auto& elementColors = m_elementBackgroundColors[element->m_index];
	if (elementColors.size() <= subelement)
	{
		elementColors.resize(subelement + 1);
	}
	return elementColors[subelement].getCurrentColor();
}