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

static gpu::Texture Blur (
	rrRenderContext* context,
	gpu::Texture* input_nonblurred,
	const Vector2i texture_size,
	RrShaderProgram*& m_bloomBlurProgram,
	rrCameraPass* cameraPass,
	RrOutputState* state )
{
	auto renderer = RrRenderer::Active; // TODO: make argument or class field
	auto gfx = context->context_graphics;

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

	// Grab the shader program
	if (m_bloomBlurProgram == nullptr)
	{
		m_bloomBlurProgram = RrShaderProgram::Load(rrShaderProgramCs{"shaders/postprocess/bloom_blur_c.spv"});
	}

	// Allocate the X and XY results
	gpu::Texture colorBlurX;
	gpu::Texture colorBlurXY;

	rrRTBufferRequest colorRequest;

	colorRequest = {texture_size, core::gfx::tex::kColorFormatRGBA16F};
	renderer->CreateRenderTexture( colorRequest, &colorBlurX );
	colorRequest = {texture_size, core::gfx::tex::kColorFormatRGBA16F};
	renderer->CreateRenderTexture( colorRequest, &colorBlurXY );

	struct rrBlurParams
	{
		Vector2f	blur_vector = Vector2f(0, 0);
		int32		sample_count = 9;
	};

	// Blur on X
	{
		// Create blur params
		rrBlurParams blurParams { Vector2f(1.0F, 0.0F), 9 };
		gpu::Buffer cbBlurParams = context->constantBuffer_pool->Allocate( sizeof(blurParams) );
		cbBlurParams.upload(gfx, &blurParams, sizeof(blurParams), gpu::kTransferWriteDiscardPrevious);

		// Create writeable with the render target
		gpu::WriteableResource rwColor;
		rwColor.create(&colorBlurX, 0);

		// Clear out usage of render target to gain control of buffers
		gfx->setRenderTarget(NULL);

		// Set up compute shader
		gfx->setComputeShader(&m_bloomBlurProgram->GetShaderPipeline());
		gfx->setShaderTexture(gpu::kShaderStageCs, 0, input_nonblurred, &linearSampler);
		gfx->setShaderWriteable(gpu::kShaderStageCs, 1, &rwColor);
		gfx->setShaderCBuffer(gpu::kShaderStageCs, renderer::CBUFFER_USER0, &cbBlurParams);
		// Blur the buffer
		gfx->dispatch(texture_size.x / 8, texture_size.y / 8, 1);
		// Unbind the UAVs
		gfx->setShaderWriteable(gpu::kShaderStageCs, 1, NULL);

		// Done with writeables
		rwColor.destroy();
	}

	// Blur on Y
	{
		// Create blur params
		rrBlurParams blurParams { Vector2f(0.0F, 1.0F), 9 };
		gpu::Buffer cbBlurParams = context->constantBuffer_pool->Allocate( sizeof(blurParams) );
		cbBlurParams.upload(gfx, &blurParams, sizeof(blurParams), gpu::kTransferWriteDiscardPrevious);

		// Create writeable with the render target
		gpu::WriteableResource rwColor;
		rwColor.create(&colorBlurXY, 0);

		// Clear out usage of render target to gain control of buffers
		gfx->setRenderTarget(NULL);

		// Set up compute shader
		gfx->setComputeShader(&m_bloomBlurProgram->GetShaderPipeline());
		gfx->setShaderTexture(gpu::kShaderStageCs, 0, &colorBlurX, &linearSampler);
		gfx->setShaderWriteable(gpu::kShaderStageCs, 1, &rwColor);
		gfx->setShaderCBuffer(gpu::kShaderStageCs, renderer::CBUFFER_USER0, &cbBlurParams);
		// Blur the buffer
		gfx->dispatch(texture_size.x / 8, texture_size.y / 8, 1);
		// Unbind the UAVs
		gfx->setShaderWriteable(gpu::kShaderStageCs, 1, NULL);

		// Done with writeables
		rwColor.destroy();
	}

	// Free samplers now
	linearSampler.destroy(NULL);

	return colorBlurXY;
}

static core::settings::SessionSetting<bool> gsesh_BloomSetupUseCompute ("r_BloomSetupUseCompute", false);

