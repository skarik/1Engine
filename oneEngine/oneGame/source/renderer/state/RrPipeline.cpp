#include "RrPipeline.h"
#include "RrPipelinePaletted.h"

#include "core-ext/settings/SessionSettings.h"

#include "renderer/state/RrRenderer.h"
#include "renderer/object/RrRenderObject.h"
#include "renderer/camera/RrCamera.h"
#include "renderer/material/RrShaderProgram.h"

#include "renderer/state/RaiiHelpers.h"

#include "gpuw/Pipeline.h"
#include "gpuw/ShaderPipeline.h"

//=====================================
// Pipeline instantiators
//=====================================

RrPipelineStateRenderer* renderer::CreatePipelineRenderer ( const renderer::PipelineMode mode )
{
	switch (mode)
	{
	case renderer::PipelineMode::kNormal:
		return new RrPipelineStandardRenderer();

	case renderer::PipelineMode::kPaletted:
		return new RrPipelinePalettedRenderer();
	}
	return nullptr;
}

RrPipelineOptions* renderer::CreatePipelineOptions ( const renderer::PipelineMode mode )
{
	switch (mode)
	{
	case renderer::PipelineMode::kNormal:
		return new RrPipelineStandardOptions();

	case renderer::PipelineMode::kPaletted:
		return new RrPipelinePalettedOptions();
	}
	return nullptr;
}

//=====================================
// Standard renderer pipeline
//=====================================

RrPipelineStandardRenderer::RrPipelineStandardRenderer ( void )
	: RrPipelineStateRenderer()
{
	m_lightingCompositePipeline = new gpu::Pipeline;
}

RrPipelineStandardRenderer::~RrPipelineStandardRenderer ( void )
{
	delete m_lightingCompositeProgram;

	m_lightingCompositePipeline->destroy(NULL);
	delete m_lightingCompositePipeline;
}

void RrPipelineStandardRenderer::CullObjects ( gpu::GraphicsContext* gfx, const RrOutputInfo& output, RrOutputState* state, RrWorld* world )
{
	// Update the render distance of objects against this camera:
	for ( int objectIndex = 0; objectIndex < world->objects.size(); objectIndex += 1 )
	{	
		// Put into it's own loop, since it's the same calculation across all objects
		if ( world->objects[objectIndex] != nullptr )
		{ 
			// TODO: Handle 2D mode properly. Postprocess is entirely sorted by transform.position.z Should this be made more consistent?
			world->objects[objectIndex]->renderDistance = (output.camera->transform.position - world->objects[objectIndex]->transform.world.position).sqrMagnitude();
		}
	}
}

#include "../.res-1/shaders/deferred_pass/shade_lighting_p.variants.h"

static core::settings::SessionSetting<bool> gsesh_LightingUseDebugBuffers ("rdbg_deferred_gbuffers", false);
static core::settings::SessionSetting<bool> gsesh_LightingUseLighting ("rdbg_deferred_lighting", true);

