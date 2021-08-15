#include "RrPipelineStandard.h"
#include "RrPipelinePaletted.h"

#include "core-ext/settings/SessionSettings.h"

#include "renderer/state/RrRenderer.h"
#include "renderer/object/RrRenderObject.h"
#include "renderer/camera/RrCamera.h"
#include "renderer/material/RrShaderProgram.h"
#include "renderer/material/ShaderVariations.h"

#include "renderer/state/RaiiHelpers.h"

#include "gpuw/Pipeline.h"
#include "gpuw/ShaderPipeline.h"

//=====================================
// Standard renderer pipeline
//=====================================

RrPipelineStandardRenderer::RrPipelineStandardRenderer ( void )
	: RrPipelineStateRenderer()
{
	;
}

RrPipelineStandardRenderer::~RrPipelineStandardRenderer ( void )
{
	if (m_lightingCompositeProgram)
		m_lightingCompositeProgram->RemoveReference();

	if (m_lightingCompositePipeline)
	{
		m_lightingCompositePipeline->destroy(NULL);
		delete m_lightingCompositePipeline;
	}

	if (m_lightingLighting0Program)
		m_lightingLighting0Program->RemoveReference();

	if (m_lightingLighting0Pipeline)
	{
		m_lightingLighting0Pipeline->destroy(NULL);
		delete m_lightingLighting0Pipeline;
	}
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

//	GetPostprocessVariant(...) : Helper to cache specific variations of shaders
// This should probably be a smarter or better-thought out mechanism at some point, but -
//  - we have such a narrow usecase, we don't need complicated things right now.
// Arguments:
//	renderer:		Renderer to pull the common geometry pipeline information from.
//	vertexShader:	SPV Resource to use as vertex shader
//	pixelShaderBase:	SPV Resource Path to use as a basis for pixel shader path
//	variantInfo:	Shader to load
//	isForScreenQuad:	If the created pipeline is for a screen quad or for geometry.
//	out cachedPipeline:	Output created pipeline.
//	out cachedProgram:	Output created program
static void GetPostprocessVariant (
	RrRenderer* renderer,
	const char* vertexShader,
	const char* pixelShaderBase,
	const RrShaderVariantBase& variantInfo,
	const bool isForScreenQuad,
	gpu::Pipeline** cachedPipeline,
	RrShaderProgram** cachedProgram )
{
	RrShaderProgram* desiredProgram = RrShaderProgram::Load(rrShaderProgramVsPs{
		vertexShader,
		(pixelShaderBase + variantInfo.GetVariantName()).c_str()
		});

	if (desiredProgram != *cachedProgram)
	{
		// Cache the shader program we found:
		if (*cachedProgram != nullptr)
		{
			(*cachedProgram)->RemoveReference();
		}
		*cachedProgram = desiredProgram;

		// Cache the pipeline we want to use:
		if (*cachedPipeline != nullptr)
		{
			(*cachedPipeline)->destroy(NULL);
		}
		else
		{
			*cachedPipeline = new gpu::Pipeline;
		}
		if (isForScreenQuad)
		{
			renderer->CreatePipeline(&(*cachedProgram)->GetShaderPipeline(), **cachedPipeline); // Creates a pipeline specifically for screen quad.
		}
		else
		{
			ARCORE_ERROR("TODO: create a general geometry pipeline for the shader, for loaded meshes.");
		}
	}
}

#include "renderer/light/RrLight.h"

#include "../.res-1/shaders/deferred_pass/shade_lighting_p.variants.h"

static core::settings::SessionSetting<bool> gsesh_LightingUseDebugBuffers ("rdbg_deferred_gbuffers", false);
static core::settings::SessionSetting<bool> gsesh_LightingUseLighting ("rdbg_deferred_lighting", true);

rrCompositeOutput RrPipelineStandardRenderer::CompositeDeferred ( gpu::GraphicsContext* gfx, const rrPipelineCompositeInput& compositeInput, RrOutputState* state )
{
	auto renderer = RrRenderer::Active; // TODO: make argument

	{
		RR_SHADER_VARIANT(shade_lighting_p) l_shadeLightingVariantInfo;
		l_shadeLightingVariantInfo.VARIANT_PASS = gsesh_LightingUseDebugBuffers
			? l_shadeLightingVariantInfo.VARIANT_PASS_DEBUG_SURFACE
			: l_shadeLightingVariantInfo.VARIANT_PASS_DO_INDIRECT_EMISSIVE;

		GetPostprocessVariant(renderer, "shaders/deferred_pass/shade_common_vv.spv", "shaders/deferred_pass/",
			l_shadeLightingVariantInfo, true,
			&m_lightingCompositePipeline, &m_lightingCompositeProgram);
	}

	{
		RR_SHADER_VARIANT(shade_lighting_p) l_shadeLightingVariantInfo;
		l_shadeLightingVariantInfo.VARIANT_PASS = l_shadeLightingVariantInfo.VARIANT_PASS_DO_DIRECT_DIRECTIONAL;

		GetPostprocessVariant(renderer, "shaders/deferred_pass/shade_instanced_vv.spv", "shaders/deferred_pass/",
			l_shadeLightingVariantInfo, true,
			&m_lightingLighting0Pipeline,
			&m_lightingLighting0Program);
	}

#if 0
	{
		RR_SHADER_VARIANT(shade_lighting_p) l_shadeLightingVariantInfo;
		l_shadeLightingVariantInfo.VARIANT_PASS = l_shadeLightingVariantInfo.VARIANT_PASS_DO_DIRECT_OMNI;

		GetPostprocessVariant(renderer, "shaders/deferred_pass/shade_worldproj_instanced_vv.spv", "shaders/deferred_pass/",
			l_shadeLightingVariantInfo, false,
			&m_lightingLightingOmniPipeline, &m_lightingLightingOmniProgram);
	}
#endif
	gfx->debugGroupPush("RrPipelineStandardRenderer::CompositeDeferred");

	// Grab output size for the screen quad info
	auto& output = *state->output_info;
	rrViewport output_viewport =  output.GetOutputViewport();

	// Allocate a new buffer 
	gpu::Texture outputLightingComposite;
	rrRTBufferRequest colorRequest {output_viewport.size, core::gfx::tex::kColorFormatRGBA16F}; 
	renderer->CreateRenderTexture( colorRequest, &outputLightingComposite );

	// Create render target output
	rrRenderTarget compositeOutput (&outputLightingComposite);
	
	// Set output
	gfx->setRenderTarget(&compositeOutput.m_renderTarget);
	// Render the current result to the screen
	gfx->setViewport(output_viewport.corner.x, output_viewport.corner.y, output_viewport.corner.x + output_viewport.size.x, output_viewport.corner.y + output_viewport.size.y);
	gfx->setScissor(output_viewport.corner.x, output_viewport.corner.y, output_viewport.corner.x + output_viewport.size.x, output_viewport.corner.y + output_viewport.size.y);

	// Clear color
	{
		float clearColor[] = {0, 0, 0, 0};
		gfx->clearColor(clearColor);
	}

	// Debug output:
	if (gsesh_LightingUseDebugBuffers)
	{
		DrawDebugOutput(gfx, compositeInput, state);
	}
	else
	{
		// Render the lighting

		auto lightSetup = SetupLights(gfx);

		RenderShadows(gfx, compositeInput, &lightSetup);

		outputLightingComposite = RenderLights(gfx, compositeInput, state, &lightSetup, outputLightingComposite);

		// Render the forward data
		if (compositeInput.forward_color)
		{
			{ // No depth/stencil test. Always draw.
				gpu::DepthStencilState ds;
				ds.depthTestEnabled   = false;
				ds.depthWriteEnabled  = false;
				ds.stencilTestEnabled = false;
				ds.stencilWriteMask   = 0x00;
				gfx->setDepthStencilState(ds);
			}

			// Alpha blend using the destination's alpha channel
			gpu::BlendState bs;
			bs.enable = true;
			bs.src = gpu::kBlendModeInvDstAlpha;
			bs.dst = gpu::kBlendModeDstAlpha;
			bs.srcAlpha = gpu::kBlendModeOne;
			bs.dstAlpha = gpu::kBlendModeOne;
			bs.opAlpha = gpu::kBlendOpMax;
			gfx->setBlendState(bs);

			gfx->setPipeline(&renderer->GetScreenQuadCopyPipeline());
			gfx->setVertexBuffer(0, &renderer->GetScreenQuadVertexBuffer(), 0); // see RrPipelinePasses.cpp
			gfx->setVertexBuffer(1, &renderer->GetScreenQuadVertexBuffer(), 0); // there are two binding slots defined with different stride
			gfx->setShaderTextureAuto(gpu::kShaderStagePs, 0, compositeInput.forward_color);

			gfx->draw(4, 0);
		}
	}

	gfx->debugGroupPop();

	return rrCompositeOutput {outputLightingComposite};
}

void RrPipelineStandardRenderer::DrawDebugOutput (
	gpu::GraphicsContext* gfx,
	const rrPipelineCompositeInput& compositeInput,
	RrOutputState* state)
{
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

	// Render with the composite shader
	DrawWithPipelineAndGBuffers(gfx, compositeInput, m_lightingCompositePipeline, nullptr, nullptr, [](RrRenderer* renderer, gpu::GraphicsContext* gfx)
	{
		gfx->setVertexBuffer(0, &renderer->GetScreenQuadVertexBuffer(), 0); // see RrPipelinePasses.cpp
		gfx->setVertexBuffer(1, &renderer->GetScreenQuadVertexBuffer(), 0); // there are two binding slots defined with different stride
		gfx->draw(4, 0);
	});
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