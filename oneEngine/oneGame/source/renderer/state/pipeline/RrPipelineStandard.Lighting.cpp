#include "RrPipelineStandard.h"

#include "core-ext/settings/SessionSettings.h"

#include "renderer/light/RrLight.h"
#include "renderer/object/RrRenderObject.h"
#include "renderer/camera/RrCamera.h"
#include "renderer/state/RrRenderer.h"
#include "renderer/types/shaders/cbuffers.h"
#include "renderer/types/shaders/sbuffers.h"

#include "renderer/state/RaiiHelpers.h"

#include "gpuw/Pipeline.h"
#include "gpuw/ShaderPipeline.h"

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

RrPipelineStandardRenderer::rrLightSetup RrPipelineStandardRenderer::SetupLights(
	gpu::GraphicsContext* gfx)
{
	rrLightSetup setup;
	
	// Sort all the lights:
	SortLights();
	// Create a SBuffer with all the light information.
	{
		std::vector<renderer::cbuffer::rrLight> lights;
		lights.resize(directional_lights.size() + spot_lights.size() + omni_lights.size());

		int light_index = 0;
		setup.directionalLightFirstIndex = light_index;
		for (RrLight* light : directional_lights)
		{
			lights[light_index] = renderer::cbuffer::rrLight(light);
			++light_index;
		}

		setup.spotLightFirstIndex = light_index;
		for (RrLight* light : spot_lights)
		{
			lights[light_index] = renderer::cbuffer::rrLight(light);
			++light_index;
		}
			
		setup.omniLightFirstIndex = light_index;
		for (RrLight* light : omni_lights)
		{
			lights[light_index] = renderer::cbuffer::rrLight(light);
			++light_index;
		}

		if (!lights.empty())
		{
			setup.lightParameterBuffer = new gpu::Buffer();
			setup.lightParameterBuffer->initAsStructuredBuffer(NULL, sizeof(renderer::cbuffer::rrLight) * lights.size());
			setup.lightParameterBuffer->upload(gfx, lights.data(), sizeof(renderer::cbuffer::rrLight) * lights.size(), gpu::TransferStyle::kTransferStream);
		}
	}

	return setup;
}

void RrPipelineStandardRenderer::RenderShadows(
	gpu::GraphicsContext* gfx,
	const rrPipelineCompositeInput& gbuffers,
	rrLightSetup* lightSetup)
{
	gfx->debugGroupPush("RenderShadows");
	gfx->debugGroupPop();
}

gpu::Texture RrPipelineStandardRenderer::RenderLights(
	gpu::GraphicsContext* gfx,
	const rrPipelineCompositeInput& gbuffers,
	RrOutputState* state,
	rrLightSetup* lightSetup,
	gpu::Texture clearedOutputTexture)
{
	gfx->debugGroupPush("RenderLights");

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

	// Render the ambient lighting
	{
		// Render with the composite shader
		DrawWithPipelineAndGBuffers(gfx, gbuffers, m_lightingCompositePipeline, nullptr, nullptr, [](RrRenderer* renderer, gpu::GraphicsContext* gfx)
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
		int directionalLightFirstIndex = lightSetup->directionalLightFirstIndex;

		struct rrCBufferLightInfo
		{
			int firstIndex;
		} cbuffer_light_params;
		cbuffer_light_params.firstIndex = lightSetup->directionalLightFirstIndex;

		gpu::Buffer cbuffer;
		cbuffer.initAsConstantBuffer(NULL, sizeof(rrCBufferLightInfo));
		cbuffer.upload(gfx, &cbuffer_light_params, sizeof(rrCBufferLightInfo), gpu::kTransferStream);

		// Render with the lighting0
		DrawWithPipelineAndGBuffers(
			gfx, gbuffers, m_lightingLighting0Pipeline,
			&cbuffer,
			lightSetup->lightParameterBuffer,
			[directionalLightFirstIndex, directionalLightCount](RrRenderer* renderer, gpu::GraphicsContext* gfx)
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
		DrawWithPipelineAndGBuffers(gfx, compositeInput, m_lightingLighting0Pipeline, &cbuffer, &lightParameterBuffer, [directionalLightFirstIndex, omniLightCount, cameraPass](RrRenderer* renderer, gpu::GraphicsContext* gfx)
		{
			gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_CAMERA_INFORMATION, &cameraPass->m_cbuffer);
			gfx->setVertexBuffer(0, &renderer->GetLightSphereVertexBuffer(), 0); // see RrPipelinePasses.cpp
			gfx->drawInstanced(4, omniLightCount, 0);
		});

		cbuffer.free(NULL);
	}
#endif

	// done with buffers
	if (lightSetup->lightParameterBuffer)
	{
		lightSetup->lightParameterBuffer->free(NULL);
		delete lightSetup->lightParameterBuffer;
	}

	// Pop debug
	gfx->debugGroupPop();

	// We're currently still using this same output texture so we can just return it as is
	return clearedOutputTexture;
}