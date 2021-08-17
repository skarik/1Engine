#include "RrPipelinePaletted.h"

#include "renderer/camera/RrCamera.h"
#include "renderer/state/RrRenderer.h"
#include "renderer/material/RrShaderProgram.h"
#include "renderer/types/shaders/cbuffers.h"

#include "renderer/state/RaiiHelpers.h"

RrPipelinePalettedRenderer::~RrPipelinePalettedRenderer ( void )
{
	if (m_postprocessOutlineProgram)
		m_postprocessOutlineProgram->RemoveReference();

	if (m_postprocessOutlinePipeline)
	{
		m_postprocessOutlinePipeline->destroy(NULL);
		delete m_postprocessOutlinePipeline;
	}
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


rrCompositeOutput RrPipelinePalettedRenderer::CompositeDeferred ( gpu::GraphicsContext* gfx, const rrPipelineCompositeInput& compositeInput, RrOutputState* state )
{
	rrCompositeOutput compositeState = RrPipelineStandardRenderer::CompositeDeferred(gfx, compositeInput, state);

	auto renderer = RrRenderer::Active; // TODO: make argument

	// Grab outline shader
	GetPostprocess(renderer, "shaders/deferred_pass/shade_instanced_vv.spv", "shaders/postprocess/outline_p.spv",
		true,
		&m_postprocessOutlinePipeline,
		&m_postprocessOutlineProgram);

	// Grab output size for the screen quad info
	auto& output = *state->output_info;
	rrViewport output_viewport =  output.GetOutputViewport();

	// Allocate a new buffer for the outlines
	gpu::Texture outlineResult;
	rrRTBufferRequest colorRequest {output_viewport.size, core::gfx::tex::kColorFormatRGBA16F}; 
	renderer->CreateRenderTexture( colorRequest, &outlineResult );

	{
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

		gfx->setPipeline(m_postprocessOutlinePipeline);

		gfx->setShaderTextureAuto(gpu::kShaderStagePs, 0, &compositeState.color);
		gfx->setShaderTextureAuto(gpu::kShaderStagePs, 1, compositeInput.combined_depth);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_CAMERA_INFORMATION, &compositeInput.cameraPass->m_cbuffer);
		gfx->setShaderCBuffer(gpu::kShaderStagePs, renderer::CBUFFER_PER_CAMERA_INFORMATION, &compositeInput.cameraPass->m_cbuffer);
		gfx->setVertexBuffer(0, &renderer->GetScreenQuadVertexBuffer(), 0); // see RrPipelinePasses.cpp
		gfx->setVertexBuffer(1, &renderer->GetScreenQuadVertexBuffer(), 0); // there are two binding slots defined with different stride

		gfx->draw(4, 0);

		// Save the current status
		compositeState.color = outlineResult;
	}

	return compositeState;
}