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
#include "gpuw/WriteableResource.h"

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
	// TODO: A lot of repeated code. A better way to handle this?

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

	if (m_hzbGenerationProgram)
		m_hzbGenerationProgram->RemoveReference();

	if (m_shadowingProjectionProgram)
		m_shadowingProjectionProgram->RemoveReference();

	if (m_shadowingContactShadowProgram)
		m_shadowingContactShadowProgram->RemoveReference();

	if (m_bloomDownscaleProgram)
		m_bloomDownscaleProgram->RemoveReference();

	if (m_bloomBlurProgram)
		m_bloomBlurProgram->RemoveReference();

	if (m_postprocessBloomProgram)
		m_postprocessBloomProgram->RemoveReference();

	if (m_postprocessBloomPipeline)
	{
		m_postprocessBloomPipeline->destroy(NULL);
		delete m_postprocessBloomPipeline;
	}

	if (m_postprocessTonemapProgram)
		m_postprocessTonemapProgram->RemoveReference();

	if (m_postprocessTonemapPipeline)
	{
		m_postprocessTonemapPipeline->destroy(NULL);
		delete m_postprocessTonemapPipeline;
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

void RrPipelineStandardRenderer::PostDepth ( gpu::GraphicsContext* gfx, const rrPipelinePostDepthInput& postDepthInput, RrOutputState* state )
{
	// Create HZB needed for some effects
	GenerateHZB(gfx, postDepthInput.combined_depth, postDepthInput.cameraPass, state);
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
			//ARCORE_ERROR("TODO: create a general geometry pipeline for the shader, for loaded meshes.");

			// create the pipeline
			gpu::VertexInputBindingDescription binding_desc [2];
			binding_desc[0].binding = 0;
			binding_desc[0].stride = sizeof(Vector4f);
			binding_desc[0].inputRate = gpu::kInputRatePerVertex;

			gpu::VertexInputAttributeDescription attrib_desc [3];
			attrib_desc[0].binding = 0;
			attrib_desc[0].offset = 0;
			attrib_desc[0].location = (uint32_t)renderer::shader::Location::kPosition;
			attrib_desc[0].format = gpu::kFormatR32G32B32SFloat;

			gpu::PipelineCreationDescription desc;
			desc.shader_pipeline = &(*cachedProgram)->GetShaderPipeline();
			desc.vv_inputBindings = binding_desc;
			desc.vv_inputBindingsCount = 1;
			desc.vv_inputAttributes = attrib_desc;
			desc.vv_inputAttributesCount = 1;
			desc.ia_topology = gpu::kPrimitiveTopologyTriangleStrip;
			desc.ia_primitiveRestartEnable = false;
			(*cachedPipeline)->create(NULL, &desc);
		}
	}
}

#include "../.res-1/shaders/deferred_pass/shade_lighting_p.variants.h"

static core::settings::SessionSetting<bool> gsesh_LightingUseDebugBuffers ("rdbg_deferred_gbuffers", false);
static core::settings::SessionSetting<bool> gsesh_LightingUseLighting ("rdbg_deferred_lighting", true);

