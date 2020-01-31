#include "UIRenderer.h"

#include "core/system/Screen.h"

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

#include "engine-common/dusk/UI.h"
#include "engine-common/dusk/Element.h"

dusk::UIRenderer::UIRenderer (UserInterface* ui)
	: CRenderableObject(),
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
	PassInitWithInput(0, &copyPass);

	// Load up the default font used
	m_fontTexture	= RrFontTexture::Load( "monofonto.ttf", 16, kFW_Bold );

	// Set the actual UI pass
	RrPass dguiPass;
	dguiPass.m_layer = renderer::kRenderLayerSkip;
	dguiPass.m_type = kPassTypeForward;
	dguiPass.m_primitiveType = gpu::kPrimitiveTopologyTriangleList;
	copyPass.utilSetupAs2D();
	dguiPass.m_surface.diffuseColor = Color(1.0F, 1.0F, 1.0F, 1.0F);
	dguiPass.setTexture( TEX_MAIN, m_fontTexture );
	dguiPass.setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/v2d/duskui_default_vv.spv", "shaders/v2d/duskui_default_p.spv"}) );
	renderer::shader::Location t_vspecDguiPass[] = {renderer::shader::Location::kPosition,
													renderer::shader::Location::kUV0,
													renderer::shader::Location::kColor,
													renderer::shader::Location::kNormal}; // Normals used for controlling text or shapes.
	dguiPass.setVertexSpecificationByCommonList(t_vspecDguiPass, 4);
	PassInitWithInput(1, &dguiPass);

	// Create the render targets
	m_renderTargetTexture = new gpu::Texture [1];
	m_renderTarget = new gpu::RenderTarget [1];
}

dusk::UIRenderer::~UIRenderer (void)
{
	delete[] m_renderTargetTexture;
	m_renderTargetTexture->free();

	delete[] m_renderTarget;
	m_renderTarget->destroy(NULL);

	m_fontTexture->RemoveReference();

	delete[] m_modeldata.indices;
	delete[] m_modeldata.position;
	delete[] m_modeldata.texcoord0;
	delete[] m_modeldata.color;
	delete[] m_modeldata.normal;
}


// Can be called multiple times per frame, but generally only once per camera.
bool dusk::UIRenderer::PreRender ( rrCameraPass* cameraPass )
{
	PushCbufferPerObject(transform.world, cameraPass);
	return true;
}
bool dusk::UIRenderer::Render ( const rrRenderParams* params )
{
	gpu::GraphicsContext* gfx = gpu::getDevice()->getContext();

	gpu::Pipeline* pipeline = GetPipeline( params->pass );
	// Set up the material helper...
	renderer::Material(this, gfx, params->pass, pipeline)
		// set the pipeline
		.setStart()
		// set the depth & rasterizer state registers
		.setDepthStencilState()
		.setRasterizerState()
		// bind the blend state
		.setBlendState();
	gfx->setShaderTextureAuto(gpu::kShaderStagePs, TEX_MAIN, m_renderTargetTexture);

	// Bind the material for alpha-blending
	//matScreenCopy->setTexture( TEX_MAIN, renderBuffer );
	//matScreenCopy->bindPass( pass );
	//GLd.DrawScreenQuad(matScreenCopy);

	return true;
}

// Called once per frame.
bool dusk::UIRenderer::BeginRender ( void )
{
	return true;
}

// Called once per frame.
bool dusk::UIRenderer::EndRender ( void )
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
	bool t_requireFullUpdate = ERUpdateRenderTarget();
	if (t_requireFullUpdate) {
		t_updateArea = Rect(0, 0, (Real)Screen::Info.width, (Real)Screen::Info.height);
	}

	// Check update rect. Don't need to re-render if there's nothing to update
	if ( t_updateArea.size.x < 1 && t_updateArea.size.y < 1 ) {
		return true;
	}

	// Get the render list
	std::vector<Element*> t_renderList;
	t_renderList.reserve(m_interface->m_elements.size());
	ERUpdateRenderList(&t_renderList);

	// Render the elements
	ERRenderElements(t_renderList, t_updateArea);

	return true;
}

//	ERUpdateRenderTarget() : Updates & resizes render target.
bool dusk::UIRenderer::ERUpdateRenderTarget ( void )
{
	if ( m_renderTargetSize.x != Screen::Info.width || m_renderTargetSize.y != Screen::Info.height )
	{
		m_renderTarget->destroy(NULL);
		m_renderTargetTexture->free();

		// create the texture
		m_renderTargetTexture->allocate(core::gfx::tex::kTextureType2D, core::gfx::tex::kColorFormatRGBA8, Screen::Info.width, Screen::Info.height, 1, 1);

		// create the render target
		m_renderTarget->create(NULL);
		m_renderTarget->attach(gpu::kRenderTargetSlotColor0, m_renderTargetTexture);
		auto result = m_renderTarget->assemble();
		ARCORE_ASSERT(result == gpu::kError_SUCCESS);

		// update the target size
		m_renderTargetSize.x = Screen::Info.width;
		m_renderTargetSize.y = Screen::Info.height;

		return true;
	}

	return false;
}

