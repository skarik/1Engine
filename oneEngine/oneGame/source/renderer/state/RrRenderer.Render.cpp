#include "RrRenderer.h"
#include "renderer/state/Settings.h"
//#include "core/settings/CGameSettings.h"
#include "core/math/Math.h"
#include "core-ext/profiler/CTimeProfiler.h"
#include "core/system/Screen.h"
#include "core/debug/console.h"
#include "core-ext/threads/Jobs.h"
#include "core-ext/resources/ResourceManager.h"

#include "renderer/exceptions/exceptions.h"

#include "renderer/camera/RrCamera.h"
#include "renderer/light/RrLight.h"
#include "renderer/texture/RrRenderTexture.h"

#include "renderer/object/RrRenderObject.h"

#include "gpuw/Pipeline.h"
#include "gpuw/ShaderPipeline.h"
#include "gpuw/OutputSurface.h"
#include "renderer/material/RrPass.h"
#include "renderer/material/RrShaderProgram.h"
#include "renderer/state/pipeline/RrPipeline.h"
#include "renderer/windowing/RrWindow.h"

#include <algorithm>

#include "renderer/state/RaiiHelpers.h"

using namespace renderer;

#pragma warning( disable : 4102 ) // Suppress warnings about unreferenced labels. Used for organizational purposes here.

//===============================================================================================//
// RrWorld : Frame Update
//===============================================================================================//

void RrWorld::FrameUpdate ( void )
{
	switch (state.work_step)
	{
	case rrWorldState::kWorkStepCompactObjects:
		CompactObjectListing();
		break;
	case rrWorldState::kWorkStepCompactLogics:
		CompactLogicListing();
		break;
	case rrWorldState::kWorkStepSortObjects:
		SortObjectListing();
		break;
	case rrWorldState::kWorkStepSortLogics:
		SortLogicListing();
		break;
	}
	state.work_step = (rrWorldState::WorkStep)(state.work_step % rrWorldState::kWorkStep_MAX);
}

void RrWorld::CompactObjectListing ( void )
{
	// Cut off the items at the end of the listing.
	for (size_t i = objects.size() - 1; i >= 0; --i)
	{
		if (objects[i] != nullptr)
		{
			objects.resize(i + 1);
			break;
		}
	}
}

void RrWorld::SortObjectListing ( void )
{
	std::sort(objects.begin(), objects.end(), [](RrRenderObject* a, RrRenderObject* b)
	{
		// Force nullptr to the end of the list. Treat nullptr as a high value.
		if (a == nullptr && b != nullptr) return false;
		if (a != nullptr && b == nullptr) return true;

		// Otherwise, sort by memory address.
		return a < b;

		// TODO: We can sort by type if each RrRenderableObject has a virtual & static typeid.
		// To ensure that this is set up properly, we can also have a virtual IsInstantiatable() call.
	});

	// Update the item IDs
	for (int objectIndex = 0; objectIndex < objects.size(); ++objectIndex)
	{
		RrRenderObject* object = objects[objectIndex];
		if (object != nullptr)
		{
			rrId id;
			id.world_index = world_index;
			id.object_index = objectIndex;
			object->Access_id_From_RrWorld().Set(id);
		}
	}
}

//===============================================================================================//
// RrRenderer: Rendering
//===============================================================================================//

