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
		all_lights.resize(lights.size());

		int light_index = 0;

		// The order of the following 3 loops are important - this is how currently how they're sorted.

		setup.directionalLightFirstIndex = light_index;
		for (RrLight* light : directional_lights)
		{
			lights[light_index] = renderer::cbuffer::rrLight(light);
			all_lights[light_index] = light;
			++light_index;
		}

		setup.spotLightFirstIndex = light_index;
		for (RrLight* light : spot_lights)
		{
			lights[light_index] = renderer::cbuffer::rrLight(light);
			all_lights[light_index] = light;
			++light_index;
		}
			
		setup.omniLightFirstIndex = light_index;
		for (RrLight* light : omni_lights)
		{
			lights[light_index] = renderer::cbuffer::rrLight(light);
			all_lights[light_index] = light;
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
	RrOutputState* state,
	rrLightSetup* lightSetup)
{
	auto renderer = RrRenderer::Active; // TODO: make argument or class field

	gfx->debugGroupPush("RenderShadows");

	// Grab output size for the screen quad info
	auto& output = *state->output_info;
	rrViewport output_viewport =  output.GetOutputViewport();

	shadow_masks.resize(all_lights.size());

	// Loop through all lights and check if any need shadows
	//for (RrLight* light : all_lights)
	for (int lightIndex = 0; lightIndex < all_lights.size(); ++lightIndex)
	{
		RrLight* light = all_lights[lightIndex];

		if (!light->shadows.AreEnabled())
		{
			shadow_masks[lightIndex] = gpu::Texture();
		}
		else
		{
			Vector2i shadow_resolution = {1024, 1024};

			// Create buffer for this current light
			struct rrCBufferLightInfo
			{
				int firstIndex;
			} cbuffer_light_params;
			cbuffer_light_params.firstIndex = lightIndex;

			gpu::Buffer cbuffer;
			cbuffer.initAsConstantBuffer(NULL, sizeof(rrCBufferLightInfo));
			cbuffer.upload(gfx, &cbuffer_light_params, sizeof(rrCBufferLightInfo), gpu::kTransferStream);

			// Set up shadows for this given light in the lightSetup
			//rrDepthBufferRequest shadowDepthRequest {shadow_resolution, core::gfx::tex::kDepthFormat16, core::gfx::tex::kStencilFormatNone}; 

			// Grab another buffer for shadow masking
			gpu::Texture shadowMask;
			rrRTBufferRequest shadowMaskRequest {output_viewport.size, core::gfx::tex::kColorFormatR8};
			renderer->CreateRenderTexture( shadowMaskRequest, &shadowMask );

			// Set up RT for shadow mask
			rrRenderTarget rtShadowMask (&shadowMask);

			// Set up output
			gfx->setRenderTarget(&rtShadowMask.m_renderTarget);
			gfx->setViewport(output_viewport.corner.x, output_viewport.corner.y, output_viewport.corner.x + output_viewport.size.x, output_viewport.corner.y + output_viewport.size.y);
			gfx->setScissor(output_viewport.corner.x, output_viewport.corner.y, output_viewport.corner.x + output_viewport.size.x, output_viewport.corner.y + output_viewport.size.y);

			// Clear shadow mask
			{
				float clearColor[] = {1.0F, 1.0F, 1.0F, 1.0F};
				gfx->clearColor(clearColor);
			}

			if (light->shadows.use_contact_shadows)
			{
				if (m_shadowingContactShadowProgram == nullptr)
				{
					m_shadowingContactShadowProgram = RrShaderProgram::Load(rrShaderProgramCs{"shaders/deferred_pass/contact_shadows_c.spv"});
				}

				// Clear out usage of render target
				gfx->setRenderTarget(NULL);
				
				// Create writeable with the render target
				gpu::WriteableResource rwShadowMask;
				rwShadowMask.create(&shadowMask, 0);

				// Set up compute shader
				gfx->setComputeShader(&m_shadowingContactShadowProgram->GetShaderPipeline());
				gfx->setShaderCBuffer(gpu::kShaderStageCs, renderer::CBUFFER_PER_CAMERA_INFORMATION, &gbuffers.cameraPass->m_cbuffer);
				gfx->setShaderCBuffer(gpu::kShaderStageCs, renderer::CBUFFER_USER0, &cbuffer);
				gfx->setShaderSBuffer(gpu::kShaderStageCs, renderer::SBUFFER_USER0, lightSetup->lightParameterBuffer);
				gfx->setShaderWriteable(gpu::kShaderStageCs, 0, &rwShadowMask);
				gfx->setShaderTexture(gpu::kShaderStageCs, 1, gbuffers.combined_depth);
				// Render the contact shadows
				gfx->dispatch(output_viewport.size.x, output_viewport.size.y, 1);
				// Unbind the UAV
				gfx->setShaderWriteable(gpu::kShaderStageCs, 0, NULL);

				// Done with the writeable
				rwShadowMask.destroy();
			}

			cbuffer.free(NULL);

			// Save the shadow mask now
			shadow_masks[lightIndex] = shadowMask;
		}
	}

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

		gpu::Texture shadowMask = shadow_masks[directionalLightFirstIndex];

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
			[directionalLightFirstIndex, directionalLightCount, shadowMask](RrRenderer* renderer, gpu::GraphicsContext* gfx)
		{
			if (shadowMask.valid())
			{
				gfx->setShaderTextureAuto(gpu::kShaderStagePs, 6, (gpu::Texture*)&shadowMask);
			}
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