//	ERUpdateRenderList() : Updates the render list.
void dusk::UIRenderer::ERUpdateRenderList ( std::vector<Element*>* renderList )
{
	renderList->clear();

	// Hold a dialogue render list in case we need it
	std::vector<Element*> t_renderListDialogue;

	// Here, we want to actually generate render lists and render to the framebuffer.
	std::vector<bool> t_elementAdded (m_interface->m_elements.size(), false);

	// Loop through all the elements and generate render lists
	for (uint32_t i = 0; i < m_interface->m_elements.size(); ++i)
	{
		Element* currentElement = m_interface->m_elements[i];

		// Skip items that have already been considered
		if (t_elementAdded[currentElement->m_index])
			continue;
		// Skip deleted items
		if (currentElement == NULL)
			continue;

		// Go to the first parent
		Element* rootParent = currentElement;
		while (rootParent->m_parent != NULL)
		{
			ARCORE_ASSERT(m_interface->m_elements[rootParent->m_index] == rootParent);
			rootParent = rootParent->m_parent;
		}

		// Skip if it's not visible
		if (!rootParent->m_visible)
			continue;

		// Otherwise, we add the current item to the list, then start running down the list of children
		std::vector<Element*> t_children;
		t_children.push_back(rootParent);

		for (uint32_t iTree = 0; iTree < t_children.size(); ++iTree)
		{
			Element* currentParent = t_children[iTree];

			// Now look for all the children
			for (uint32_t iChild = 0; iChild < m_interface->m_elements.size(); ++iChild)
			{
				Element* possibleChild = m_interface->m_elements[iChild];

				// Skip items that have already been considered
				if (t_elementAdded[iChild])
					continue;
				// Skip deleted items
				if (possibleChild == NULL)
					continue;

				// Skip if it's not visible
				if (!possibleChild->m_visible)
					continue;

				// Check if the parent matches, and if it does match...
				if (possibleChild->m_parent == currentParent)
				{
					// Save this child to check it for children & mark it as considered
					t_children.push_back(possibleChild);
					t_elementAdded[possibleChild->m_index] = true;
				}
			}
		}

		// Add the current t_children list to the render list
		if (m_interface->m_currentDialogue != rootParent->m_index)
		{
			for (uint32_t iTree = 0; iTree < t_children.size(); ++iTree)
			{
				currentElement = t_children[iTree];
				ARCORE_ASSERT(m_interface->m_elements[currentElement->m_index] == currentElement);
				renderList->push_back(currentElement);
			}
		}
		// If in dialogue mode, add the current t_children list to the dialogue list that has to be added at the end
		else
		{
			for (uint32_t iTree = 0; iTree < t_children.size(); ++iTree)
			{
				currentElement = t_children[iTree];
				ARCORE_ASSERT(m_interface->m_elements[currentElement->m_index] == currentElement);
				t_renderListDialogue.push_back(currentElement);
			}
		}
	}

	// Force dialogues to come last in the rendering loop
	for (uint32_t i = 0; i < t_renderListDialogue.size(); ++i)
	{
		renderList->push_back(t_renderListDialogue[i]);
	}
}


void dusk::UIRenderer::ERRenderElements (const std::vector<Element*>& renderList, const Rect& scissorArea)
{
	UIRendererContext l_ctx;
	l_ctx.m_uir = this;

	//
	// Mesh generation

	// Set up the mesh builder now
	rrMeshBuilder2D meshBuilder (&m_modeldata);
	meshBuilder.enableAttribute(renderer::shader::kVBufferSlotNormal); // Require normals for the given mode

	// Loop through the elements and build their mesh
	for (uint32_t i = 0; i < renderList.size(); ++i)
	{
		renderList[i]->Render(&l_ctx);
	}

	// Upload the mesh
	arModelData t_modeldataDrawn = meshBuilder.getModelData();
	m_meshBuffer.InitMeshBuffers(&t_modeldataDrawn);


	//
	// Rendering

	gpu::GraphicsContext* gfx = gpu::getDevice()->getContext();

	// Set up the target buffer
	gfx->setRenderTarget(m_renderTarget);
	gfx->setScissor(
		(int)scissorArea.pos.x,
		(int)(Screen::Info.height - scissorArea.pos.y - scissorArea.size.y),
		(int)scissorArea.size.x,
		(int)scissorArea.size.y);

	// Clear the color of where we're rendering
	float t_clearColor[] = {0.0F, 0.0F, 0.0F, 0.0F};
	gfx->clearColor(t_clearColor);

	// TODO: add a fence for both the mesh buffer copy & the render target rendering
	const uint8_t kPassId = 1;
	gpu::Pipeline* pipeline = GetPipeline( kPassId );
	// Set up the material helper...
	renderer::Material(this, gfx, kPassId, pipeline)
		// set the pipeline
		.setStart()
		// set almost everything else
		.setDepthStencilState()
		.setRasterizerState()
		.setBlendState()
		.setTextures();
	// Set the constant buffer used
	gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_OBJECT_EXTENDED, &m_cbufPerObjectSurfaces[1]);

	// bind the index buffer
	gfx->setIndexBuffer(&m_meshBuffer.m_indexBuffer, gpu::kIndexFormatUnsigned16);
	// bind the vertex buffers
	for (int i = 0; i < renderer::shader::kVBufferSlotMaxCount; ++i)
		if (m_meshBuffer.m_bufferEnabled[i])
			gfx->setVertexBuffer(i, &m_meshBuffer.m_buffer[i], 0);

	// draw now
	gfx->drawIndexed(t_modeldataDrawn.indexNum, 0);

	gfx->submit(); // TODO: remove this
}