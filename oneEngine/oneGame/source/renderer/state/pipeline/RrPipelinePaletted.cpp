#include "RrPipelinePaletted.h"

#include "core-ext/system/io/Resources.h"
#include "core-ext/system/io/assets/texloader/png.hpp"
#include "core-ext/threads/ParallelFor.h"

#include "gpuw/Sampler.h"

#include "renderer/camera/RrCamera.h"
#include "renderer/state/RrRenderer.h"
#include "renderer/material/RrShaderProgram.h"
#include "renderer/types/shaders/cbuffers.h"

#include "renderer/state/RaiiHelpers.h"

RrPipelinePalettedOptions::~RrPipelinePalettedOptions ( void )
{
	m_paletteLUT_Primary.free();
	m_paletteLUT_Secondary.free();
}

struct rrColorDistance
{
	core::gfx::arPixel	color;
	float				distance = math::square(256.0F) * 3.0F;
};

//	FindClosestColors( lookup_position ) : Returns the closest color in the image to the given position.
template <int DistanceCount>
static void FindClosestColors(core::gfx::arPixel& lookup, const core::gfx::tex::arImageInfo& paletteInfo, const core::gfx::arPixel* paletteData, rrColorDistance* io_distances)
{
	// Set initial colors for safety
	for (int i = 0; i < DistanceCount; ++i)
	{
		io_distances[i].color = paletteData[0];
	}

	for (uint pixel_x = 0; pixel_x < paletteInfo.width; ++pixel_x)
	{
		for (uint pixel_y = 0; pixel_y < paletteInfo.height; ++pixel_y)
		{
			core::gfx::arPixel newColor = paletteData[pixel_x + pixel_y * paletteInfo.height];

			// Calculate the distance of this new color from the position
			float distance =
				math::square((float)((int)newColor.r - (int)lookup.r))
				+ math::square((float)((int)newColor.g - (int)lookup.g))
				+ math::square((float)((int)newColor.b - (int)lookup.b));

			for (int i = 0; i < DistanceCount; ++i)
			{
				// If this distance is smaller, we have a new closest distance
				if ( distance < io_distances[i].distance )
				{
					// Shift all the other values downward
					for (int shiftIndex = DistanceCount - 1; shiftIndex > i; --shiftIndex)
					{
						io_distances[shiftIndex] = io_distances[shiftIndex - 1];
					}

					// Save the new highest
					io_distances[i].distance = distance;
					io_distances[i].color = newColor;

					// Break out of the loop since we've already inserted the value.
					break;
				}
			}
		}
	}
};

void RrPipelinePalettedOptions::LoadPalette ( const char* resource_name )
{
	FILE* palette_file = core::Resources::Open( resource_name, "rb" );
	if (palette_file == NULL)
	{
		ARCORE_ERROR("Invalid resource %s given for a palette.", resource_name);
		return;
	}

	// Open the PNG and read the data:
	core::gfx::tex::arImageInfo imageInfo = {};
	core::gfx::arPixel* pixel = core::texture::loadPNG(palette_file, imageInfo);
	if (pixel == NULL)
	{
		ARCORE_ERROR("No pixel data retreived from palette %s, check file.", resource_name);
		return;
	}

	// Create a 3D LUT for data
	core::gfx::arPixel* pixelsLUT_Primary = new core::gfx::arPixel [ 256 * 256 * 256 ];
	core::gfx::arPixel* pixelsLUT_Secondary = new core::gfx::arPixel [ 256 * 256 * 256 ];

	// Generate the closest color for each LUT.
	core::parallel_for(true, 0, 256, [pixelsLUT_Primary, pixelsLUT_Secondary, &imageInfo, pixel](const int sliceX)
		{
			for (int sliceY = 0; sliceY < 256; ++sliceY)
			{
				for (int pixelZ = 0; pixelZ < 256; ++pixelZ)
				{
					rrColorDistance closestColors [2];
					FindClosestColors<2>(core::gfx::arPixel((uint8)sliceX, (uint8)sliceY, (uint8)pixelZ, 255), imageInfo, pixel, closestColors);

					pixelsLUT_Primary[sliceX + sliceY * 256 + pixelZ * 256 * 256] = closestColors[0].color;
					pixelsLUT_Secondary[sliceX + sliceY * 256 + pixelZ * 256 * 256] = closestColors[1].color;
				}
			}
		});

	// Free source data
	delete[] pixel;

	// Upload the textures
	gpu::Buffer uploadBufferPrimary;
	gpu::Buffer uploadBufferSecondary;

	uploadBufferPrimary.initAsTextureBuffer( NULL, core::gfx::tex::kTextureType3D, core::gfx::tex::kColorFormatRGBA8, 256, 256, 256 );
	uploadBufferSecondary.initAsTextureBuffer( NULL, core::gfx::tex::kTextureType3D, core::gfx::tex::kColorFormatRGBA8, 256, 256, 256 );

	uploadBufferPrimary.upload( NULL, pixelsLUT_Primary, sizeof( core::gfx::arPixel ) * 256 * 256 * 256, gpu::kTransferStatic );
	uploadBufferSecondary.upload( NULL, pixelsLUT_Secondary, sizeof( core::gfx::arPixel ) * 256 * 256 * 256, gpu::kTransferStatic );

	m_paletteLUT_Primary.allocate( core::gfx::tex::kTextureType3D, core::gfx::tex::kColorFormatRGBA8, 256, 256, 256, 1 );
	m_paletteLUT_Secondary.allocate( core::gfx::tex::kTextureType3D, core::gfx::tex::kColorFormatRGBA8, 256, 256, 256, 1 );

	m_paletteLUT_Primary.upload( NULL, uploadBufferPrimary, 0, 0 );
	m_paletteLUT_Secondary.upload( NULL, uploadBufferSecondary, 0, 0 );
	
	// TODO: add a semaphore here so we can wait for the upload to finish.

	uploadBufferPrimary.free(NULL);
	uploadBufferSecondary.free(NULL);
	
	delete[] pixelsLUT_Primary;
	delete[] pixelsLUT_Secondary;
}