RrPipelineStandardRenderer::rrBloomSetup RrPipelineStandardRenderer::SetupBloom (
	rrRenderContext* context,
	gpu::Texture* input_color,
	rrCameraPass* cameraPass,
	RrOutputState* state )
{
	auto renderer = RrRenderer::Active; // TODO: make argument or class field
	auto gfx = context->context_graphics;
	rrBloomSetup bloom;

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

	gfx->debugGroupPush("SetupBloom");

	// Perform the 4x and 16x downsample
	{
		// Grab output size for the screen quad info
		auto& output = *state->output_info;
		rrViewport output_viewport =  output.GetOutputViewport();

		// Allocate the 4 and 16 downscales
		gpu::Texture colorDownscale4;
		gpu::Texture colorDownscale16;

		rrRTBufferRequest colorRequest;

		colorRequest = {output_viewport.size / 4, core::gfx::tex::kColorFormatRGBA16F};
		renderer->CreateRenderTexture( colorRequest, &colorDownscale4 );
		colorRequest = {output_viewport.size / 16, core::gfx::tex::kColorFormatRGBA16F};
		renderer->CreateRenderTexture( colorRequest, &colorDownscale16 );

		const bool bUseCompute = gsesh_BloomSetupUseCompute;

		if (bUseCompute)
		{
			// Grab the shader program
			if (m_bloomDownscaleProgram == nullptr)
			{
				m_bloomDownscaleProgram = RrShaderProgram::Load(rrShaderProgramCs{"shaders/postprocess/bloom_downscale_c.spv"});
			}

			// Create writeable with the render target
			gpu::WriteableResource rwColorDownscale4;
			gpu::WriteableResource rwColorDownscale16;
			rwColorDownscale4.create(&colorDownscale4, 0);
			rwColorDownscale16.create(&colorDownscale16, 0);

			// Clear out usage of render target to gain control of buffers
			gfx->setRenderTarget(NULL);

			// Set up compute shader
			gfx->setComputeShader(&m_bloomDownscaleProgram->GetShaderPipeline());
			gfx->setShaderCBuffer(gpu::kShaderStageCs, renderer::CBUFFER_PER_CAMERA_INFORMATION, &cameraPass->m_cbuffer);
			gfx->setShaderWriteable(gpu::kShaderStageCs, 0, &rwColorDownscale4);
			gfx->setShaderWriteable(gpu::kShaderStageCs, 1, &rwColorDownscale16);
			gfx->setShaderTexture(gpu::kShaderStageCs, 2, input_color, &linearSampler);
			// Calculate the downscaled buffers
			gfx->dispatch(output_viewport.size.x / 4, output_viewport.size.y / 4, 1);
			// Unbind the UAVs
			gfx->setShaderWriteable(gpu::kShaderStageCs, 0, NULL);
			gfx->setShaderWriteable(gpu::kShaderStageCs, 1, NULL);

			// Done with the writeables
			rwColorDownscale4.destroy();
			rwColorDownscale16.destroy();
		}
		else
		{
			// Grab the shader program
			GetPostprocess(renderer, "shaders/deferred_pass/shade_common_vv.spv", "shaders/postprocess/bloom_downscale_p.spv",
				true,
				&m_bloomDownscaleVsPsPipeline,
				&m_bloomDownscaleVsPsProgram);

			// Create render target output
			rrRenderTarget output4x (&colorDownscale4);
			rrRenderTarget output16x (&colorDownscale16);

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

			struct rrDownscaleParams
			{
				Vector2f	input_size = Vector2f(0, 0);
				Vector2f	output_size = Vector2f(0, 0);
				float		bloom_bias = 0.95F;
			};

			// 4X Downscale
			{
				// Create blur params
				rrDownscaleParams downscaleParams { Vector2f(output_viewport.size.x, output_viewport.size.y), Vector2f(output_viewport.size.x / 4, output_viewport.size.y / 4), 0.95F };
				gpu::Buffer cbBlurParams = context->constantBuffer_pool->Allocate( sizeof(downscaleParams) );
				cbBlurParams.upload(gfx, &downscaleParams, sizeof(downscaleParams), gpu::kTransferWriteDiscardPrevious);

				// Set output
				gfx->setRenderTarget(&output4x.m_renderTarget);
				// Render the current result to the screen
				gfx->setViewport(output_viewport.corner.x / 4, output_viewport.corner.y / 4, (output_viewport.corner.x + output_viewport.size.x) / 4, (output_viewport.corner.y + output_viewport.size.y) / 4);
				gfx->setScissor(output_viewport.corner.x / 4, output_viewport.corner.y / 4, (output_viewport.corner.x + output_viewport.size.x) / 4, (output_viewport.corner.y + output_viewport.size.y) / 4);
				// Set up params
				gfx->setPipeline(m_bloomDownscaleVsPsPipeline);
				gfx->setShaderTextureAuto(gpu::kShaderStagePs, 0, input_color);
				gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_CAMERA_INFORMATION, &cameraPass->m_cbuffer);
				gfx->setShaderCBuffer(gpu::kShaderStagePs, renderer::CBUFFER_PER_CAMERA_INFORMATION, &cameraPass->m_cbuffer);
				gfx->setShaderCBuffer(gpu::kShaderStagePs, renderer::CBUFFER_USER0, &cbBlurParams);
				gfx->setVertexBuffer(0, &renderer->GetScreenQuadVertexBuffer(), 0); // see RrPipelinePasses.cpp
				gfx->setVertexBuffer(1, &renderer->GetScreenQuadVertexBuffer(), 0); // there are two binding slots defined with different stride
				// Render
				gfx->draw(4, 0);
			}

			// 16X Downscale (4X downscale the 4X)
			{
				// Create blur params
				rrDownscaleParams downscaleParams { Vector2f(output_viewport.size.x / 4, output_viewport.size.y / 4), Vector2f(output_viewport.size.x / 16, output_viewport.size.y / 16), 0.0F };
				gpu::Buffer cbBlurParams = context->constantBuffer_pool->Allocate( sizeof(downscaleParams) );
				cbBlurParams.upload(gfx, &downscaleParams, sizeof(downscaleParams), gpu::kTransferWriteDiscardPrevious);

				// Set output
				gfx->setRenderTarget(&output16x.m_renderTarget);
				// Render the current result to the screen
				gfx->setViewport(output_viewport.corner.x / 16, output_viewport.corner.y / 16, (output_viewport.corner.x + output_viewport.size.x) / 16, (output_viewport.corner.y + output_viewport.size.y) / 16);
				gfx->setScissor(output_viewport.corner.x / 16, output_viewport.corner.y / 16, (output_viewport.corner.x + output_viewport.size.x) / 16, (output_viewport.corner.y + output_viewport.size.y) / 16);
				// Set up params
				gfx->setPipeline(m_bloomDownscaleVsPsPipeline);
				gfx->setShaderTextureAuto(gpu::kShaderStagePs, 0, &colorDownscale4);
				gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_CAMERA_INFORMATION, &cameraPass->m_cbuffer);
				gfx->setShaderCBuffer(gpu::kShaderStagePs, renderer::CBUFFER_PER_CAMERA_INFORMATION, &cameraPass->m_cbuffer);
				gfx->setShaderCBuffer(gpu::kShaderStagePs, renderer::CBUFFER_USER0, &cbBlurParams);
				gfx->setVertexBuffer(0, &renderer->GetScreenQuadVertexBuffer(), 0); // see RrPipelinePasses.cpp
				gfx->setVertexBuffer(1, &renderer->GetScreenQuadVertexBuffer(), 0); // there are two binding slots defined with different stride
				// Render
				gfx->draw(4, 0);
			}
		}

		// Save the results
		bloom.m_colorDownscale4 = colorDownscale4;
		bloom.m_colorDownscale16 = colorDownscale16;
	}

	// Blur both the 4x and 16x downsample on X and Y
	{
		// Grab output size for the screen quad info
		auto& output = *state->output_info;
		rrViewport output_viewport =  output.GetOutputViewport();

		bloom.m_colorDownscale16_Blurred = Blur(context, &bloom.m_colorDownscale16, output_viewport.size / 16, m_bloomBlurProgram, cameraPass, state);
		bloom.m_colorDownscale16_Blurred = Blur(context, &bloom.m_colorDownscale16_Blurred, output_viewport.size / 16, m_bloomBlurProgram, cameraPass, state);
		bloom.m_colorDownscale4_Blurred = Blur(context, &bloom.m_colorDownscale4, output_viewport.size / 4, m_bloomBlurProgram, cameraPass, state);
	}

	gfx->debugGroupPop();

	// Free samplers now
	linearSampler.destroy(NULL);

	return bloom;
}

gpu::Texture
RrPipelineStandardRenderer::ApplyBloom (
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
	GetPostprocess(renderer, "shaders/deferred_pass/shade_common_vv.spv", "shaders/postprocess/bloom_p.spv",
		true,
		&m_postprocessBloomPipeline,
		&m_postprocessBloomProgram);

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

		gfx->setPipeline(m_postprocessBloomPipeline);

		gfx->setShaderTexture(gpu::kShaderStagePs, 0, input_color, &linearSampler);
		gfx->setShaderTexture(gpu::kShaderStagePs, 1, &bloom_setup->m_colorDownscale16_Blurred, &linearSampler);
		gfx->setShaderTexture(gpu::kShaderStagePs, 2, &bloom_setup->m_colorDownscale4_Blurred, &linearSampler);
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