void RrPipelineStandardRenderer::CompositeDeferred ( gpu::GraphicsContext* gfx, const rrPipelineCompositeInput& compositeInput, RrOutputState* state )
{
	RR_SHADER_VARIANT(shade_lighting_p) l_shadeLightingVariantInfo;
	l_shadeLightingVariantInfo.VARIANT_DEBUG_GBUFFERS = gsesh_LightingUseDebugBuffers;
	l_shadeLightingVariantInfo.VARIANT_DEBUG_LIGHTING = gsesh_LightingUseLighting;

	auto renderer = RrRenderer::Active; // TODO: make argument

	RrShaderProgram* desiredProgram = RrShaderProgram::Load(rrShaderProgramVsPs{
		"shaders/deferred_pass/shade_common_vv.spv",
		("shaders/deferred_pass/" + l_shadeLightingVariantInfo.GetVariantName()).c_str()
		});
	if (desiredProgram != m_lightingCompositeProgram)
	{
		if (m_lightingCompositeProgram != nullptr)
		{
			m_lightingCompositeProgram->RemoveReference();
		}
		m_lightingCompositeProgram = desiredProgram;

		// Create a new rendering pipeline.
		m_lightingCompositePipeline->destroy(NULL);
		renderer->CreatePipeline(&m_lightingCompositeProgram->GetShaderPipeline(), *m_lightingCompositePipeline);
	}

	// Grab output size for the screen quad info
	auto& output = *state->output_info;
	rrViewport output_viewport =  output.GetOutputViewport();

	// Create render target output
	rrRenderTarget compositeOutput (compositeInput.output_color);
	
	// Set output
	gfx->setRenderTarget(&compositeOutput.m_renderTarget);
	// Render the current result to the screen
	gfx->setViewport(output_viewport.corner.x, output_viewport.corner.y, output_viewport.corner.x + output_viewport.size.x, output_viewport.corner.y + output_viewport.size.y);
	gfx->setScissor(output_viewport.corner.x, output_viewport.corner.y, output_viewport.corner.x + output_viewport.size.x, output_viewport.corner.y + output_viewport.size.y);

	// Set up output state
	{
		// No cull. Always draw.
		gpu::RasterizerState rs;
		rs.cullmode = gpu::kCullModeNone;
		gfx->setRasterizerState(rs);

		// No depth/stencil test. Always draw.
		gpu::DepthStencilState ds;
		ds.depthTestEnabled   = false;
		ds.depthWriteEnabled  = false;
		ds.stencilTestEnabled = false;
		ds.stencilWriteMask   = 0x00;
		gfx->setDepthStencilState(ds);

		// Simple alpha-blending
		gpu::BlendState bs;
		bs.enable = true;
		bs.src = gpu::kBlendModeSrcAlpha;
		bs.dst = gpu::kBlendModeInvSrcAlpha;
		bs.srcAlpha = gpu::kBlendModeOne;
		bs.dstAlpha = gpu::kBlendModeOne;
		bs.opAlpha = gpu::kBlendOpMax;
		gfx->setBlendState(bs);
	}

	// Create cbuffer for the input
	struct rrCbufferCompositeParams
	{
	} cbuffer_composite_params;

	gpu::Buffer cbuffer;
	cbuffer.initAsConstantBuffer(NULL, sizeof(rrCbufferCompositeParams));
	cbuffer.upload(gfx, &cbuffer_composite_params, sizeof(rrCbufferCompositeParams), gpu::kTransferStream);

	// Render with the composite shader
	{
		gfx->setPipeline(m_lightingCompositePipeline);
		gfx->setVertexBuffer(0, &renderer->GetScreenQuadVertexBuffer(), 0); // see RrPipelinePasses.cpp
		gfx->setVertexBuffer(1, &renderer->GetScreenQuadVertexBuffer(), 0); // there are two binding slots defined with different stride
		gfx->setShaderTextureAuto(gpu::kShaderStagePs, 0, compositeInput.deferred_albedo);
		gfx->setShaderTextureAuto(gpu::kShaderStagePs, 1, compositeInput.deferred_normals);
		gfx->setShaderTextureAuto(gpu::kShaderStagePs, 2, compositeInput.deferred_surface);
		gfx->setShaderTextureAuto(gpu::kShaderStagePs, 3, compositeInput.deferred_emissive);
		gfx->setShaderTextureAuto(gpu::kShaderStagePs, 4, compositeInput.combined_depth);
		if (compositeInput.forward_color != nullptr)
		{
			gfx->setShaderTextureAuto(gpu::kShaderStagePs, 5, compositeInput.forward_color);
		}
		gfx->setShaderCBuffer(gpu::kShaderStagePs, renderer::CBUFFER_USER0, &cbuffer);

		gfx->draw(4, 0);
	}

	// done with cbuffer
	cbuffer.free(NULL);
}

rrPipelineOutput RrPipelineStandardRenderer::RenderLayerEnd ( gpu::GraphicsContext* gfx, const rrPipelineLayerFinishInput& finishInput, RrOutputState* state ) 
{
	if (finishInput.layer != renderer::kRenderLayerWorld)
	{
		return RrPipelineStateRenderer::RenderLayerEnd(gfx, finishInput, state);
	}
	else
	{
		return RrPipelineStateRenderer::RenderLayerEnd(gfx, finishInput, state);
	}
}