RrPipelinePalettedRenderer::~RrPipelinePalettedRenderer ( void )
{
	if (m_postprocessOutlineProgram)
		m_postprocessOutlineProgram->RemoveReference();

	if (m_postprocessOutlinePipeline)
	{
		m_postprocessOutlinePipeline->destroy(NULL);
		delete m_postprocessOutlinePipeline;
	}

	if (m_postprocessPalettizeProgram)
		m_postprocessPalettizeProgram->RemoveReference();

	if (m_postprocessPalettizePipeline)
	{
		m_postprocessPalettizePipeline->destroy(NULL);
		delete m_postprocessPalettizePipeline;
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
	return compositeState;
}

//	CompositePostOpaques() : Called when the renderer is done rendering all deferred+forward opaques.
rrCompositeOutput RrPipelinePalettedRenderer::CompositePostOpaques ( gpu::GraphicsContext* gfx, const rrPipelinePostOpaqueCompositeInput& compositeInput, RrOutputState* state )
{
	gpu::Texture outputColor = ApplyOutline(gfx, compositeInput.combined_color, compositeInput.combined_depth, compositeInput.cameraPass, state);
	return rrCompositeOutput{outputColor};
}

//	RenderLayerEnd() : Called when the renderer finishes a given layer.
rrPipelineOutput RrPipelinePalettedRenderer::RenderLayerEnd ( gpu::GraphicsContext* gfx, const rrPipelineLayerFinishInput& finishInput, RrOutputState* state )
{
	if ( finishInput.layer != renderer::kRenderLayerWorld )
	{
		return RrPipelineStandardRenderer::RenderLayerEnd(gfx, finishInput, state);
	}
	else
	{
		gpu::Texture outputColor = *finishInput.color;
		
		// Setup the bloom
		auto bloomSetup = SetupBloom(gfx, &outputColor, finishInput.cameraPass, state);
		auto tonemapSetup = SetupTonemap(gfx, &outputColor, state);
		auto exposureSetup = SetupExposure(gfx, &m_previousFrameOutput, state);

		// Apply the tonemap before the stylistic effect
		outputColor = ApplyTonemap(gfx, &outputColor, &bloomSetup, &tonemapSetup, &exposureSetup, finishInput.cameraPass, state);

		// Before bloom, do stylistic effect
		outputColor = ApplyPalettize(gfx, &outputColor, finishInput.cameraPass, state);

		// Apply the bloom
		outputColor = ApplyBloom(gfx, &outputColor, &bloomSetup, &tonemapSetup, &exposureSetup, finishInput.cameraPass, state);

		// Save & analyze the color
		SaveAndAnalyzeOutput(gfx, &outputColor, state);

		return rrPipelineOutput{outputColor};
	}
}

gpu::Texture RrPipelinePalettedRenderer::ApplyOutline (
		gpu::GraphicsContext* gfx,
		gpu::Texture* color,
		gpu::Texture* depth,
		rrCameraPass* cameraPass,
		RrOutputState* state )
{
	auto renderer = RrRenderer::Active; // TODO: make argument
	
	const bool bEnableOutlines = true;

	gpu::Texture result_color = *color;

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

	// Grab outline shader
	GetPostprocess(renderer, "shaders/deferred_pass/shade_common_vv.spv", "shaders/postprocess/outline_p.spv",
		true,
		&m_postprocessOutlinePipeline,
		&m_postprocessOutlineProgram);

	// Grab output size for the screen quad info
	auto& output = *state->output_info;
	rrViewport output_viewport =  output.GetOutputViewport();

	if (bEnableOutlines)
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

		gfx->setPipeline(m_postprocessOutlinePipeline);

		gfx->setShaderTextureAuto(gpu::kShaderStagePs, 0, color);
		gfx->setShaderTexture(gpu::kShaderStagePs, 1, depth);
		gfx->setShaderSampler(gpu::kShaderStagePs, 1, &linearSampler);
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

gpu::Texture RrPipelinePalettedRenderer::ApplyPalettize (
	gpu::GraphicsContext* gfx,
	gpu::Texture* color,
	rrCameraPass* cameraPass,
	RrOutputState* state )
{
	auto renderer = RrRenderer::Active; // TODO: make argument
	auto options = (RrPipelinePalettedOptions*)m_options;

	const bool bEnablePalettize = true;

	gpu::Texture result_color = *color;

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
	GetPostprocess(renderer, "shaders/deferred_pass/shade_common_vv.spv", "shaders/postprocess/palettize_p.spv",
		true,
		&m_postprocessPalettizePipeline,
		&m_postprocessPalettizeProgram);

	// Grab output size for the screen quad info
	auto& output = *state->output_info;
	rrViewport output_viewport =  output.GetOutputViewport();

	if (bEnablePalettize)
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

		gfx->setPipeline(m_postprocessPalettizePipeline);

		gfx->setShaderTextureAuto(gpu::kShaderStagePs, 0, color);
		//gfx->setShaderTexture(gpu::kShaderStagePs, 1, compositeInput.combined_depth, &linearSampler);
		gfx->setShaderTexture(gpu::kShaderStagePs, 2, &options->m_paletteLUT_Primary, &pointSampler);
		gfx->setShaderTexture(gpu::kShaderStagePs, 3, &options->m_paletteLUT_Secondary, &pointSampler);
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