void RrRenderer::StepPreRender ( rrRenderFrameState* frameState )
{
	// Wait for the PostStep to finish:
	core::jobs::System::Current::WaitForJobs( core::jobs::kJobTypeRenderStep );

	// Add the queued up objects to the world now that all jobs are done.
	AddQueuedToWorld();
	// Update the worlds
	for ( size_t worldIndex = 0; worldIndex < worlds.size(); ++worldIndex )
	{
		worlds[worldIndex]->FrameUpdate();
	}

	// Update the camera system for the next frame
	auto l_currentCamera = RrCamera::GetFirstCamera();
	while (l_currentCamera != RrCamera::GetLastCamera())
	{
		l_currentCamera->LateUpdate();
		l_currentCamera = l_currentCamera->GetNextCamera();
	}

	// Update the per-frame constant buffer data:
	{
		renderer::cbuffer::rrPerFrame frameInfo = {};
		// Set up time constants:
		frameInfo.time		= Vector4f(Time::currentTime / 2.0F, Time::currentTime, Time::currentTime * 2.0F, Time::currentTime * 3.0F);
		frameInfo.sinTime	= Vector4f(sinf(Time::currentTime / 8.0F), sinf(Time::currentTime / 4.0F), sinf(Time::currentTime / 2.0F), sinf(Time::currentTime));
		frameInfo.cosTime	= Vector4f(cosf(Time::currentTime / 8.0F), cosf(Time::currentTime / 4.0F), cosf(Time::currentTime / 2.0F), cosf(Time::currentTime));
		// Set up simple distance fog constants:
		frameInfo.fogColor	= Vector4f(Vector3f(renderer::Settings.fogColor.raw), 1.0F);
		Color t_atmoColor = Color::Lerp(renderer::Settings.fogColor, renderer::Settings.ambientColor, 0.5F) * Color(0.9F, 0.9F, 1.1F); // Hack for AFTER
		frameInfo.atmoColor = Vector4f(Vector3f(t_atmoColor.raw), 1.0F);
		frameInfo.fogEnd = renderer::Settings.fogEnd;
		frameInfo.fogScale = renderer::Settings.fogScale; // These fog values likely won't see actual use in more modern rendering pipelines.
		// Frame index
		frameInfo.frameIndex = frame_index;
		// Push to GPU:
		frameState->cbuffer_perFrame.initAsConstantBuffer(NULL, sizeof(renderer::cbuffer::rrPerFrame));
		frameState->cbuffer_perFrame.upload(NULL, &frameInfo, sizeof(renderer::cbuffer::rrPerFrame), gpu::kTransferWriteDiscardPrevious);
	}

	// Update the per-pass constant buffer data:
	/*for (size_t outputIndex = 0; outputIndex < render_outputs.size(); ++outputIndex )
	{
		RrOutputState* state = render_outputs[outputIndex].state;
		if (state != nullptr)
		{
			for (int iLayer = renderer::kRenderLayer_BEGIN; iLayer < renderer::kRenderLayer_MAX; ++iLayer)
			{
				renderer::cbuffer::rrPerPassLightingInfo passInfo = {}; // Unused. May want to remove later...

				state->internal_cbuffers_passes[state->internal_chain_index * renderer::kRenderLayer_MAX + iLayer]
					.upload(NULL, &passInfo, sizeof(renderer::cbuffer::rrPerPassLightingInfo), gpu::kTransferWriteDiscardPrevious);
			}
		}
	}*/

	// Begin the logic jobs
	for ( size_t worldIndex = 0; worldIndex < worlds.size(); ++worldIndex )
	{
		for ( size_t logicIndex = 0; logicIndex < worlds[worldIndex]->logics.size(); ++logicIndex )
		{
			RrLogicObject* logic = worlds[worldIndex]->logics[logicIndex];
			if ( logic != nullptr && logic->GetActive() )
			{
				core::jobs::System::Current::AddJobRequest( core::jobs::kJobTypeRenderStep, &(RrLogicObject::PreStep), logic );
			}
		}
	}
	// Perform the synchronous logic jobs
	for ( size_t worldIndex = 0; worldIndex < worlds.size(); ++worldIndex )
	{
		for ( size_t logicIndex = 0; logicIndex < worlds[worldIndex]->logics.size(); ++logicIndex )
		{
			RrLogicObject* logic = worlds[worldIndex]->logics[logicIndex];
			if ( logic != nullptr && logic->GetActive() )
			{
				logic->PreStepSynchronus();
			}
		}
	}

	// Update the streamed loading system
	auto resourceManager = core::ArResourceManager::Active();
	{	// Update only the renderer-specific systems
		resourceManager->UpdateManual(core::kResourceTypeRrTexture);
	}
	
	// Update stale buffers in the pool
	UpdateResourcePools();

	// Call begin render
	TimeProfiler.BeginTimeProfile( "rs_begin_render" );
	for ( size_t worldIndex = 0; worldIndex < worlds.size(); ++worldIndex )
	{
		for ( size_t objectIndex = 0; objectIndex < worlds[worldIndex]->objects.size(); ++objectIndex )
		{
			auto object = worlds[worldIndex]->objects[objectIndex];
			if ( object != nullptr )
			{
				object->BeginRender(); //massive slowdown?
			}
		}
	}
	TimeProfiler.EndTimeProfile( "rs_begin_render" );

	// Wait for all the PreStep to finish:
	core::jobs::System::Current::WaitForJobs( core::jobs::kJobTypeRenderStep );
}

void RrRenderer::StepPostRender ( void )
{
	// Call end render
	TimeProfiler.BeginTimeProfile( "rs_end_render" );
	for ( size_t worldIndex = 0; worldIndex < worlds.size(); ++worldIndex )
	{
		for ( size_t objectIndex = 0; objectIndex < worlds[worldIndex]->objects.size(); ++objectIndex )
		{
			auto object = worlds[worldIndex]->objects[objectIndex];
			if ( object != nullptr )
			{
				object->EndRender();
			}
		}
	}
	TimeProfiler.EndTimeProfile( "rs_end_render" );

	// Begin the post-step render jobs
	for ( size_t worldIndex = 0; worldIndex < worlds.size(); ++worldIndex )
	{
		for ( size_t logicIndex = 0; logicIndex < worlds[worldIndex]->logics.size(); ++logicIndex )
		{
			RrLogicObject* logic = worlds[worldIndex]->logics[logicIndex];
			if ( logic != nullptr && logic->GetActive() )
			{
				core::jobs::System::Current::AddJobRequest( core::jobs::kJobTypeRenderStep, &(RrLogicObject::PostStep), logic );
			}
		}
	}
	// Perform the synchronous logic jobs
	for ( size_t worldIndex = 0; worldIndex < worlds.size(); ++worldIndex )
	{
		for ( size_t logicIndex = 0; logicIndex < worlds[worldIndex]->logics.size(); ++logicIndex )
		{
			RrLogicObject* logic = worlds[worldIndex]->logics[logicIndex];
			if ( logic != nullptr && logic->GetActive() )
			{
				logic->PostStepSynchronus();
			}
		}
	}
}

void RrRenderer::StepBufferPush ( gpu::GraphicsContext* gfx, const RrOutputInfo& output, RrOutputState* state, gpu::Texture texture )
{
	// Are we in buffer mode? (This should always be true)
	gfx->debugGroupPush("Buffer Push");
	TimeProfiler.BeginTimeProfile( "rs_buffer_push" );
	{
		rrViewport output_viewport = output.GetOutputViewport();

		// Render the current result to the screen
		gfx->setRenderTarget(output.GetRenderTarget());
		gfx->setViewport(output_viewport.corner.x, output_viewport.corner.y, output_viewport.corner.x + output_viewport.size.x, output_viewport.corner.y + output_viewport.size.y);
		gfx->setScissor(output_viewport.corner.x, output_viewport.corner.y, output_viewport.corner.x + output_viewport.size.x, output_viewport.corner.y + output_viewport.size.y);
		{
			float clearColor[] = {1, 1, 0, 0};
			gfx->clearColor(clearColor);

			gpu::RasterizerState rs;
			rs.cullmode = gpu::kCullModeNone;
			gfx->setRasterizerState(rs);

			gpu::DepthStencilState ds;
			ds.depthTestEnabled   = false;
			ds.depthWriteEnabled  = false;
			ds.stencilTestEnabled = false;
			ds.stencilWriteMask   = 0x00;
			gfx->setDepthStencilState(ds);

			gpu::BlendState bs;
			bs.enable = false;
			bs.src = gpu::kBlendModeOne;
			bs.dst = gpu::kBlendModeZero;
			bs.srcAlpha = gpu::kBlendModeOne;
			bs.dstAlpha = gpu::kBlendModeZero;
			gfx->setBlendState(bs);

			gfx->setPipeline(&GetScreenQuadCopyPipeline());
			gfx->setVertexBuffer(0, &GetScreenQuadOutputVertexBuffer(), 0); // see RrPipelinePasses.cpp
			gfx->setVertexBuffer(1, &GetScreenQuadOutputVertexBuffer(), 0); // there are two binding slots defined with different stride
			gfx->setShaderTextureAuto(gpu::kShaderStagePs, 0,
										&texture);
			gfx->draw(4, 0);
		}
		//gfx->clearPipelineAndWait(); // Wait until we're done using the buffer...
	}
	TimeProfiler.EndTimeProfile( "rs_buffer_push" );
	gfx->debugGroupPop();
}

