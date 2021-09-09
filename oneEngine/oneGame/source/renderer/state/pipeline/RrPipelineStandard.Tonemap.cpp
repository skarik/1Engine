#include "RrPipelineStandard.h"

#include "core-ext/settings/SessionSettings.h"

#include "renderer/light/RrLight.h"
#include "renderer/object/RrRenderObject.h"
#include "renderer/material/RrShaderProgram.h"
#include "renderer/camera/RrCamera.h"
#include "renderer/state/RrRenderer.h"
#include "renderer/types/shaders/cbuffers.h"
#include "renderer/types/shaders/sbuffers.h"

#include "renderer/state/RaiiHelpers.h"

#include "gpuw/Pipeline.h"
#include "gpuw/ShaderPipeline.h"
#include "gpuw/WriteableResource.h"
#include "gpuw/Sampler.h"

RrPipelineStandardRenderer::rrTonemapSetup
RrPipelineStandardRenderer::SetupTonemap (
		rrRenderContext* context,
		gpu::Texture* input_color,
		RrOutputState* state)
{
	return rrTonemapSetup();
}

RrPipelineStandardRenderer::rrExposureSetup
RrPipelineStandardRenderer::SetupExposure (
		rrRenderContext* context,
		rrPreviousFrameOutput* reference_frame,
		RrOutputState* state)
{
	return rrExposureSetup();
}


//	GetPostprocess(...) : Helper to cache specific variations of shaders
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
static void GetPostprocess (
	RrRenderer* renderer,
	const char* vertexShader,
	const char* pixelShader,
	const bool isForScreenQuad,
	gpu::Pipeline** cachedPipeline,
	RrShaderProgram** cachedProgram )
{
	RrShaderProgram* desiredProgram = RrShaderProgram::Load(rrShaderProgramVsPs{
		vertexShader,
		pixelShader,
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

gpu::Texture
RrPipelineStandardRenderer::ApplyTonemap (
	rrRenderContext* context,
	gpu::Texture* input_color,
	rrBloomSetup* bloom_setup,
	rrTonemapSetup* tonemap_setup,
	rrExposureSetup* exposure_setup,
	rrCameraPass* cameraPass,
	RrOutputState* state )
{
	auto renderer = RrRenderer::Active; // TODO: make argument
	auto gfx = context->context_graphics;

	gpu::Texture result_color = *input_color;

	// Create samplers:
	gpu::Sampler linearSampler;
	linearSampler.create(
		NULL,
		gpu::SamplerCreationDescription()
			.MagFilter(core::gfx::tex::kSamplingLinear)
			.MinFilter(core::gfx::tex::kSamplingLinear)
			.WrapmodeX(core::gfx::tex::kWrappingClamp)
			.WrapmodeY(core::gfx::tex::kWrappingClamp)
	);
	gpu::Sampler pointSampler;
	pointSampler.create(
		NULL,
		gpu::SamplerCreationDescription()
			.MagFilter(core::gfx::tex::kSamplingPoint)
			.MinFilter(core::gfx::tex::kSamplingPoint)
			.WrapmodeX(core::gfx::tex::kWrappingClamp)
			.WrapmodeY(core::gfx::tex::kWrappingClamp)
	);

	// Grab palettize shader
	GetPostprocess(renderer, "shaders/deferred_pass/shade_common_vv.spv", "shaders/postprocess/exposure_p.spv",
		true,
		&m_postprocessTonemapPipeline,
		&m_postprocessTonemapProgram);

	// Grab output size for the screen quad info
	auto& output = *state->output_info;
	rrViewport output_viewport =  output.GetOutputViewport();

	if (true)
	{
		// Allocate a new buffer for the outlines
		gpu::Texture outlineResult;
		rrRTBufferRequest colorRequest {output_viewport.size, core::gfx::tex::kColorFormatRGBA16F}; 
		renderer->CreateRenderTexture( colorRequest, &outlineResult );

		// Create render target output
		rrRenderTarget outlineOutput (&outlineResult);

		// Set output
		gfx->setRenderTarget(&outlineOutput.m_renderTarget);
		// Render the current result to the screen
		gfx->setViewport(output_viewport.corner.x, output_viewport.corner.y, output_viewport.corner.x + output_viewport.size.x, output_viewport.corner.y + output_viewport.size.y);
		gfx->setScissor(output_viewport.corner.x, output_viewport.corner.y, output_viewport.corner.x + output_viewport.size.x, output_viewport.corner.y + output_viewport.size.y);

		// Clear color
		{
			float clearColor[] = {0, 0, 0, 0};
			gfx->clearColor(clearColor);
		}

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

			// No blending. Only the source.
			gpu::BlendState bs;
			bs.enable = false;
			bs.src = gpu::kBlendModeOne;
			bs.dst = gpu::kBlendModeZero;
			bs.srcAlpha = gpu::kBlendModeOne;
			bs.dstAlpha = gpu::kBlendModeZero;
			bs.opAlpha = gpu::kBlendOpMax;
			gfx->setBlendState(bs);
		}

		gfx->setPipeline(m_postprocessTonemapPipeline);

		gfx->setShaderTexture(gpu::kShaderStagePs, 0, input_color, &linearSampler);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_CAMERA_INFORMATION, &cameraPass->m_cbuffer);
		gfx->setShaderCBuffer(gpu::kShaderStagePs, renderer::CBUFFER_PER_CAMERA_INFORMATION, &cameraPass->m_cbuffer);
		gfx->setVertexBuffer(0, &renderer->GetScreenQuadVertexBuffer(), 0); // see RrPipelinePasses.cpp
		gfx->setVertexBuffer(1, &renderer->GetScreenQuadVertexBuffer(), 0); // there are two binding slots defined with different stride

		gfx->draw(4, 0);

		// Save the current status
		result_color = outlineResult;
	}

	// done with samplers
	linearSampler.destroy(NULL);
	pointSampler.destroy(NULL);

	return result_color;
}