rrCompositeOutput RrPipelineStandardRenderer::CompositeDeferred ( gpu::GraphicsContext* gfx, const rrPipelineCompositeInput& compositeInput, RrOutputState* state )
{
	auto renderer = RrRenderer::Active; // TODO: make argument
	auto options = (RrPipelineStandardOptions*)m_options;

	{
		RR_SHADER_VARIANT(shade_lighting_p) l_shadeLightingVariantInfo;
		l_shadeLightingVariantInfo.VARIANT_STYLE = options->m_celShadeLighting
			? l_shadeLightingVariantInfo.VARIANT_STYLE_CELSHADED
			: l_shadeLightingVariantInfo.VARIANT_STYLE_NORMAL;
		l_shadeLightingVariantInfo.VARIANT_PASS = gsesh_LightingUseDebugBuffers
			? l_shadeLightingVariantInfo.VARIANT_PASS_DEBUG_SURFACE
			: l_shadeLightingVariantInfo.VARIANT_PASS_DO_INDIRECT_EMISSIVE;

		GetPostprocessVariant(renderer, "shaders/deferred_pass/shade_common_vv.spv", "shaders/deferred_pass/",
			l_shadeLightingVariantInfo, true,
			&m_lightingCompositePipeline, &m_lightingCompositeProgram);
	}

	{
		RR_SHADER_VARIANT(shade_lighting_p) l_shadeLightingVariantInfo;
		l_shadeLightingVariantInfo.VARIANT_STYLE = options->m_celShadeLighting
			? l_shadeLightingVariantInfo.VARIANT_STYLE_CELSHADED
			: l_shadeLightingVariantInfo.VARIANT_STYLE_NORMAL;
		l_shadeLightingVariantInfo.VARIANT_PASS = l_shadeLightingVariantInfo.VARIANT_PASS_DO_DIRECT_DIRECTIONAL;

		GetPostprocessVariant(renderer, "shaders/deferred_pass/shade_instanced_vv.spv", "shaders/deferred_pass/",
			l_shadeLightingVariantInfo, true,
			&m_lightingLighting0Pipeline,
			&m_lightingLighting0Program);
	}

#if 1
	{
		RR_SHADER_VARIANT(shade_lighting_p) l_shadeLightingVariantInfo;
		l_shadeLightingVariantInfo.VARIANT_STYLE = options->m_celShadeLighting
			? l_shadeLightingVariantInfo.VARIANT_STYLE_CELSHADED
			: l_shadeLightingVariantInfo.VARIANT_STYLE_NORMAL;
		l_shadeLightingVariantInfo.VARIANT_PASS = l_shadeLightingVariantInfo.VARIANT_PASS_DO_DIRECT_OMNI;

		GetPostprocessVariant(renderer, "shaders/deferred_pass/shade_worldproj_instanced_vv.spv", "shaders/deferred_pass/",
			l_shadeLightingVariantInfo, false,
			&m_lightingLightingOmniPipeline,
			&m_lightingLightingOmniProgram);
	}
#endif
	gfx->debugGroupPush("Setup Lights");
	rrLightSetup lightSetup;
	{
		// Render the lighting
		lightSetup = SetupLights(gfx);
		
		// Render shadows now
		RenderShadows(gfx, compositeInput.deferred_normals, compositeInput.combined_depth, compositeInput.cameraPass, state, &lightSetup);
	}
	gfx->debugGroupPop();

	gfx->debugGroupPush("RrPipelineStandardRenderer::CompositeDeferred");
	gpu::Texture outputLightingComposite;
	{
		// Grab output size for the screen quad info
		auto& output = *state->output_info;
		rrViewport output_viewport =  output.GetOutputViewport();

		// Allocate a new buffer 
		rrRTBufferRequest colorRequest {output_viewport.size, core::gfx::tex::kColorFormatRGBA16F}; 
		renderer->CreateRenderTexture( colorRequest, &outputLightingComposite );

		// Clear the new render texture
		{
			// Create render target output
			rrRenderTarget rtLightingOutput (&outputLightingComposite);

			// Set up the output
			{
				// Set output
				gfx->setRenderTarget(&rtLightingOutput.m_renderTarget);
				// Render the current result to the screen
				gfx->setViewport(output_viewport.corner.x, output_viewport.corner.y, output_viewport.corner.x + output_viewport.size.x, output_viewport.corner.y + output_viewport.size.y);
				gfx->setScissor(output_viewport.corner.x, output_viewport.corner.y, output_viewport.corner.x + output_viewport.size.x, output_viewport.corner.y + output_viewport.size.y);
			}

			// Clear to empty black
			{
				float clearColor[] = {0, 0, 0, 0};
				gfx->clearColor(clearColor);
			}
		}

		// Debug output:
		if (gsesh_LightingUseDebugBuffers)
		{
			DrawDebugOutput(gfx, compositeInput, state, outputLightingComposite);
		}
		else
		{
			// Composite lights onto the scene
			outputLightingComposite = RenderLights(gfx, compositeInput, state, &lightSetup, outputLightingComposite);

			// Render the old forward data
			if (compositeInput.old_forward_color)
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
				gfx->setShaderTextureAuto(gpu::kShaderStagePs, 0, compositeInput.old_forward_color);

				gfx->draw(4, 0);
			}
		}
	}
	gfx->debugGroupPop();

	return rrCompositeOutput {outputLightingComposite};
}