// Called during the window's rendering routine.
void RrRenderer::Render ( void )
{
#define _ENGINE_LIMIT_FRAMES // TODO: Move this block somewhere else.
#ifdef _ENGINE_LIMIT_FRAMES
	// Slow down the framerate if it's too fast.
	// Incredibly high framerates can cause massive issues with various window managers (DWM and and Xlib both have frozen).
	// To alleviate this, we sleep for 0.1 ms if we ever get too fast, so we top off around 500 to 1000 FPS.
	int fps = int(1.0F / Time::smoothDeltaTime);
	if (fps > 600)
	{
		std::this_thread::sleep_for(std::chrono::microseconds(100));
	}
#endif

	{
		// Update all aggregate counters
		TimeProfiler.ZeroTimeProfile( "rs_render_makelist" );
		TimeProfiler.ZeroTimeProfile( "rs_render_pre" );
		TimeProfiler.ZeroTimeProfile( "rs_render_lightpush" );
		TimeProfiler.ZeroTimeProfile( "rs_render_postpush" );

		TimeProfiler.ZeroTimeProfile( "rs_mat_bindpass" );
		TimeProfiler.ZeroTimeProfile( "rs_mat_bindshader" );
		TimeProfiler.ZeroTimeProfile( "rs_mat_binduniforms" );
		TimeProfiler.ZeroTimeProfile( "rs_mat_uni_sampler" );
		TimeProfiler.ZeroTimeProfile( "rs_mat_uni_consts" );
		TimeProfiler.ZeroTimeProfile( "rs_mat_uni_lights" );
		TimeProfiler.ZeroTimeProfile( "rs_mat_uni_obj" );

		TimeProfiler.ZeroTimeProfile( "rs_skinned_model_begin" );
		TimeProfiler.ZeroTimeProfile( "rs_skinned_model_end" );
		//TimeProfiler.ZeroTimeProfile( "rs_skinned_model_begin_s" );
		//TimeProfiler.ZeroTimeProfile( "rs_skinned_model_begin_w" );
		//TimeProfiler.ZeroTimeProfile( "rs_skinned_model_begin_x" );

		TimeProfiler.ZeroTimeProfile( "rs_particle_renderer_begin" );
		TimeProfiler.ZeroTimeProfile( "rs_particle_renderer_push" );
	}

	rrRenderFrameState frameState;

	// Update pre-render steps:
	//	- RrLogicObject::PostStep finish
	//	- RrCamera::LateUpdate
	//	- RrLogicObject::PreStep, RrLogicObject::PreStepSynchronous
	//	- ResourceManager::update
	//	- BeginRender
	StepPreRender(&frameState); 

	// Check for a main camera to work with
	if ( RrCamera::activeCamera == NULL )
	{
		return; // Don't render if there's no camera to render with...
	}

	RrCamera* prevActiveCam = RrCamera::activeCamera;

	std::vector<gpu::GraphicsContext*> collected_contexts;
	std::vector<gpu::OutputSurface*> collected_outputs;

	// Prepare frame
	
	// Loop through all cameras (todo: sort render order for each camera)
	TimeProfiler.BeginTimeProfile( "rs_camera_matrix" );
	for ( size_t outputIndex = 0; outputIndex < render_outputs.size(); ++outputIndex )
	{
		auto& render_output = render_outputs[outputIndex];
		if (render_output.info.enabled)
		{
			ARCORE_ASSERT(render_output.info.camera != nullptr);
			render_output.info.camera->UpdateMatrix(render_output.info);
		}
	}
	TimeProfiler.EndTimeProfile( "rs_camera_matrix" );
	
	TimeProfiler.BeginTimeProfile( "rs_render" );
	// TODO: Loop through all Outputs. Find the ones that are active.
	static RrWindow* gLastDrawnWindow = nullptr;
	for ( size_t outputIndex = 0; outputIndex < render_outputs.size(); ++outputIndex )
	{
		auto& render_output = render_outputs[outputIndex];
		if (render_output.info.enabled)
		{
			// Update the state
			if (render_output.state == nullptr)
			{
				render_output.state = new RrOutputState();
			}
			render_output.state->Update(&render_output.info, &frameState);

			// Grab the graphics context to render with
			gpu::GraphicsContext* gfx = render_output.state->graphics_context;

			// Collect this current context
			if (std::find(collected_contexts.begin(), collected_contexts.end(), gfx) == collected_contexts.end())
			{
				collected_contexts.push_back(gfx);
			}

			// Reset the context
			gfx->reset();

			// Ensure the outputs context is correct
			if (render_output.info.type == RrOutputInfo::Type::kWindow)
			{
				if (gLastDrawnWindow != render_output.info.output_window)
				{
					gLastDrawnWindow = render_output.info.output_window;
					gLastDrawnWindow->GpuSurface()->activate();
				}

				// Collect the output so they can be finalized
				collected_outputs.push_back(render_output.info.output_window->GpuSurface());
			}

			//
			gpu::Texture output_texture;

			// Render with the given camera.
			ARCORE_ASSERT(render_output.info.camera != nullptr);
			if (render_output.info.camera->GetRender())
			{
				// Render from camera
				RrCamera::activeCamera = render_output.info.camera;
				gfx->debugGroupPush(render_output.info.name.c_str());
				// We drop the const on the world because rendering objects changes their state
				output_texture = RenderOutput(gfx, render_output.info, render_output.state, render_output.info.world);
				gfx->debugGroupPop();
			}

			if (output_texture.valid())
			{
				// Push buffer to screen
				StepBufferPush(gfx, render_output.info, render_output.state, output_texture);
			}
			else
			{
				ARCORE_ERROR("Invalid texture attempted to be pushed to screen");
			}
		}
	}
	TimeProfiler.EndTimeProfile( "rs_render" );

	// Set active camera back to default
	RrCamera::activeCamera = prevActiveCam;

	// Signal frame ended
	for (gpu::GraphicsContext* gfx : collected_contexts)
	{
		// Submit the commands now
		gfx->submit();
		// Validate the GPU state isn't completely broken
		gfx->validate();
	}

	// Query up all the outputs we were working on
	// Present the output buffer
	for (gpu::OutputSurface* surface : collected_outputs)
	{
		// TODO: On Vulkan and other platforms, this will likely need a fence.
		surface->present();
	}

	for ( size_t outputIndex = 0; outputIndex < render_outputs.size(); ++outputIndex )
	{
		RrOutputState* state = render_outputs[outputIndex].state;
		if (state != nullptr)
		{
			// TODO: This originally counted frames here.
		}
	}
	// Count the global frame rendered
	frame_index++;
	m_constantBufferPool.m_currentFrame = frame_index;

	// Call post-render and start up new jobs
	StepPostRender();

	// Free frame resources
	frameState.cbuffer_perFrame.free(NULL);
}

