#include "RrPipeline.h"
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

	// Helper to bind pipelines:
	auto BindPipelineAndRun = [gfx, &compositeInput, &renderer](
		gpu::Pipeline* pipeline,
		gpu::Buffer* cbuffer,
		gpu::Buffer* sbuffer,
		std::function<void(RrRenderer*, gpu::GraphicsContext*)> renderCall)
	{
		gfx->setPipeline(pipeline);
		gfx->setShaderTextureAuto(gpu::kShaderStagePs, 0, compositeInput.deferred_albedo);
		gfx->setShaderTextureAuto(gpu::kShaderStagePs, 1, compositeInput.deferred_normals);
		gfx->setShaderTextureAuto(gpu::kShaderStagePs, 2, compositeInput.deferred_surface);
		gfx->setShaderTextureAuto(gpu::kShaderStagePs, 3, compositeInput.deferred_emissive);
		gfx->setShaderTextureAuto(gpu::kShaderStagePs, 4, compositeInput.combined_depth);
		if (compositeInput.forward_color != nullptr)
		{
			gfx->setShaderTextureAuto(gpu::kShaderStagePs, 5, compositeInput.forward_color);
		}
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_CAMERA_INFORMATION, &compositeInput.cameraPass->m_cbuffer);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_USER0, cbuffer);
		gfx->setShaderCBuffer(gpu::kShaderStagePs, renderer::CBUFFER_USER0, cbuffer);
		gfx->setShaderSBuffer(gpu::kShaderStageVs, renderer::SBUFFER_USER0, sbuffer);
		gfx->setShaderSBuffer(gpu::kShaderStagePs, renderer::SBUFFER_USER0, sbuffer);
		renderCall(renderer, gfx);
	};

	// Debug output:
	if (gsesh_LightingUseDebugBuffers)
	{
		// Render with the composite shader
		BindPipelineAndRun(m_lightingCompositePipeline, nullptr, nullptr, [](RrRenderer* renderer, gpu::GraphicsContext* gfx)
		{
			gfx->setVertexBuffer(0, &renderer->GetScreenQuadVertexBuffer(), 0); // see RrPipelinePasses.cpp
			gfx->setVertexBuffer(1, &renderer->GetScreenQuadVertexBuffer(), 0); // there are two binding slots defined with different stride
			gfx->draw(4, 0);
		});
	}
	else
	{
		// Sort all the lights:
		SortLights();
		// Create a SBuffer with all the light information.
		int directionalLightFirstIndex = 0;
		int spotLightFirstIndex = 0;
		int omniLightFirstIndex = 0;
		gpu::Buffer lightParameterBuffer;
		{
			std::vector<renderer::cbuffer::rrLight> lights;
			lights.resize(directional_lights.size() + spot_lights.size() + omni_lights.size());

			int light_index = 0;
			directionalLightFirstIndex = light_index;
			for (RrLight* light : directional_lights)
			{
				lights[light_index] = renderer::cbuffer::rrLight(light);
				++light_index;
			}

			spotLightFirstIndex = light_index;
			for (RrLight* light : spot_lights)
			{
				lights[light_index] = renderer::cbuffer::rrLight(light);
				++light_index;
			}
			
			omniLightFirstIndex = light_index;
			for (RrLight* light : omni_lights)
			{
				lights[light_index] = renderer::cbuffer::rrLight(light);
				++light_index;
			}

			if (!lights.empty())
			{
				lightParameterBuffer.initAsStructuredBuffer(NULL, sizeof(renderer::cbuffer::rrLight) * lights.size());
				lightParameterBuffer.upload(gfx, lights.data(), sizeof(renderer::cbuffer::rrLight) * lights.size(), gpu::TransferStyle::kTransferStream);
			}
		}

		// Render the ambient lighting
		{
			// Render with the composite shader
			BindPipelineAndRun(m_lightingCompositePipeline, nullptr, nullptr, [](RrRenderer* renderer, gpu::GraphicsContext* gfx)
			{
				gfx->setVertexBuffer(0, &renderer->GetScreenQuadVertexBuffer(), 0); // see RrPipelinePasses.cpp
				gfx->setVertexBuffer(1, &renderer->GetScreenQuadVertexBuffer(), 0); // there are two binding slots defined with different stride
				gfx->draw(4, 0);
			});
		}

		{	// Switch to additive blending for all the lights
			gpu::BlendState bs;
			bs.enable = true;
			bs.src = gpu::kBlendModeSrcAlpha;
			bs.dst = gpu::kBlendModeOne;
			bs.srcAlpha = gpu::kBlendModeOne;
			bs.dstAlpha = gpu::kBlendModeOne;
			bs.opAlpha = gpu::kBlendOpMax;
			gfx->setBlendState(bs);
		}

		// Render directional lights as quads
		if (!directional_lights.empty())
		{
			int directionalLightCount = (int)directional_lights.size();

			struct rrCBufferLightInfo
			{
				int firstIndex;
			} cbuffer_light_params;
			cbuffer_light_params.firstIndex = directionalLightFirstIndex;

			gpu::Buffer cbuffer;
			cbuffer.initAsConstantBuffer(NULL, sizeof(rrCBufferLightInfo));
			cbuffer.upload(gfx, &cbuffer_light_params, sizeof(rrCBufferLightInfo), gpu::kTransferStream);

			// Render with the lighting0
			BindPipelineAndRun(m_lightingLighting0Pipeline, &cbuffer, &lightParameterBuffer, [directionalLightFirstIndex, directionalLightCount](RrRenderer* renderer, gpu::GraphicsContext* gfx)
			{
				gfx->setVertexBuffer(0, &renderer->GetScreenQuadVertexBuffer(), 0); // see RrPipelinePasses.cpp
				gfx->setVertexBuffer(1, &renderer->GetScreenQuadVertexBuffer(), 0); // there are two binding slots defined with different stride
				gfx->drawInstanced(4, directionalLightCount, 0);
			});

			cbuffer.free(NULL);
		}

		// TODO: Cannot bind depth buffer as depth buffer & texture at same time. Might need to sort instead, or depth-test in the shader
		{ // Go back to normal depth with omni lights.
			gpu::DepthStencilState ds;
			ds.depthTestEnabled   = true;
			ds.depthFunc          = gpu::kCompareOpLessEqual;
			ds.depthWriteEnabled  = false;
			ds.stencilTestEnabled = false;
			ds.stencilWriteMask   = 0x00;
			gfx->setDepthStencilState(ds);
		}

#if 0
		// Render point lights as spheres
		if (!omni_lights.empty())
		{
			// TODO: Change the mesh loader to be more general/universal, so the mesh can be grabbed.
			int omniLightCount = (int)omni_lights.size();
			rrCameraPass* cameraPass = compositeInput.cameraPass;

			struct rrCBufferLightInfo
			{
				int firstIndex;
			} cbuffer_light_params;
			cbuffer_light_params.firstIndex = omniLightFirstIndex;

			gpu::Buffer cbuffer;
			cbuffer.initAsConstantBuffer(NULL, sizeof(rrCBufferLightInfo));
			cbuffer.upload(gfx, &cbuffer_light_params, sizeof(rrCBufferLightInfo), gpu::kTransferStream);

			// Render with the lightingOmni
			BindPipelineAndRun(m_lightingLighting0Pipeline, &cbuffer, &lightParameterBuffer, [directionalLightFirstIndex, omniLightCount, cameraPass](RrRenderer* renderer, gpu::GraphicsContext* gfx)
			{
				gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_CAMERA_INFORMATION, &cameraPass->m_cbuffer);
				gfx->setVertexBuffer(0, &renderer->GetLightSphereVertexBuffer(), 0); // see RrPipelinePasses.cpp
				gfx->drawInstanced(4, omniLightCount, 0);
			});

			cbuffer.free(NULL);
		}
#endif

		// done with buffers
		lightParameterBuffer.free(NULL);

		{ // No depth/stencil test. Always draw.
			gpu::DepthStencilState ds;
			ds.depthTestEnabled   = false;
			ds.depthWriteEnabled  = false;
			ds.stencilTestEnabled = false;
			ds.stencilWriteMask   = 0x00;
			gfx->setDepthStencilState(ds);
		}

		// Render the forward data
		if (compositeInput.forward_color)
		{
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

	return rrCompositeOutput {outputLightingComposite};
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

void RrPipelineStandardRenderer::SortLights ( void )
{
	auto lightList = renderer::LightList();

	/*std::vector<RrLight*> directional_lights;
	std::vector<RrLight*> spot_lights;
	std::vector<RrLight*> omni_lights;*/
	directional_lights.clear();
	spot_lights.clear();
	omni_lights.clear();

	// Place all the lights into the scene into the listing.
	for (RrLight* light : lightList)
	{
		switch (light->type)
		{
		case kLightTypeDirectional:
			directional_lights.push_back(light);
			break;
		case kLightTypeSpotlight:
			spot_lights.push_back(light);
			break;
		case kLightTypeOmni:
			omni_lights.push_back(light);
			break;
		}
	}
}