void RrPipelineStandardRenderer::DrawDebugOutput (
	gpu::GraphicsContext* gfx,
	const rrPipelineCompositeInput& compositeInput,
	RrOutputState* state,
	gpu::Texture clearedOutputTexture)
{
	// Grab output size for the screen quad info
	auto& output = *state->output_info;
	rrViewport output_viewport =  output.GetOutputViewport();

	// Create render target output
	rrRenderTarget rtLightingOutput (&clearedOutputTexture);

	// Set up the output
	{
		// Set output
		gfx->setRenderTarget(&rtLightingOutput.m_renderTarget);
		// Render the current result to the screen
		gfx->setViewport(output_viewport.corner.x, output_viewport.corner.y, output_viewport.corner.x + output_viewport.size.x, output_viewport.corner.y + output_viewport.size.y);
		gfx->setScissor(output_viewport.corner.x, output_viewport.corner.y, output_viewport.corner.x + output_viewport.size.x, output_viewport.corner.y + output_viewport.size.y);
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

void RrPipelineStandardRenderer::GenerateHZB (
	gpu::GraphicsContext* gfx,
	gpu::Texture* combined_depth,
	rrCameraPass* cameraPass,
	RrOutputState* state)
{
	auto renderer = RrRenderer::Active; // TODO: make argument or class field

	gfx->debugGroupPush("GenerateHZB");

	// Grab output size for the screen quad info
	auto& output = *state->output_info;
	rrViewport output_viewport =  output.GetOutputViewport();

	// Allocate the 4 and 16 downscales
	gpu::Texture depthInfoDownscale4;
	gpu::Texture depthInfoDownscale16;

	rrRTBufferRequest depthInfoRequest;

	depthInfoRequest = {output_viewport.size / 4, core::gfx::tex::kColorFormatRG32F};
	renderer->CreateRenderTexture( depthInfoRequest, &depthInfoDownscale4 );
	depthInfoRequest = {output_viewport.size / 16, core::gfx::tex::kColorFormatRG32F};
	renderer->CreateRenderTexture( depthInfoRequest, &depthInfoDownscale16 );

	// Grab the shader program
	if (m_hzbGenerationProgram == nullptr)
	{
		m_hzbGenerationProgram = RrShaderProgram::Load(rrShaderProgramCs{"shaders/deferred_pass/hzb_generate_c.spv"});
	}

	// Clear out usage of render target to gain control of depth
	gfx->setRenderTarget(NULL);

	// Create writeable with the render target
	gpu::WriteableResource rwDepthInfoDownscale4;
	gpu::WriteableResource rwDepthInfoDownscale16;
	rwDepthInfoDownscale4.create(&depthInfoDownscale4, 0);
	rwDepthInfoDownscale16.create(&depthInfoDownscale16, 0);

	// Set up compute shader
	gfx->setComputeShader(&m_hzbGenerationProgram->GetShaderPipeline());
	gfx->setShaderCBuffer(gpu::kShaderStageCs, renderer::CBUFFER_PER_CAMERA_INFORMATION, &cameraPass->m_cbuffer);
	gfx->setShaderWriteable(gpu::kShaderStageCs, 0, &rwDepthInfoDownscale4);
	gfx->setShaderWriteable(gpu::kShaderStageCs, 1, &rwDepthInfoDownscale16);
	gfx->setShaderTextureAuto(gpu::kShaderStageCs, 2, combined_depth);
	// Render the contact shadows
	gfx->dispatch(output_viewport.size.x / 4, output_viewport.size.y / 4, 1);
	// Unbind the UAVs
	gfx->setShaderWriteable(gpu::kShaderStageCs, 0, NULL);
	gfx->setShaderWriteable(gpu::kShaderStageCs, 1, NULL);

	// Done with the writeables
	rwDepthInfoDownscale4.destroy();
	rwDepthInfoDownscale16.destroy();

	// Save the results
	hzb_4 = depthInfoDownscale4;
	hzb_16 = depthInfoDownscale16;

	gfx->debugGroupPop();
}

rrCompositeOutput RrPipelineStandardRenderer::CompositePostOpaques ( gpu::GraphicsContext* gfx, const rrPipelinePostOpaqueCompositeInput& compositeInput, RrOutputState* state )
{
	if (compositeInput.layer != renderer::kRenderLayerWorld)
	{
		return RrPipelineStateRenderer::CompositePostOpaques(gfx, compositeInput, state);
	}
	else
	{
		return RrPipelineStateRenderer::CompositePostOpaques(gfx, compositeInput, state);
		// TODO: Ambient occlusion, motion blur, and other simple shadowing?
	}
}

rrPipelineOutput RrPipelineStandardRenderer::RenderLayerEnd ( gpu::GraphicsContext* gfx, const rrPipelineLayerFinishInput& finishInput, RrOutputState* state ) 
{
	if (finishInput.layer != renderer::kRenderLayerWorld)
	{
		return RrPipelineStateRenderer::RenderLayerEnd(gfx, finishInput, state);
	}
	else
	{
		gpu::Texture outputColor = *finishInput.color;
		
		// Setup the bloom
		auto bloomSetup = SetupBloom(gfx, &outputColor, finishInput.cameraPass, state);
		auto tonemapSetup = SetupTonemap(gfx, &outputColor, state);
		auto exposureSetup = SetupExposure(gfx, &m_previousFrameOutput, state);

		// Apply the tonemap
		outputColor = ApplyTonemap(gfx, &outputColor, &bloomSetup, &tonemapSetup, &exposureSetup, finishInput.cameraPass, state);
		// Apply the bloom
		outputColor = ApplyBloom(gfx, &outputColor, &bloomSetup, &tonemapSetup, &exposureSetup, finishInput.cameraPass, state);

		// Save & analyze the color
		SaveAndAnalyzeOutput(gfx, &outputColor, state);

		return rrPipelineOutput{outputColor};
	}
}

void RrPipelineStandardRenderer::SaveAndAnalyzeOutput ( gpu::GraphicsContext* gfx, gpu::Texture* final_output_color, RrOutputState* state )
{
	// Copy output color to m_previousFrameOutputColor
	CopyRenderTexture(&m_previousFrameOutput.m_color, final_output_color, gfx, state);

	// Run tonemap prep on the previous frame
}