// Some debug compiler flags
//#define SKIP_NON_WORLD_STUFF
#define FORCE_BUFFER_CLEAR
//#define ENABLE_RUNTIME_BLIT_TEST

gpu::Texture RrRenderer::RenderOutput ( gpu::GraphicsContext* gfx, const RrOutputInfo& output, RrOutputState* state, RrWorld* world )
{
Create_Pipeline:
	if (state->pipeline_renderer == nullptr || !state->pipeline_renderer->IsCompatible(world->pipeline_mode))
	{
		// Free the old renderer
		delete_safe(state->pipeline_renderer);

		// Create new renderer & save the current mode
		state->pipeline_renderer = renderer::CreatePipelineRenderer(world->pipeline_mode);
		state->pipeline_mode = world->pipeline_mode;
	}

Prepare_Pipeline:
	if (state->pipeline_renderer != nullptr)
	{
		const auto pipelineOptions = state->output_info->world->pipeline_options;
		const bool bPipelineOptionsCompatible = (pipelineOptions == nullptr) ? false : pipelineOptions->IsCompatible(state->pipeline_mode);
		state->pipeline_renderer->PrepareOptions(bPipelineOptionsCompatible ? pipelineOptions : nullptr);
	}

Update_Culling:
	if (state->pipeline_renderer != nullptr)
	{
		// Create common context for all renders
		rrRenderContext render_context;
		render_context.context_graphics = gfx;
		render_context.constantBuffer_pool = &m_constantBufferPool;

		state->pipeline_renderer->CullObjects(&render_context, output, state, world);
	}

Render_Camera_Passes:
	// Get a pass list from the camera
	const int kMaxCameraPasses = 8;
	rrCameraPass cameraPasses [kMaxCameraPasses];
	rrCameraPassInput cameraPassInput;
	cameraPassInput.m_maxPasses = kMaxCameraPasses;
	//cameraPassInput.m_bufferingCount = (uint16_t)std::min<size_t>(state->internal_chain_list.size(), 0xFFFF);
	//cameraPassInput.m_bufferingIndex = state->internal_chain_index;
	cameraPassInput.m_outputInfo = &output;
	cameraPassInput.m_graphicsContext = gfx;

	int passCount = output.camera->PassCount();
	output.camera->PassRetrieve(&cameraPassInput, cameraPasses);

	// Begin rendering now
	//output.camera->RenderBegin();

	// Loop through each pass and render with them:
	gpu::Texture output_color;
	for (int iCameraPass = 0; iCameraPass < passCount; ++iCameraPass)
	{
		if (cameraPasses[iCameraPass].m_passType == kCameraRenderWorld)
		{
			output_color = RenderObjectListWorld(gfx, &cameraPasses[iCameraPass], world->objects.data(), (uint32_t)world->objects.size(), state);
		}
		else
		{
			ARCORE_ERROR("Only single pass supported");
		}

		// Free the camera pass now that we're done with it.
		cameraPasses[iCameraPass].free();
	}

	// Finish up the rendering
	//output.camera->RenderEnd();

	// Return result output
	return output_color;
}

gpu::Texture RrRenderer::RenderObjectListWorld ( gpu::GraphicsContext* gfx, rrCameraPass* cameraPass, RrRenderObject** objectsToRender, const uint32_t objectCount, RrOutputState* state )
{
	struct rrRenderRequestGroup
	{
		bool m_enabled;
		// Depth-prepass is used for all opaques, deferred and forward. This is what is used for the base of occlusion queries.
		std::vector<rrRenderRequest> m_4rDepthPrepass;
		// Deferred objects render with a separate pass.
		std::vector<rrRenderRequest> m_4rDeferred;
		// Forward rendered objects render on top of the deferred state.
		std::vector<rrRenderRequest> m_4rForward;
		// Forward translucent objects render on top of everything else.
		std::vector<rrRenderRequest> m_4rForwardTranslucent;

		// Jobs are separate from the output. Run before the previous steps
		std::vector<rrRenderRequest> m_4rJob;

		// effects, possibly unused?
		std::vector<rrRenderRequest> m_4rFog;
		std::vector<rrRenderRequest> m_4rWarp;
	};
	rrRenderRequestGroup	l_4rGroup [renderer::kRenderLayer_MAX];
	core::jobs::JobId		l_4rJobs [renderer::kRenderLayer_MAX];
	core::jobs::JobId		l_4rSortJobs [renderer::kRenderLayer_MAX];

Prerender_Pass:

	// Now we loop through all the objects, check if they have an enabled pass, and call Pre-Render on them.
	for (uint8_t iLayer = renderer::kRenderLayer_BEGIN; iLayer < renderer::kRenderLayer_MAX; ++iLayer)
	{
		for (uint32_t iObject = 0; iObject < objectCount; ++iObject)
		{
			RrRenderObject* renderable = objectsToRender[iObject];
			if (renderable != NULL && renderable->GetVisible())
			{
				bool l_hasPass = false;

				// Loop through each pass to place them in the render lists.
				for (uint8_t iPass = 0; iPass < kPass_MaxPassCount; ++iPass)
				{
					if (renderable->PassEnabled(iPass) && renderable->PassLayer(iPass) == iLayer)
					{
						l_hasPass = true;
						break;
					}
				}

				// If there is an enabled pass, we want to call PreRender on the object & enable the layer for rendering.
				if (l_hasPass)
				{
					//renderable->PreRender(cameraPass);
					renderable->CreateConstants(cameraPass);
				}
			}
		}
	}

Pass_Groups:

	for (uint8_t iLayer = renderer::kRenderLayer_BEGIN; iLayer < renderer::kRenderLayer_MAX; ++iLayer)
	{
#	ifdef SKIP_NON_WORLD_STUFF
		// Only add 2D or world objects for now
		if (iLayer != renderer::kRLWorld
			&& iLayer != renderer::kRLV2D)
			continue;
#	endif
		l_4rGroup[iLayer].m_enabled = false;

		// Create the thread that puts objects into the layer.
		l_4rJobs[iLayer] = core::jobs::System::Current::AddJobRequest(core::jobs::kJobTypeRendererSorting, [=, &l_4rGroup, &l_4rSortJobs]()
		{

		Pass_Collection:

			for (uint32_t iObject = 0; iObject < objectCount; ++iObject)
			{
				RrRenderObject* renderable = objectsToRender[iObject];
				if (renderable != NULL && renderable->GetVisible())
				{
					bool l_hasPass = false;

					// Loop through each pass to place them in the render lists.
					for (uint8_t iPass = 0; iPass < kPass_MaxPassCount; ++iPass)
					{
						if (renderable->PassEnabled(iPass) && renderable->PassLayer(iPass) == iLayer)
						{
							const rrPassType passType = renderable->PassType(iPass);
							const bool passDepthWrite = renderable->PassDepthWrite(iPass);
							const bool passTranslucent = renderable->PassIsTranslucent(iPass);

							// If part of the world...
							if (passType == kPassTypeForward || passType == kPassTypeDeferred)
							{
								if (passDepthWrite)
								{	// Add opaque objects to the prepass list:
									l_4rGroup[iLayer].m_4rDepthPrepass.push_back(rrRenderRequest{renderable, iPass});
								}
								// Add the other objects to the deferred or forward pass
								if (passType == kPassTypeDeferred)
								{
									ARCORE_ASSERT(passTranslucent == false);
									l_4rGroup[iLayer].m_4rDeferred.push_back(rrRenderRequest{renderable, iPass});
								}
								else
								{
									if (!passTranslucent)
										l_4rGroup[iLayer].m_4rForward.push_back(rrRenderRequest{renderable, iPass});
									else
										l_4rGroup[iLayer].m_4rForwardTranslucent.push_back(rrRenderRequest{renderable, iPass});
								}
							}
							// If part of fog effects...
							else if (passType == kPassTypeVolumeFog)
							{
								l_4rGroup[iLayer].m_4rFog.push_back(rrRenderRequest{renderable, iPass});
							}
							// If part of warp effects...
							else if (passType == kPassTypeWarp)
							{
								l_4rGroup[iLayer].m_4rWarp.push_back(rrRenderRequest{renderable, iPass});
							}
							// If part of a job...
							else if (passType == kPassTypeJob)
							{
								l_4rGroup[iLayer].m_4rJob.push_back(rrRenderRequest{renderable, iPass});
							}
							else
							{
								ARCORE_ERROR("Invalid pass type provided in the job.");
							}

							l_hasPass = true;
						}
					}

					// If there is an enabled pass, we want to call PreRender on the object & enable the layer for rendering.
					if (l_hasPass)
					{
						l_4rGroup[iLayer].m_enabled = true;
					}
				}
			}

		Object_Sorting:

			// Add a job for the sorting.
			l_4rSortJobs[iLayer] = core::jobs::System::Current::AddJobRequest(core::jobs::kJobTypeRendererSorting, [=, &l_4rGroup]()
			{
				// immidiately sort the depth pre-pass:
				std::sort(l_4rGroup[iLayer].m_4rDepthPrepass.begin(), l_4rGroup[iLayer].m_4rDepthPrepass.end(), RenderRequestSorter); 

				// sort the rest:
				std::sort(l_4rGroup[iLayer].m_4rDeferred.begin(), l_4rGroup[iLayer].m_4rDeferred.end(), RenderRequestSorter); 
				std::sort(l_4rGroup[iLayer].m_4rForward.begin(), l_4rGroup[iLayer].m_4rForward.end(), RenderRequestSorter); 
				std::sort(l_4rGroup[iLayer].m_4rForwardTranslucent.begin(), l_4rGroup[iLayer].m_4rForwardTranslucent.end(), RenderRequestSorter); 
				std::sort(l_4rGroup[iLayer].m_4rFog.begin(), l_4rGroup[iLayer].m_4rFog.end(), RenderRequestSorter); 
				std::sort(l_4rGroup[iLayer].m_4rWarp.begin(), l_4rGroup[iLayer].m_4rWarp.end(), RenderRequestSorter); 
			});

		});
	}

Setup_Pipeline:
	// Create common context for all renders
	rrRenderContext render_context;
	render_context.context_graphics = gfx;
	render_context.constantBuffer_pool = &m_constantBufferPool;

	// Lighting needs to be handled by the pipeline info
	if (state->pipeline_renderer)
	{
		state->pipeline_renderer->PreparePass(&render_context);
	}

Render_Groups:
	// Run the jobs first
	for (uint8_t iLayer = renderer::kRenderLayer_BEGIN; iLayer < renderer::kRenderLayer_MAX; ++iLayer)
	{
		// wait for the sorting of this layer to finish:
		core::jobs::System::Current::WaitForJob(l_4rJobs[iLayer]);

		// Skip if there's nothing
		if (l_4rGroup[iLayer].m_4rJob.empty())
		{
			continue;
		}
	RenderJobs:

		// Do the forward pass:
		for (size_t iObject = 0; iObject < l_4rGroup[iLayer].m_4rJob.size(); ++iObject)
		{
			const rrRenderRequest&  l_4r = l_4rGroup[iLayer].m_4rJob[iObject];
			RrRenderObject* renderable = l_4r.obj;

			RrRenderObject::rrRenderParams params;
			params.pass = l_4r.pass;
			params.cbuf_perCamera = &cameraPass->m_cbuffer;
			params.cbuf_perFrame = &state->frame_state->cbuffer_perFrame;
			params.cbuf_perPass = nullptr;
			params.context = &render_context;

			ARCORE_ASSERT(params.context != nullptr);
			renderable->Render(&params);
		}
	}

	// Set up output buffers
	gpu::Texture outputDepth;
	gpu::WOFrameAttachment outputStencil;
	gpu::Texture outputColor;
	{
		rrDepthBufferRequest depthRequest = m_currentDepthBufferRequest;
		depthRequest.size = cameraPass->m_viewport.size;

		rrRTBufferRequest colorRequest {cameraPass->m_viewport.size, core::gfx::tex::kColorFormatRGBA16F}; 

		CreateRenderTexture( &depthRequest, &outputDepth, &outputStencil );
		CreateRenderTexture( colorRequest, &outputColor );

		m_currentDepthBufferRequest = depthRequest; // Save the depth request settings
	}

	// Clear color on output
	{
		rrRenderTarget forwardTarget ( &outputColor );
		gfx->setRenderTarget(&forwardTarget.m_renderTarget);

		float clearColor[] = {0, 0, 0, 0};
		gfx->clearColor(clearColor);
	}

	// Run the full rendering routine
	for (uint8_t iLayer = renderer::kRenderLayer_BEGIN; iLayer < renderer::kRenderLayer_MAX; ++iLayer)
	{
		// wait for the sorting of this layer to finish:
		core::jobs::System::Current::WaitForJob(l_4rJobs[iLayer]);
		core::jobs::System::Current::WaitForJob(l_4rSortJobs[iLayer]);

		// Skip if there's nothing
		if (l_4rGroup[iLayer].m_4rDepthPrepass.empty()
			&& l_4rGroup[iLayer].m_4rDeferred.empty() 
			&& l_4rGroup[iLayer].m_4rForward.empty() 
			&& l_4rGroup[iLayer].m_4rForwardTranslucent.empty() 
			&& l_4rGroup[iLayer].m_4rFog.empty() 
			&& l_4rGroup[iLayer].m_4rWarp.empty())
		{
			continue;
		}

	Rendering:

		bool dirty_deferred = false;
		bool dirty_forward = false;

		gfx->debugGroupPush((std::string("Rendering Layer ") + std::to_string((int)iLayer)).c_str());

		gfx->debugGroupPush("Depth Pre-pass");
		{
			rrRenderTarget depthTarget ( &outputDepth, &outputStencil );

			// Set up output
			gfx->setRenderTarget(&depthTarget.m_renderTarget); 

			// Set up no-color draw for pre-pass mode.
			{
				gpu::BlendState bs;
				bs.channelMask = 0x00; // Disable color entirely.
				bs.enable = false;
				gfx->setBlendState(bs);
			}
		
			// Set up viewport
			{
				gfx->setViewport(
					cameraPass->m_viewport.corner.x,
					cameraPass->m_viewport.corner.y,
					cameraPass->m_viewport.corner.x + cameraPass->m_viewport.size.x,
					cameraPass->m_viewport.corner.y + cameraPass->m_viewport.size.y );
				gfx->setScissor(
					cameraPass->m_viewport.corner.x,
					cameraPass->m_viewport.corner.y,
					cameraPass->m_viewport.corner.x + cameraPass->m_viewport.size.x,
					cameraPass->m_viewport.corner.y + cameraPass->m_viewport.size.y );
			}

			// Clear depth:
			{
				gpu::DepthStencilState ds;
				ds.depthTestEnabled   = true;
				ds.depthWriteEnabled  = true;
				ds.depthFunc = gpu::kCompareOpLessEqual;
				ds.stencilTestEnabled = false;
				gfx->setDepthStencilState(ds);

				gfx->clearDepthStencil(true, 1.0F, false, 0x00);
			}
		
			// Do depth pre-pass:
			for (size_t iObject = 0; iObject < l_4rGroup[iLayer].m_4rDepthPrepass.size(); ++iObject)
			{
				const rrRenderRequest&  l_4r = l_4rGroup[iLayer].m_4rDepthPrepass[iObject];
				RrRenderObject* renderable = l_4r.obj;

				RrRenderObject::rrRenderParams params;
				params.pass = l_4r.pass;
				params.pass_type = kPassTypeSystemDepth;
				params.cbuf_perCamera = &cameraPass->m_cbuffer;
				params.cbuf_perFrame = &state->frame_state->cbuffer_perFrame;
				params.cbuf_perPass = nullptr;
				params.context = &render_context;

				ARCORE_ASSERT(params.context != nullptr);
				renderable->PrepRender(cameraPass);
				renderable->Render(&params);
			}
		}
		gfx->debugGroupPop();

		// Do post-depth work now
		if (state->pipeline_renderer != nullptr)
		{
			rrPipelinePostDepthInput input;

			input.layer = (renderer::rrRenderLayer)iLayer;
			input.combined_depth = &outputDepth;
			input.cameraPass = cameraPass;

			state->pipeline_renderer->PostDepth(&render_context, input, state);
		}

		// Request a normals buffer, since both deferreds & forwards need to render their normals out.
		gpu::Texture outputNormals;
		{
			rrRTBufferRequest normalsRequest {cameraPass->m_viewport.size, core::gfx::tex::kColorFormatRG32UI}; // Normals + packed material info
			CreateRenderTexture( normalsRequest, &outputNormals );
		}

		if (!l_4rGroup[iLayer].m_4rDeferred.empty())
		{
			// Allocated needed texture buffers for rendering
			static constexpr int kColorGBufferCount = 3;
			static const core::gfx::tex::arColorFormat kColorAttachments[kColorGBufferCount] = {
				core::gfx::tex::kColorFormatRGBA8,
				core::gfx::tex::kColorFormatRGBA8,
				core::gfx::tex::kColorFormatRGBA8};
			gpu::Texture deferredGbuffers [kColorGBufferCount];
			{
				rrMRTBufferRequest colorsRequest {cameraPass->m_viewport.size, kColorGBufferCount, kColorAttachments};
				CreateRenderTextures( colorsRequest, deferredGbuffers );
			}
			
			// Set up the layout for the GBuffers
			static constexpr int kColorOutputCount = 4;
			gpu::Texture outputGbuffers [kColorOutputCount] = {
				deferredGbuffers[0],
				outputNormals,
				deferredGbuffers[1],
				deferredGbuffers[2]
			};

			gfx->debugGroupPush("Deferred Opaques");
			{
				// Set up output
				rrRenderTarget deferredTarget ( &outputDepth, &outputStencil, outputGbuffers, kColorOutputCount );
				gfx->setRenderTarget(&deferredTarget.m_renderTarget);
			
				// Set color draw for deferred mode.
				{
					gpu::BlendState bs;
					bs.channelMask = 0xFF; // Enable color masking.
					bs.enable = false;
					gfx->setBlendState(bs);
				}

				// Set up viewport
				{
					gfx->setViewport(
						cameraPass->m_viewport.corner.x,
						cameraPass->m_viewport.corner.y,
						cameraPass->m_viewport.corner.x + cameraPass->m_viewport.size.x,
						cameraPass->m_viewport.corner.y + cameraPass->m_viewport.size.y );
					gfx->setScissor(
						cameraPass->m_viewport.corner.x,
						cameraPass->m_viewport.corner.y,
						cameraPass->m_viewport.corner.x + cameraPass->m_viewport.size.x,
						cameraPass->m_viewport.corner.y + cameraPass->m_viewport.size.y );
				}

				// Clear color on MRT now
				float clearColor[] = {0, 0, 0, 0};
				gfx->clearColorAll(clearColor);

				// Set up default depth-equal for lazy objects
				{
					gpu::DepthStencilState ds;
					ds.depthTestEnabled   = true;
					ds.depthWriteEnabled  = false;
					ds.depthFunc = gpu::kCompareOpEqual;
					ds.stencilTestEnabled = false;
					gfx->setDepthStencilState(ds);
				}

				// Do the deferred pass:
				for (size_t iObject = 0; iObject < l_4rGroup[iLayer].m_4rDeferred.size(); ++iObject)
				{
					const rrRenderRequest&  l_4r = l_4rGroup[iLayer].m_4rDeferred[iObject];
					RrRenderObject* renderable = l_4r.obj;

					RrRenderObject::rrRenderParams params;
					params.pass = l_4r.pass;
					params.pass_type = kPassTypeDeferred;
					params.cbuf_perCamera = &cameraPass->m_cbuffer;
					params.cbuf_perFrame = &state->frame_state->cbuffer_perFrame;
					params.cbuf_perPass = nullptr;
					params.context = &render_context;
			
					ARCORE_ASSERT(params.context != nullptr);
					renderable->PrepRender(cameraPass);
					renderable->Render(&params);
				}
			}
			gfx->debugGroupPop();

			// Run a composite pass. This often includes partial post-processing and other inputs
			if (state->pipeline_renderer != nullptr)
			{
				rrPipelineCompositeInput input;

				input.layer = (renderer::rrRenderLayer)iLayer;
				input.deferred_albedo = &outputGbuffers[0];
				input.deferred_normals = &outputGbuffers[1];
				input.deferred_surface = &outputGbuffers[2];
				input.deferred_emissive = &outputGbuffers[3];
				input.combined_depth = &outputDepth;
				input.old_forward_color = &outputColor;
				input.cameraPass = cameraPass;

				rrCompositeOutput output = state->pipeline_renderer->CompositeDeferred(&render_context, input, state);
				outputColor = output.color;
			}
		}

		if (!l_4rGroup[iLayer].m_4rForward.empty())
		{
			// Set up the layout for the GBuffer forward pass
			static constexpr int kColorOutputCount = 2;
			gpu::Texture outputGbuffers [kColorOutputCount] = {
				outputColor,
				outputNormals,
			};

			// Set up output
			rrRenderTarget forwardOpaqueTarget ( &outputDepth, &outputStencil, outputGbuffers, kColorOutputCount );
			gfx->setRenderTarget(&forwardOpaqueTarget.m_renderTarget); // TODO: Binding buffers at the right time.

			// Set up color draw for forward mode. Draw normally.
			{
				gpu::BlendState bs;
				bs.channelMask = 0xFF; // Enable color masking.
				bs.enable = false;
				gfx->setBlendState(bs);
			}

			// Set up viewport
			{
				gfx->setViewport(
					cameraPass->m_viewport.corner.x,
					cameraPass->m_viewport.corner.y,
					cameraPass->m_viewport.corner.x + cameraPass->m_viewport.size.x,
					cameraPass->m_viewport.corner.y + cameraPass->m_viewport.size.y );
				gfx->setScissor(
					cameraPass->m_viewport.corner.x,
					cameraPass->m_viewport.corner.y,
					cameraPass->m_viewport.corner.x + cameraPass->m_viewport.size.x,
					cameraPass->m_viewport.corner.y + cameraPass->m_viewport.size.y );
			}

			gfx->debugGroupPush("Forward Opaques");
			// Set default depth-equal
			{
				gpu::DepthStencilState ds;
				ds.depthTestEnabled   = true;
				ds.depthWriteEnabled  = false;
				ds.depthFunc = gpu::kCompareOpEqual;
				ds.stencilTestEnabled = false;
				gfx->setDepthStencilState(ds);
			}

			// Do the forward pass:
			for (size_t iObject = 0; iObject < l_4rGroup[iLayer].m_4rForward.size(); ++iObject)
			{
				const rrRenderRequest&  l_4r = l_4rGroup[iLayer].m_4rForward[iObject];
				RrRenderObject* renderable = l_4r.obj;

				RrRenderObject::rrRenderParams params;
				params.pass = l_4r.pass;
				params.pass_type = kPassTypeForward;
				params.cbuf_perCamera = &cameraPass->m_cbuffer;
				params.cbuf_perFrame = &state->frame_state->cbuffer_perFrame;
				params.cbuf_perPass = nullptr;
				params.context = &render_context;

				ARCORE_ASSERT(params.context != nullptr);
				renderable->PrepRender(cameraPass);
				renderable->Render(&params);
			}
			gfx->debugGroupPop();
		}

		// Do post-opaque work
		if (!l_4rGroup[iLayer].m_4rDepthPrepass.empty())
		{
			if (state->pipeline_renderer != nullptr)
			{
				rrPipelinePostOpaqueCompositeInput input;

				input.layer = (renderer::rrRenderLayer)iLayer;
				input.combined_color = &outputColor;
				input.combined_normals = &outputNormals;
				input.combined_depth = &outputDepth;
				input.cameraPass = cameraPass;

				rrCompositeOutput output = state->pipeline_renderer->CompositePostOpaques(&render_context, input, state);
				outputColor = output.color;
			}
		}

		if (!l_4rGroup[iLayer].m_4rForwardTranslucent.empty())
		{
			// Set up output
			rrRenderTarget forwardTranslucentTarget ( &outputDepth, &outputStencil, &outputColor );
			gfx->setRenderTarget(&forwardTranslucentTarget.m_renderTarget); // TODO: Binding buffers at the right time.

			// Set up color draw for forward mode. Draw normally.
			{
				gpu::BlendState bs;
				bs.channelMask = 0xFF; // Enable color masking.
				bs.enable = false;
				gfx->setBlendState(bs);
			}

			// Set up viewport
			{
				gfx->setViewport(
					cameraPass->m_viewport.corner.x,
					cameraPass->m_viewport.corner.y,
					cameraPass->m_viewport.corner.x + cameraPass->m_viewport.size.x,
					cameraPass->m_viewport.corner.y + cameraPass->m_viewport.size.y );
				gfx->setScissor(
					cameraPass->m_viewport.corner.x,
					cameraPass->m_viewport.corner.y,
					cameraPass->m_viewport.corner.x + cameraPass->m_viewport.size.x,
					cameraPass->m_viewport.corner.y + cameraPass->m_viewport.size.y );
			}

			gfx->debugGroupPush("Forward Translucents");
			// Set default less-than-equal depth test
			{
				gpu::DepthStencilState ds;
				ds.depthTestEnabled   = true;
				ds.depthWriteEnabled  = false;
				ds.depthFunc = gpu::kCompareOpLessEqual;
				ds.stencilTestEnabled = false;
				gfx->setDepthStencilState(ds);
			}

			// Do the pre-multiplied forward translucent pass:
			for (size_t iObject = 0; iObject < l_4rGroup[iLayer].m_4rForwardTranslucent.size(); ++iObject)
			{
				const rrRenderRequest&  l_4r = l_4rGroup[iLayer].m_4rForwardTranslucent[iObject];
				RrRenderObject* renderable = l_4r.obj;

				RrRenderObject::rrRenderParams params;
				params.pass = l_4r.pass;
				params.pass_type = kPassTypeForward;
				params.cbuf_perCamera = &cameraPass->m_cbuffer;
				params.cbuf_perFrame = &state->frame_state->cbuffer_perFrame;
				params.cbuf_perPass = nullptr;
				params.context = &render_context;

				ARCORE_ASSERT(params.context != nullptr);
				renderable->PrepRender(cameraPass);
				renderable->Render(&params);
			}
			gfx->debugGroupPop();
		}

		// finish off the layer
		if (state->pipeline_renderer != nullptr)
		{
			rrPipelineLayerFinishInput input;

			input.layer = (renderer::rrRenderLayer)iLayer;
			input.color = &outputColor;
			input.depth = &outputDepth;
			input.cameraPass = cameraPass;

			rrPipelineOutput output = state->pipeline_renderer->RenderLayerEnd(&render_context, input, state);
			outputColor = output.color;
		}

		gfx->debugGroupPop();
	}

	// at the end, copy the aggregate forward RT onto the render target (do that outside of this function!)
	return outputColor;
}

#pragma warning( default : 4102 )