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
//#include "renderer/texture/CMRTTexture.h"

//#include "renderer/material/RrMaterial.h"
#include "renderer/object/RrRenderObject.h"

#include "gpuw/Pipeline.h"
#include "gpuw/ShaderPipeline.h"
#include "gpuw/OutputSurface.h"
#include "renderer/material/RrPass.h"
#include "renderer/material/RrShaderProgram.h"
#include "renderer/state/RrPipeline.h"
#include "renderer/state/RrPipelinePasses.h"
#include "renderer/windowing/RrWindow.h"

#include <algorithm>

//#include "renderer/resource/CResourceManager.h"

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

void RrRenderer::StepPreRender ( void )
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
		// Push to GPU:
		//internal_cbuffers_frames[internal_chain_index].upload(NULL, &frameInfo, sizeof(renderer::cbuffer::rrPerFrame), gpu::kTransferStream);
		internal_cbuffers_frames[frame_index % internal_cbuffers_frames.size()].upload(NULL, &frameInfo, sizeof(renderer::cbuffer::rrPerFrame), gpu::kTransferStream);
	}

	// Update the per-pass constant buffer data:
	for (size_t outputIndex = 0; outputIndex < render_outputs.size(); ++outputIndex )
	{
		RrOutputState* state = render_outputs[outputIndex].state;
		if (state != nullptr)
		{
			for (int iLayer = renderer::kRenderLayer_BEGIN; iLayer < renderer::kRenderLayer_MAX; ++iLayer)
			{
				renderer::cbuffer::rrPerPassLightingInfo passInfo = {}; // Unused. May want to remove later...

				state->internal_cbuffers_passes[state->internal_chain_index * renderer::kRenderLayer_MAX + iLayer]
					.upload(NULL, &passInfo, sizeof(renderer::cbuffer::rrPerPassLightingInfo), gpu::kTransferStream);
			}
		}
	}
	//for (int iLayer = renderer::kRenderLayer_BEGIN; iLayer < renderer::kRenderLayer_MAX; ++iLayer)
	//{
	//	renderer::cbuffer::rrPerPassLightingInfo passInfo = {}; // Unused. May want to remove later...

	//	internal_cbuffers_passes[internal_chain_index * renderer::kRenderLayer_MAX + iLayer]
	//		.upload(NULL, &passInfo, sizeof(renderer::cbuffer::rrPerPassLightingInfo), gpu::kTransferStream);
	//}

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
	/*for ( i = 0; i < mLoCurrentIndex; ++i ) {
		if ( mLogicObjects[i] && mLogicObjects[i]->active ) {
			mLogicObjects[i]->PreStepSynchronus();
		}
	}*/

	// Update the streamed loading system
	//if ( mResourceManager ) {
	//	mResourceManager->RenderUpdate();
	//}
	// TODO:
	auto resourceManager = core::ArResourceManager::Active();
	//resourceManager->Update(); // TODO: This updates ALL render systems. This will stall if we're handling non-render resources too
	resourceManager->UpdateManual(core::kResourceTypeRrTexture);

	// Call begin render
	TimeProfiler.BeginTimeProfile( "rs_begin_render" );
	/*for ( i = 0; i < iCurrentIndex; i += 1 )
	{
	if ( pRenderableObjects[i] ) {
	pRenderableObjects[i]->BeginRender(); //massive slowdown
	}
	}*/
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
	/*for ( i = 0; i < iCurrentIndex; i += 1 )
	{
		if ( pRenderableObjects[i] ) {
			pRenderableObjects[i]->EndRender();
		}
	}*/
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
	/*for ( i = 0; i < mLoCurrentIndex; ++i ) {
		if ( mLogicObjects[i] && mLogicObjects[i]->active ) {
			core::jobs::System::Current::AddJobRequest( core::jobs::kJobTypeRenderStep, &(RrLogicObject::PostStep), mLogicObjects[i] );
		}
	}*/
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
	/*for ( i = 0; i < mLoCurrentIndex; ++i ) {
		if ( mLogicObjects[i] && mLogicObjects[i]->active ) {
			mLogicObjects[i]->PostStepSynchronus();
		}
	}*/
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

void RrRenderer::StepBufferPush ( gpu::GraphicsContext* gfx, const RrOutputInfo& output, RrOutputState* state )
{
	// TODO: Make it go to the output & state

	// Are we in buffer mode? (This should always be true)
	gfx->debugGroupPush("Buffer Push");
	TimeProfiler.BeginTimeProfile( "rs_buffer_push" );
	//if ( bufferedMode )
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

			gfx->setPipeline(&pipelinePasses->m_pipelineScreenQuadCopy);
			gfx->setVertexBuffer(0, &pipelinePasses->m_vbufScreenQuad_ForOutputSurface, 0); // see RrPipelinePasses.cpp
			gfx->setVertexBuffer(1, &pipelinePasses->m_vbufScreenQuad_ForOutputSurface, 0); // there are two binding slots defined with different stride
			gfx->setShaderTextureAuto(gpu::kShaderStagePs, 0,
										&state->internal_chain_current->texture_color);
				                      //state->internal_chain_current->buffer_forward_rt.getAttachment(gpu::kRenderTargetSlotColor0));
			gfx->draw(4, 0);
		}
		gfx->clearPipelineAndWait(); // Wait until we're done using the buffer...

		// Clear the buffer after we're done with it.
		gfx->setRenderTarget(&state->internal_chain_current->buffer_forward_rt);
		gfx->setViewport(output_viewport.corner.x, output_viewport.corner.y, output_viewport.corner.x + output_viewport.size.x, output_viewport.corner.y + output_viewport.size.y);
		gfx->clearDepthStencil(true, 1.0F, true, 0x00);
		float clearColor[] = {0, 0, 0, 0};
		gfx->clearColor(clearColor);
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

	// Update pre-render steps:
	//	- RrLogicObject::PostStep finish
	//	- RrCamera::LateUpdate
	//	- RrLogicObject::PreStep, RrLogicObject::PreStepSynchronous
	//	- ResourceManager::update
	//	- BeginRender
	StepPreRender(); 

	// Check for a main camera to work with
	if ( RrCamera::activeCamera == NULL )
	{
		return; // Don't render if there's no camera to render with...
	}
	//mainBufferCamera = RrCamera::activeCamera;

	RrCamera* prevActiveCam = RrCamera::activeCamera;

	// Update window buffer
	/*TimeProfiler.BeginTimeProfile( "WD_Swap" );
	SwapBuffers(RrWindow::pActive->getDevicePointer());				// Swap Buffers (Double Buffering) (VSYNC)
	//wglSwapLayerBuffers(aWindow.getDevicePointer(),WGL_SWAP_MAIN_PLANE );
	TimeProfiler.EndTimeProfile( "WD_Swap" );*/

	std::vector<gpu::GraphicsContext*> collected_contexts;
	std::vector<gpu::OutputSurface*> collected_outputs;

	// Prepare frame
	//TimeProfiler.BeginTimeProfile( "rs_present" );
	//gfx->reset();
	//TimeProfiler.EndTimeProfile( "rs_present" );
	
	// Loop through all cameras (todo: sort render order for each camera)
	TimeProfiler.BeginTimeProfile( "rs_camera_matrix" );
	/*auto l_currentCamera = RrCamera::GetFirstCamera();
	while (l_currentCamera != RrCamera::GetLastCamera())
	{
		if ( l_currentCamera->GetRender() )
		{
			l_currentCamera->UpdateMatrix();
		}
		l_currentCamera = l_currentCamera->GetNextCamera();
	}*/
	for ( size_t outputIndex = 0; outputIndex < render_outputs.size(); ++outputIndex )
	{
		auto& render_output = render_outputs[outputIndex];
		if (render_output.info.enabled)
		{
			ARCORE_ASSERT(render_output.info.camera != nullptr);
			render_output.info.camera->UpdateMatrix(render_output.info);
		}
	}
	//auto l_cameraList = RrCamera::CameraList();
	//for ( i = 0; i < l_cameraList.size(); ++i )
	//{
	//	// TODO: Thread this.
	//	RrCamera* currentCamera = l_cameraList[i];
	//	// Update the camera positions and matrices
	//	if ( currentCamera != NULL && currentCamera->GetRender() )
	//	{
	//		currentCamera->UpdateMatrix();
	//	}
	//}
	// Double check to make sure the zero camera is the active camera
	//if ( l_cameraList[0] != prevActiveCam )
	//{
	//	std::cout << "Error in rendering: invalid zero index camera! (Seriously, this CAN'T happen)" << std::endl;
	//	// Meaning, resort the camera list
	//	/*for ( auto it = RrCamera::vCameraList.begin(); it != RrCamera::vCameraList.end(); )
	//	{
	//		if ( *it == prevActiveCam )
	//		{
	//			it = RrCamera::vCameraList.erase(it);
	//			RrCamera::vCameraList.insert( RrCamera::vCameraList.begin(), prevActiveCam );
	//		}
	//		else
	//		{
	//			++it;
	//		}
	//	}*/
	//	return; // Don't render past here, then.
	//}
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
			render_output.state->Update(&render_output.info);

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

			// Render with the given camera.
			ARCORE_ASSERT(render_output.info.camera != nullptr);
			if (render_output.info.camera->GetRender())
			{
				// Update the common culling information (This should happen in the RenderScene routine)
				//for ( int i = 0; i < 

				// Render from camera
				RrCamera::activeCamera = render_output.info.camera;
				gfx->debugGroupPush(render_output.info.name.c_str());
				// We drop the const on the world because rendering objects changes their state
				RenderOutput(gfx, render_output.info, render_output.state, render_output.info.world);
				gfx->debugGroupPop();
			}

			// Push buffer to screen
			StepBufferPush(gfx, render_output.info, render_output.state);
		}
	}
	TimeProfiler.EndTimeProfile( "rs_render" );

	// Loop through all cameras:
	/*TimeProfiler.BeginTimeProfile( "rs_render" );
	for ( std::vector<RrCamera*>::reverse_iterator it = l_cameraList.rbegin(); it != l_cameraList.rend(); ++it )
	{
		RrCamera* currentCamera = *it;

		// Only render with current camera if should be rendering
		if ( currentCamera->GetRender() )
		{
			// Update the render distance of objects against this camera:
			for ( i = 0; i < iCurrentIndex; i += 1 )
			{	// Put into it's own loop, since it's the same calculation across all objects
				if ( pRenderableObjects[i] )
				{ 
					// TODO: Handle 2D mode properly. Postprocess is entirely sorted by transform.position.z Should this be made more consistent?
					pRenderableObjects[i]->renderDistance = (currentCamera->transform.position - pRenderableObjects[i]->transform.world.position).sqrMagnitude();
				}
			}

			// Render from camera
			RrCamera::activeCamera = currentCamera;
			gfx->debugGroupPush("Camera: RenderScene()");
			RenderScene(currentCamera);
			gfx->debugGroupPop();
		}
	}
	TimeProfiler.EndTimeProfile( "rs_render" );*/
	
	// Set active camera back to default
	RrCamera::activeCamera = prevActiveCam;

	// Signal frame ended
	//gfx->submit();
	for (gpu::GraphicsContext* gfx : collected_contexts)
	{
		// Submit the commands now
		gfx->submit();
		// Validate the GPU state isn't completely broken
		gfx->validate();
	}

	// Query up all the outputs we were working on
	// Present the output buffer
	//mOutputSurface->present(); // TODO: On Vulkan and other platforms, this will likely need a fence.
	for (gpu::OutputSurface* surface : collected_outputs)
	{
		surface->present();
	}

	// Cycle the draw inputs
	//{
	//	// Go to next input
	//	internal_chain_index = (internal_chain_index + 1) % internal_chain_list.size();
	//	// Pull its pointer
	//	internal_chain_current = &internal_chain_list[internal_chain_index];
	//}
	for ( size_t outputIndex = 0; outputIndex < render_outputs.size(); ++outputIndex )
	{
		RrOutputState* state = render_outputs[outputIndex].state;
		if (state != nullptr)
		{
			// Go to next input
			state->internal_chain_index = (state->internal_chain_index + 1) % state->internal_chain_list.size();
			// Pull its pointer
			state->internal_chain_current = &state->internal_chain_list[state->internal_chain_index];
		}
	}
	// Count the global frame rendered
	frame_index++;

	// Do error check at this point
	//if ( RrMaterial::Default->passinfo.size() > 16 ) throw std::exception();
	//gfx->validate();

	//// Query up all the outputs we were working on
	//// Present the output buffer
	////mOutputSurface->present(); // TODO: On Vulkan and other platforms, this will likely need a fence.
	//for (gpu::OutputSurface* surface : collected_outputs)
	//{
	//	surface->present();
	//}

	// Call post-render and start up new jobs
	StepPostRender();
}


//
//// Normal rendering routine. Called by a camera. Camera passes in its render hint.
//// This function will render to the scene targets needed.
//void RrRenderer::RenderSceneForward ( const uint32_t n_renderHint )
//{
//	// TODO: FIX ALL THIS
//
//	//GL_ACCESS GLd_ACCESS
//	// Save current mode
//	eRenderMode t_currentRenderMode = renderMode;
//	// Override it with forward
//	renderMode = kRenderModeForward;
//
//	RrRenderObject * pRO;
//	tRenderRequest	renderRQ;
//	int i;
//	unsigned char passCount;
//	RrCamera* currentCamera = RrCamera::activeCamera;
//
//	// Loop through each hint (reverse mode)
//	for ( uint currentLayer = 1<<kRenderHintCOUNT; currentLayer != 0; currentLayer >>= 1 )
//	{
//		int layer = math::log2( currentLayer );
//		// Skip non-drawn hints
//		if ( !(currentLayer & n_renderHint) ) {
//			continue;
//		}
//		// Skip non-world if no buffer model
//		if ( !bufferedMode ) {
//			if ( currentLayer != kRenderHintWorld ) {
//				continue;
//			}
//		}
//
//		// Create a sorted render list.
//		std::vector<tRenderRequest> sortedRenderList;
//		std::vector<RrRenderObject*> sortSinglePassList;
//		for ( i = 0; i < (int)iCurrentIndex; i += 1 )
//		{
//			pRO = pRenderableObjects[i];
//			if ( pRO /*&& pRO->visible*/ )
//			{
//				// Only add to list if visible in this hint.
//				if ( pRO->renderSettings.renderHints & currentLayer )
//				{
//					bool added = false;
//					// Add each pass to the render list
//					passCount = pRO->GetPassNumber();
//					for ( unsigned char p = 0; p < passCount; ++p )
//					{
//						RrPassForward* pass = pRO->GetPass(p);
//						// Mask the render hint (multipass across multiple targets)
//						if ( pass->m_hint & currentLayer )
//						{
//							// Check for depth writing (depth write comes first)
//							bool depthmask = (pass->m_transparency_mode!=ALPHAMODE_TRANSLUCENT)&&(pass->b_depthmask);
//
//							tRenderRequest newRequest;
//							newRequest.obj  = pRO;
//							newRequest.pass = p;	
//							newRequest.transparent = !depthmask;
//							newRequest.screenshader = pRO->m_material->m_isScreenShader;
//
//							sortedRenderList.push_back( newRequest );
//							added = true;
//						}
//					}
//					// Add the single-pass object to the listing.
//					if (added)
//					{
//						sortSinglePassList.push_back(pRO);
//					}
//				}
//			}
//		}
//		// Sort the render requests
//		std::sort( sortedRenderList.begin(), sortedRenderList.end(), OrderComparatorForward ); 
//
//		// If using buffer model
//		if ( bufferedMode )
//		{
//			// For the final camera, render to a buffer
//			if ( currentLayer == kRenderHintWorld && currentCamera == mainBufferCamera )
//			{
//				RrRenderTexture* currentMainRenderTarget = GL.GetMainScreenBuffer();
//				currentMainRenderTarget->BindBuffer();
//				GL.setupViewport(0,0,currentMainRenderTarget->GetWidth(),currentMainRenderTarget->GetHeight());
//				GL.CheckError();
//			}
//			/*else
//			{
//				// Bind the proper layer buffer
//				int layer = Math.log2( currentLayer );
//			}*/
//		}
//
//		// Calculate pass globals
//		renderer::Settings.fogScale = 1.0f / (renderer::Settings.fogEnd - renderer::Settings.fogStart);//currentCamera->zFar
//		// Update UBOs
//		RrMaterial::updateStaticUBO();
//
//		// Clear with BG color
//		glClear( GL_COLOR_BUFFER_BIT ); // use rendereing options for this
//
//		int sortedListSize = sortedRenderList.size();
//
//		// First work on prerender
//		for ( i = 0; i < (int)sortSinglePassList.size(); ++i )
//		{
//			sortSinglePassList[i]->PreRender();
//		}
//
//		// Then work on the actual render
//		for ( int rt = kRenderLayer_BEGIN; rt < kRenderLayer_MAX; rt += 1 )
//		{
//			if ( !currentCamera->layerVisibility[rt] ) { // move to renderer generator
//				continue;
//			}
//
//			for ( i = 0; i < (int)sortedListSize; ++i )
//			{
//				renderRQ = sortedRenderList[i];
//				pRO = renderRQ.obj;
//				//if (( pRO )&&( pRO->renderLayer == rt ))
//				if ( pRO->visible && pRO->renderLayer == rt )
//				{
//					//GL.prepareDraw();
//					if ( !pRO->Render( renderRQ.pass ) ) {
//						throw std::exception();
//					}
//					GL.CheckError();
//					//GL.cleanupDraw();
//				}
//			}
//			// Between layers, clear depth
//			glDepthMask( GL_TRUE );
//			glClear( GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
//		}
//
//		// Finally, work on postrender
//		for ( i = 0; i < (int)sortSinglePassList.size(); ++i )
//		{
//			sortSinglePassList[i]->PostRender();
//			GL.CheckError();
//		}
//		// End render loop
//
//		// Unbind fbo when done rendering
//		if ( bufferedMode )
//		{
//			// For the final camera, render to a buffer
//			if ( currentLayer == kRenderHintWorld && currentCamera == mainBufferCamera )
//			{
//				GL.GetMainScreenBuffer()->UnbindBuffer();
//			}
//			else
//			{
//				// Unbind the proper layer buffer
//				int layer = math::log2( currentLayer );
//			}
//		}
//	}
//
//	// Restore render mode
//	renderMode = t_currentRenderMode;
//}

// Some debug compiler flags
//#define SKIP_NON_WORLD_STUFF
#define FORCE_BUFFER_CLEAR
//#define ENABLE_RUNTIME_BLIT_TEST

void RrRenderer::RenderOutput ( gpu::GraphicsContext* gfx, const RrOutputInfo& output, RrOutputState* state, RrWorld* world )
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
		state->pipeline_renderer->CullObjects(gfx, output, state, world);
	}

Render_Camera_Passes:
	// Get a pass list from the camera
	const int kMaxCameraPasses = 8;
	rrCameraPass cameraPasses [kMaxCameraPasses];
	rrCameraPassInput cameraPassInput;
	cameraPassInput.m_maxPasses = kMaxCameraPasses;
	cameraPassInput.m_bufferingCount = (uint16_t)std::min<size_t>(state->internal_chain_list.size(), 0xFFFF);
	cameraPassInput.m_bufferingIndex = state->internal_chain_index;
	cameraPassInput.m_outputInfo = &output;
	cameraPassInput.m_graphicsContext = gfx;

	int passCount = output.camera->PassCount();
	output.camera->PassRetrieve(&cameraPassInput, cameraPasses);

	// Begin rendering now
	output.camera->RenderBegin();

	// Loop through each pass and render with them:
	for (int iCameraPass = 0; iCameraPass < passCount; ++iCameraPass)
	{
		if (cameraPasses[iCameraPass].m_passType == kCameraRenderWorld)
		{
			RenderObjectListWorld(gfx, &cameraPasses[iCameraPass], world->objects.data(), (uint32_t)world->objects.size(), state);
		}
		else
		{
			ARCORE_ERROR("Only single pass supported");
		}
	}

	// Finish up the rendering
	output.camera->RenderEnd();
}

////	RenderObjectList () : Renders the object list from the given camera with DeferredForward+.
//void RrRenderer::RenderObjectList ( RrCamera* camera, RrRenderObject** objectsToRender, const uint32_t objectCount )
//{
//	// Get a pass list from the camera
//	const int kMaxCameraPasses = 8;
//	rrCameraPass cameraPasses [kMaxCameraPasses];
//	rrCameraPassInput cameraPassInput;
//	cameraPassInput.m_maxPasses = kMaxCameraPasses;
//	cameraPassInput.m_bufferingCount = (uint16_t)std::min<size_t>(internal_chain_list.size(), 0xFFFF);
//	cameraPassInput.m_bufferingIndex = internal_chain_index;
//
//	int passCount = camera->PassCount();
//	camera->PassRetrieve(&cameraPassInput, cameraPasses);
//
//	// Begin rendering now
//	camera->RenderBegin();
//
//	// Loop through each pass and render with them:
//	for (int iCameraPass = 0; iCameraPass < passCount; ++iCameraPass)
//	{
//		if (cameraPasses[iCameraPass].m_passType == kCameraRenderWorld)
//		{
//			RenderObjectListWorld(&cameraPasses[iCameraPass], objectsToRender, objectCount);
//		}
//		else if (cameraPasses[iCameraPass].m_passType == kCameraRenderShadow)
//		{
//			//RenderObjectListShadows(&cameraPasses[iCameraPass], objectsToRender, objectCount);
//			ARCORE_ERROR("Shadow pass not yet implemented.");
//		}
//	}
//
//	// Finish up the rendering
//	camera->RenderEnd();
//}

void RrRenderer::RenderObjectListWorld ( gpu::GraphicsContext* gfx, rrCameraPass* cameraPass, RrRenderObject** objectsToRender, const uint32_t objectCount, RrOutputState* state )
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
			if (renderable != NULL)
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
					renderable->PreRender(cameraPass);
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
				if (renderable != NULL)
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
						//renderable->PreRender(cameraPass);
						l_4rGroup[iLayer].m_enabled = true;
					}
					/*else
					{
						// For now, display an error that there is no pass:
						debug::Console->PrintWarning("An object has no enabled passes!\n");
						ARCORE_ERROR("An object has no passes. It's possible this is not a mistake. Please review and revise as necessary.");
					}*/
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
	// Lighting needs to be handled by the pipeline info
	if (state->pipeline_renderer)
	{
		state->pipeline_renderer->PreparePass(gfx);
	}

Render_Groups:
	// select buffer chain we work with
	RrHybridBufferChain* bufferChain = cameraPass->m_bufferChain;
	if (bufferChain == NULL)
	{	// Select the main buffer chain if no RT is defined.
		bufferChain = state->internal_chain_current;
	}
	ARCORE_ASSERT(bufferChain != NULL);

	// target proper buffer
	//gfx->setRenderTarget(&bufferChain->buffer_forward_rt); // TODO: Binding buffers at the right time.
	
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

		int l_cbuffer_pass_index = state->internal_chain_index * renderer::kRenderLayer_MAX + iLayer;

		// Do the forward pass:
		for (size_t iObject = 0; iObject < l_4rGroup[iLayer].m_4rJob.size(); ++iObject)
		{
			const rrRenderRequest&  l_4r = l_4rGroup[iLayer].m_4rJob[iObject];
			RrRenderObject* renderable = l_4r.obj;

			RrRenderObject::rrRenderParams params;
			params.pass = l_4r.pass;
			params.cbuf_perCamera = cameraPass->m_cbuffer;
			params.cbuf_perFrame = &internal_cbuffers_frames[frame_index % internal_cbuffers_frames.size()];
			params.cbuf_perPass = &state->internal_cbuffers_passes[l_cbuffer_pass_index];
			params.context_graphics = gfx;

			ARCORE_ASSERT(params.context_graphics != nullptr);
			renderable->Render(&params);
		}
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

		int l_cbuffer_pass_index = state->internal_chain_index * renderer::kRenderLayer_MAX + iLayer;

		bool dirty_deferred = false;
		bool dirty_forward = false;

		gfx->debugGroupPush((std::string("Rendering Layer ") + std::to_string((int)iLayer)).c_str());

		// Set up output
		{
			gfx->setRenderTarget(&bufferChain->buffer_forward_rt); // TODO: Binding buffers at the right time.
		}

		gfx->debugGroupPush("Depth Pre-pass");
		{
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
				params.cbuf_perCamera = cameraPass->m_cbuffer;
				params.cbuf_perFrame = &internal_cbuffers_frames[frame_index % internal_cbuffers_frames.size()];
				params.cbuf_perPass = &state->internal_cbuffers_passes[l_cbuffer_pass_index];
				params.context_graphics = gfx;

				ARCORE_ASSERT(params.context_graphics != nullptr);
				renderable->Render(&params);
			}
		}
		gfx->debugGroupPop();

		gfx->debugGroupPush("Deferred Opaques");
		{
			// Set up output
			{
				gfx->setRenderTarget(&bufferChain->buffer_deferred_mrt); // TODO: Binding buffers at the right time.
			}

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
			gfx->clearColor(clearColor);

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
				params.cbuf_perCamera = cameraPass->m_cbuffer;
				params.cbuf_perFrame = &internal_cbuffers_frames[frame_index % internal_cbuffers_frames.size()];
				params.cbuf_perPass = &state->internal_cbuffers_passes[l_cbuffer_pass_index];
				params.context_graphics = gfx;
			
				ARCORE_ASSERT(params.context_graphics != nullptr);
				renderable->Render(&params);
			}
		}
		gfx->debugGroupPop();

		// check if rendered anything, mark screen dirty if so.
		if (!l_4rGroup[iLayer].m_4rDeferred.empty())
			dirty_deferred = true;

		// run a composite pass if deferred is dirty
		if (dirty_deferred)
		{
			// Mark forward as dirty so output will happen even without a forward object.
			dirty_forward = true;
		}

		gfx->debugGroupPush("Forward Pass");
		{
			// Set up output
			{
				gfx->setRenderTarget(&bufferChain->buffer_forward_rt); // TODO: Binding buffers at the right time.
			}

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
				params.cbuf_perCamera = cameraPass->m_cbuffer;
				params.cbuf_perFrame = &internal_cbuffers_frames[frame_index % internal_cbuffers_frames.size()];
				params.cbuf_perPass = &state->internal_cbuffers_passes[l_cbuffer_pass_index];
				params.context_graphics = gfx;

				ARCORE_ASSERT(params.context_graphics != nullptr);
				renderable->Render(&params);
			}
			gfx->debugGroupPop();

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

			// Do the forward translucent pass:
			for (size_t iObject = 0; iObject < l_4rGroup[iLayer].m_4rForwardTranslucent.size(); ++iObject)
			{
				const rrRenderRequest&  l_4r = l_4rGroup[iLayer].m_4rForwardTranslucent[iObject];
				RrRenderObject* renderable = l_4r.obj;

				RrRenderObject::rrRenderParams params;
				params.pass = l_4r.pass;
				params.pass_type = kPassTypeForward;
				params.cbuf_perCamera = cameraPass->m_cbuffer;
				params.cbuf_perFrame = &internal_cbuffers_frames[frame_index % internal_cbuffers_frames.size()];
				params.cbuf_perPass = &state->internal_cbuffers_passes[l_cbuffer_pass_index];
				params.context_graphics = gfx;

				ARCORE_ASSERT(params.context_graphics != nullptr);
				renderable->Render(&params);
			}
			gfx->debugGroupPop();
		}
		gfx->debugGroupPop();

		// check if rendered anything, mark screen dirty if so.
		if (!l_4rGroup[iLayer].m_4rForward.empty() || !l_4rGroup[iLayer].m_4rForwardTranslucent.empty())
			dirty_forward = true;

		// run another composite if forward is dirty
		if (dirty_deferred)
		{
			// TODO
			if (state->pipeline_renderer != nullptr)
			{
				rrPipelineCompositeInput input;

				input.deferred_albedo = &bufferChain->texture_deferred_color[0];
				input.deferred_normals = &bufferChain->texture_deferred_color[1];
				input.deferred_surface = &bufferChain->texture_deferred_color[2];
				input.deferred_emissive = &bufferChain->texture_deferred_color[3];
				input.combined_depth = &bufferChain->texture_depth;
				input.forward_color = dirty_forward ? &bufferChain->texture_color : nullptr;

				input.output_color = &bufferChain->texture_deferred_color_composite;

				state->pipeline_renderer->CompositeDeferred(gfx, input, state);

				// Quietly swap the inputs & outputs here.
				std::swap(bufferChain->buffer_deferred_rt, bufferChain->buffer_forward_rt);
				std::swap(bufferChain->texture_deferred_color_composite, bufferChain->texture_color);
			}
		}

		// finish off the layer
		/*if (state->pipeline_renderer != nullptr)
		{
			rrPipelineLayerFinishInput input;

			input.layer = (renderer::rrRenderLayer)iLayer;
			input.color = &bufferChain->texture_color;
			input.depth = &bufferChain->texture_depth;

			input.output_color = &bufferChain->texture_deferred_color_composite;

			state->pipeline_renderer->RenderLayerEnd(gfx, input, state);

			// Quietly swap the inputs & outputs here.
			std::swap(bufferChain->buffer_deferred_rt, bufferChain->buffer_forward_rt);
			std::swap(bufferChain->texture_deferred_color_composite, bufferChain->texture_color);
		}*/

		gfx->debugGroupPop();
	}

	// at the end, copy the aggregate forward RT onto the render target (do that outside of this function!)

}

#pragma warning( default : 4102 )

//
//// Deferred rendering routine.
//void RrRenderer::RenderScene ( const uint32_t renderHint, RrCamera* camera )
//{
//	GL_ACCESS GLd_ACCESS
//	RrRenderObject * pRO;
//	int i;
//	unsigned char passCount;
//	RrCamera* currentCamera = RrCamera::activeCamera;
//	bool firstRender = true;
//
//	// Create a sorted render list:
//	TimeProfiler.BeginTimeProfile( "rs_render_makelist" );
//	// Build the unsorted data
//	std::vector<tRenderRequest> sortedRenderList;
//	std::vector<RrRenderObject*> sortSinglePassList;
//	for ( i = 0; i < (int)iCurrentIndex; i += 1 )
//	{
//		pRO = pRenderableObjects[i];
//		if ( pRO )
//		{
//#ifdef SKIP_NON_WORLD_STUFF
//			// Only add 2D or world objects for now
//			if ( pRO->renderLayer != renderer::kRLWorld && pRO->renderLayer != renderer::kRLV2D ) { 
//				continue;
//			}
//#endif
//			// Only add to list if visible in this hint.
//			if ( pRO->renderSettings.renderHints & n_renderHint )
//			{
//				bool added = false;
//
//				// Add each pass to the render list
//				passCount = pRO->GetPassNumber();
//				for ( unsigned char p = 0; p < passCount; ++p )
//				{
//					RrPassDeferred* pass = pRO->GetPassDeferred(p);
//					// Mask the render hint (multipass across multiple targets)
//					//if ( pass->m_hint & currentLayer )
//					{
//						tRenderRequest newRequest;
//						newRequest.obj  = pRO;
//						newRequest.pass = p;
//						newRequest.forward = false;
//						newRequest.renderLayer = pRO->renderLayer;
//						newRequest.transparent = false;
//						newRequest.screenshader = false;
//						sortedRenderList.push_back( newRequest );
//						added = true;
//					}
//				}
//				if ( passCount == 0 ) // No deferred passes? Must be a world object.
//				{
//					if ( pRO->GetMaterial() && pRO->GetMaterial()->passinfo.size() )
//					{
//						RrPassForward* pass = pRO->GetPass(0);
//						if ( pass->m_hint & kRenderHintWorld )
//						{
//							// Check for depth writing
//							bool depthmask = (pass->m_transparency_mode!=ALPHAMODE_TRANSLUCENT)&&(pass->b_depthmask);
//
//							tRenderRequest newRequest;
//							newRequest.obj  = pRO;
//							newRequest.pass = 0;
//							newRequest.forward = true;
//							newRequest.renderLayer = pRO->renderLayer;
//							newRequest.transparent = !depthmask;
//							newRequest.screenshader = pRO->m_material->m_isScreenShader;
//							sortedRenderList.push_back( newRequest );
//							added = true;
//						}
//					}
//				}
//
//				// Add the single-pass object to the listing.
//				if (added)
//				{
//					sortSinglePassList.push_back(pRO);
//				}
//			}
//		} //
//	}
//	// Perform the sorting
//	if ( renderMode == kRenderModeDeferred )
//		std::sort( sortedRenderList.begin(), sortedRenderList.end(), OrderComparatorDeferred ); 
//	else
//		throw std::exception( "Inside RenderSceneDeferred() when not in Deferred-2!" );
//	TimeProfiler.EndAddTimeProfile( "rs_render_makelist" );
//
//	// Calculate pass globals
//	renderer::Settings.fogScale = 1.0f / (renderer::Settings.fogEnd - renderer::Settings.fogStart);//currentCamera->zFar
//																								   // Update UBOs
//	RrMaterial::updateStaticUBO();
//	RrMaterial::updateLightTBO();
//	RrMaterial::pushConstantsPerPass();
//	RrMaterial::pushConstantsPerFrame();
//	GL.CheckError();
//
//	int sortedListSize = sortedRenderList.size();
//
//	// First work on prerender
//	TimeProfiler.BeginTimeProfile( "rs_render_pre" );
//	for ( i = 0; i < (int)sortSinglePassList.size(); ++i )
//	{
//		sortSinglePassList[i]->PreRender();
//	}
//	TimeProfiler.EndAddTimeProfile( "rs_render_pre" );
//	GL.CheckError();
//
//	// Then work on the actual render:
//
//	tRenderRequest	renderRQ_current;		// Current object rendering.
//	tRenderRequest	renderRQ_next;			// Next object rendering. Used to check for changing render states
//	bool rendered = false;					// Holds if an object has been rendered in the current stage.
//
//											// Get the current main buffer to render to, as well as the size.
//	RrRenderTexture* render_target = currentCamera->GetRenderTexture();
//	if ( render_target == NULL )
//	{	// If not a render-camera, bind to the internal common buffer
//		render_target = internal_chain_current->buffer_forward_rt;
//	}
//
//	// Get the size of the viewport we should render at
//	Vector2i unscaledRenderSize;
//	Vector2i currentRenderSize;
//	if ( render_target != NULL )
//	{	
//		// Scaled target only enabled when screen copy enabled
//		unscaledRenderSize = render_target->GetSize();
//		currentRenderSize = Vector2i(
//			(int)(render_target->GetWidth()  * currentCamera->render_scale),
//			(int)(render_target->GetHeight() * currentCamera->render_scale)
//		);
//	}
//	else
//	{
//		unscaledRenderSize = internal_chain_current->buffer_deferred_rt->GetSize();
//		currentRenderSize = Vector2i(
//			(int)(internal_chain_current->buffer_deferred_rt->GetWidth()  * currentCamera->render_scale),
//			(int)(internal_chain_current->buffer_deferred_rt->GetHeight() * currentCamera->render_scale)
//		);
//	}
//	// Set up viewport
//	GL.setupViewport( 0, 0, currentRenderSize.x, currentRenderSize.y );
//
//	// Bind the MRT to render to
//	internal_chain_current->buffer_deferred_mrt->BindBuffer();
//
//	i = -1;	// Starts at -1 in case first object requires forward renderer
//	while ( i < sortedListSize )
//	{
//		// Grab the next two render objects
//		if ( i < 0 )
//		{
//			// First object can never be forward - so we make a fake deferred object:
//			renderRQ_current.obj = NULL;
//			renderRQ_current.renderLayer = kRenderLayer_BEGIN;
//			renderRQ_current.forward = false;
//			renderRQ_current.screenshader = false;
//			renderRQ_current.pass = 0;
//		}
//		else
//		{
//			renderRQ_current = sortedRenderList[i];
//		}
//		if ( i < sortedListSize-1 )
//		{
//			renderRQ_next = sortedRenderList[i+1];
//		}
//		else
//		{
//			renderRQ_next.obj = NULL;
//		}
//
//		// Render the object
//		if ( !renderRQ_current.forward && renderRQ_current.obj && renderRQ_current.obj->visible )
//		{
//			if ( renderRQ_current.transparent )
//			{	// There should be no transparent objects in the deferred pass.
//				throw std::exception();
//			}
//			else
//			{
//				//GL.prepareDraw();
//				if ( !renderRQ_current.obj->Render( renderRQ_current.pass ) )
//				{
//					throw std::exception();
//				}
//				GL.CheckError();
//				//GL.cleanupDraw();
//				rendered = true;
//			}
//		}
//		else if ( renderRQ_current.forward )
//		{	// There should be no forward objects in the deferred pass.
//			throw std::exception();
//		}
//
//		// Check for layer state changes:
//
//		// Check if should perform deferred pass on everything that was currently rendered:
//		if ( ( i >= 0 ) && ( rendered ) && ( renderRQ_current.renderLayer == kRLWorld ) && ( 
//			// Render out if this will be the last deferred object period:
//			( renderRQ_next.obj == NULL ) ||
//			// Render out if this is last deferred object on this list or layer:
//			( !renderRQ_current.forward && (renderRQ_next.forward || renderRQ_current.renderLayer != renderRQ_next.renderLayer) ) 
//			))
//		{
//			TimeProfiler.BeginTimeProfile( "rs_render_lightpush" );
//
//			// Bind the MRT's temporary results area in order to render the deferred result to it
//			internal_chain_current->buffer_deferred_rt->BindBuffer();
//			// Set up viewport. Deferred renderer runs need to hit the entire buffer, so we give it the entire screen.
//			GL.setupViewport( 0, 0, unscaledRenderSize.x, unscaledRenderSize.y );
//
//			// Choose lighting pass to use
//			RrMaterial* targetPass = LightingPass;
//			switch ( pipelineMode )
//			{
//			case renderer::kPipelineModeEcho:
//				targetPass = EchoPass;
//				break;
//			case renderer::kPipelineModeShaft:
//				targetPass = ShaftPass;
//				break;
//			case renderer::kPipelineMode2DPaletted:
//				targetPass = Lighting2DPass;
//				break;
//			}
//
//			// Change the projection to identity-type 2D (similar to orthographic)
//			{
//				// Perform a lighting pass
//				targetPass->setSampler( TEX_SLOT0, internal_chain_current->buffer_deferred_mrt->GetBufferTexture(0), GL.Enum(Texture2D) );
//				targetPass->setSampler( TEX_SLOT1, internal_chain_current->buffer_deferred_mrt->GetBufferTexture(1), GL.Enum(Texture2D) );
//				targetPass->setSampler( TEX_SLOT2, internal_chain_current->buffer_deferred_mrt->GetBufferTexture(2), GL.Enum(Texture2D) );
//				targetPass->setSampler( TEX_SLOT3, internal_chain_current->buffer_deferred_mrt->GetBufferTexture(3), GL.Enum(Texture2D) );
//				targetPass->setSampler( TEX_SLOT4, internal_chain_current->buffer_deferred_mrt->GetDepthSampler(), GL.Enum(Texture2D) );
//				//targetPass->prepareShaderConstants();
//				targetPass->bindPassForward(0);
//
//				// TODO: Make the following configurable. (This can likely be used for some of the game-driving of later effects).
//				//glUniform4f(renderer::UNI_LIGHTING_PARAM_HACK, 0.5F, 1.0F, 1.0F, 1.0F); // luvppl
//				glUniform4f(renderer::UNI_LIGHTING_PARAM_HACK, 1.0F, 1.0F, 0.0F, 1.0F); // otherwise.
//
//																						// Disable alpha blending
//				glDisable( GL_BLEND );
//				glBlendFunc( GL_ONE, GL_ZERO );
//				// Disable writing to depth and stencil
//				glStencilMask( GL_FALSE );
//				glDepthMask( GL_FALSE );
//				// Disable tests
//				glDisable( GL_STENCIL_TEST );
//				glDisable( GL_DEPTH_TEST );
//				// Draw the screen quad
//				GLd.DrawScreenQuad(targetPass);
//			}
//			// Finish rendering
//
//			// Pop the main buffer off the render stack, returning to the MRT target
//			internal_chain_current->buffer_deferred_rt->UnbindBuffer();
//
//			// Clear the MRT's color results now that we no longer need them
//			glClear( GL_COLOR_BUFFER_BIT );
//
//			TimeProfiler.EndAddTimeProfile( "rs_render_lightpush" );
//		}
//
//		// Check if should temporarily switch to forward mode
//		if ( renderRQ_next.obj && renderRQ_current.forward == false && renderRQ_next.forward == true ) // Last deferred object, switch to forward mode
//		{
//			rendered = false;
//
//			// Copy over the depth to use when drawing in forward mode
//			internal_chain_current->buffer_deferred_rt->BindBuffer();
//			// Set up viewport. Forward geometry pass of the renderer again renders at the same size as the deferred geometry pass.
//			GL.setupViewport( 0, 0, currentRenderSize.x, currentRenderSize.y );
//
//			// The forward and deferred buffers share their depth and stencil, so we can continue render without copying anything:
//
//			// Set to forward mode
//			renderMode = kRenderModeForward;
//			bool renderForward = true;
//			while ( renderForward )
//			{
//				i += 1;
//				// Grab the next two render objects
//				renderRQ_current = sortedRenderList[i];
//				if ( i < sortedListSize-1 ) {
//					renderRQ_next = sortedRenderList[i+1];
//				}
//				else {
//					renderRQ_next.obj = NULL;
//				}
//				// Render the object
//				if ( renderRQ_current.obj->visible && renderRQ_current.forward )
//					/*( (RrMaterial::special_mode == renderer::kPipelineModeNormal || RrMaterial::special_mode == renderer::kPipelineModeShaft || RrMaterial::special_mode == renderer::kPipelineMode2DPaletted) && renderRQ_current.forward )
//					||( RrMaterial::special_mode == renderer::kPipelineModeEcho && renderRQ_current.forward  && renderRQ_current.renderLayer == V2D ) // (unless echopass, then only do v2d)
//					))*/
//				{
//					if ( !renderRQ_current.obj->Render( renderRQ_current.pass ) ) {
//						throw std::exception();
//					}
//					GL.CheckError();
//				}
//				else if ( !renderRQ_current.forward ) 
//				{	// Shouldn't be rendering a deferred object here.
//					throw std::exception();
//				}
//				// Check the next object to see if should keep rendering
//				if ( renderRQ_next.obj == NULL || // No next object
//					!renderRQ_next.forward ) // Next object is not forward
//				{	// Stop rendering forward mode
//					renderForward = false;
//				}
//				// If next object is on another layer, clear depth
//				else if ( renderRQ_current.renderLayer != renderRQ_next.renderLayer )
//				{
//					// Clear depth between layers
//					glStencilMask( GL_TRUE );
//					glDepthMask( GL_TRUE );
//					glClear( GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
//				}
//			}
//
//			// Switch back to deferred renderer
//			renderMode = kRenderModeDeferred;
//
//			// Go back to deferred buffer
//			internal_chain_current->buffer_deferred_rt->UnbindBuffer();
//		}
//
//		// Check for main layer switch (rendering is ending, or new layer)
//		if ( renderRQ_next.obj == NULL || renderRQ_current.renderLayer != renderRQ_next.renderLayer )
//		{
//			// Clear data between layers
//			glStencilMask( GL_TRUE );
//			glDepthMask( GL_TRUE );
//			glClear( GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
//
//			// Copy the layer over alpha-blended
//			render_target->BindBuffer();
//			{
//				// Set up the viewport to copy to the entire screen:
//				GL.setupViewport(0, 0, render_target->GetWidth(), render_target->GetHeight());
//				{
//					// Set the current main screen buffer as the texture
//					CopyScaled->setTexture( TEX_MAIN, internal_chain_current->buffer_deferred_rt );
//					CopyScaled->m_diffuse[0] = currentRenderSize.x / (Real)unscaledRenderSize.x;
//					CopyScaled->m_diffuse[1] = currentRenderSize.y / (Real)unscaledRenderSize.y;
//					CopyScaled->prepareShaderConstants();
//					CopyScaled->bindPassForward(0);
//
//					// Set up an always-rendered
//					glStencilMask( GL_FALSE );
//					glDepthMask( GL_FALSE );
//
//					glDisable( GL_STENCIL_TEST );
//					glDisable( GL_DEPTH_TEST );
//
//					glEnable( GL_BLEND );
//					glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
//
//					GLd.DrawScreenQuad(CopyScaled);
//				}
//			}
//			render_target->UnbindBuffer();
//
//			// Clear out the buffer now that we no longer need it.
//			internal_chain_current->buffer_deferred_rt->BindBuffer();
//			{
//				glStencilMask( GL_TRUE );
//				glDepthMask( GL_TRUE );
//				glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
//			}
//			internal_chain_current->buffer_deferred_rt->UnbindBuffer();
//
//			// Reset render size in subsequent targets
//			//if ( renderRQ_next.renderLayer > kRLWorld )
//			//{
//			//	// Reset render scale-down after the main layer
//			//	currentRenderSize = unscaledRenderSize;
//			//} // TODO: May want this to be configurable.
//
//			// Reset the viewport for the next deferred geometry stage
//			GL.setupViewport( 0, 0, currentRenderSize.x, currentRenderSize.y );
//		}
//
//		// Go to the next job
//		i += 1;
//	}
//
//	// Set up the viewport to copy to the entire screen:
//	GL.setupViewport(0, 0, render_target->GetWidth(), render_target->GetHeight());
//
//	// Finally, work on postrender
//	for ( i = 0; i < (int)sortSinglePassList.size(); ++i )
//	{
//		sortSinglePassList[i]->PostRender();
//	}
//	// End render loop
//
//	// Unbind fbo when done rendering
//	internal_chain_current->buffer_deferred_mrt->UnbindBuffer();
//}



//// Deferred rendering routine.
//void RrRenderer::RenderSceneDeferred ( const uint32_t n_renderHint )
//{
//	GL_ACCESS GLd_ACCESS
//	RrRenderObject * pRO;
//	int i;
//	unsigned char passCount;
//	RrCamera* currentCamera = RrCamera::activeCamera;
//	bool firstRender = true;
//
//	// Create a sorted render list:
//	TimeProfiler.BeginTimeProfile( "rs_render_makelist" );
//	// Build the unsorted data
//	std::vector<tRenderRequest> sortedRenderList;
//	std::vector<RrRenderObject*> sortSinglePassList;
//	for ( i = 0; i < (int)iCurrentIndex; i += 1 )
//	{
//		pRO = pRenderableObjects[i];
//		if ( pRO )
//		{
//#ifdef SKIP_NON_WORLD_STUFF
//			// Only add 2D or world objects for now
//			if ( pRO->renderLayer != renderer::kRLWorld && pRO->renderLayer != renderer::kRLV2D ) { 
//				continue;
//			}
//#endif
//			// Only add to list if visible in this hint.
//			if ( pRO->renderSettings.renderHints & n_renderHint )
//			{
//				bool added = false;
//
//				// Add each pass to the render list
//				passCount = pRO->GetPassNumber();
//				for ( unsigned char p = 0; p < passCount; ++p )
//				{
//					RrPassDeferred* pass = pRO->GetPassDeferred(p);
//					// Mask the render hint (multipass across multiple targets)
//					//if ( pass->m_hint & currentLayer )
//					{
//						tRenderRequest newRequest;
//						newRequest.obj  = pRO;
//						newRequest.pass = p;
//						newRequest.forward = false;
//						newRequest.renderLayer = pRO->renderLayer;
//						newRequest.transparent = false;
//						newRequest.screenshader = false;
//						sortedRenderList.push_back( newRequest );
//						added = true;
//					}
//				}
//				if ( passCount == 0 ) // No deferred passes? Must be a world object.
//				{
//					if ( pRO->GetMaterial() && pRO->GetMaterial()->passinfo.size() )
//					{
//						RrPassForward* pass = pRO->GetPass(0);
//						if ( pass->m_hint & kRenderHintWorld )
//						{
//							// Check for depth writing
//							bool depthmask = (pass->m_transparency_mode!=ALPHAMODE_TRANSLUCENT)&&(pass->b_depthmask);
//
//							tRenderRequest newRequest;
//							newRequest.obj  = pRO;
//							newRequest.pass = 0;
//							newRequest.forward = true;
//							newRequest.renderLayer = pRO->renderLayer;
//							newRequest.transparent = !depthmask;
//							newRequest.screenshader = pRO->m_material->m_isScreenShader;
//							sortedRenderList.push_back( newRequest );
//							added = true;
//						}
//					}
//				}
//
//				// Add the single-pass object to the listing.
//				if (added)
//				{
//					sortSinglePassList.push_back(pRO);
//				}
//			}
//		} //
//	}
//	// Perform the sorting
//	if ( renderMode == kRenderModeDeferred )
//		std::sort( sortedRenderList.begin(), sortedRenderList.end(), OrderComparatorDeferred ); 
//	else
//		throw std::exception( "Inside RenderSceneDeferred() when not in Deferred-2!" );
//	TimeProfiler.EndAddTimeProfile( "rs_render_makelist" );
//
//	// Calculate pass globals
//	renderer::Settings.fogScale = 1.0f / (renderer::Settings.fogEnd - renderer::Settings.fogStart);//currentCamera->zFar
//	// Update UBOs
//	RrMaterial::updateStaticUBO();
//	RrMaterial::updateLightTBO();
//	RrMaterial::pushConstantsPerPass();
//	RrMaterial::pushConstantsPerFrame();
//	GL.CheckError();
//
//	int sortedListSize = sortedRenderList.size();
//
//	// First work on prerender
//	TimeProfiler.BeginTimeProfile( "rs_render_pre" );
//	for ( i = 0; i < (int)sortSinglePassList.size(); ++i )
//	{
//		sortSinglePassList[i]->PreRender();
//	}
//	TimeProfiler.EndAddTimeProfile( "rs_render_pre" );
//	GL.CheckError();
//
//	// Then work on the actual render:
//
//	tRenderRequest	renderRQ_current;		// Current object rendering.
//	tRenderRequest	renderRQ_next;			// Next object rendering. Used to check for changing render states
//	bool rendered = false;					// Holds if an object has been rendered in the current stage.
//
//	// Get the current main buffer to render to, as well as the size.
//	RrRenderTexture* render_target = currentCamera->GetRenderTexture();
//	if ( render_target == NULL )
//	{	// If not a render-camera, bind to the internal common buffer
//		render_target = internal_chain_current->buffer_forward_rt;
//	}
//
//	// Get the size of the viewport we should render at
//	Vector2i unscaledRenderSize;
//	Vector2i currentRenderSize;
//	if ( render_target != NULL )
//	{	
//		// Scaled target only enabled when screen copy enabled
//		unscaledRenderSize = render_target->GetSize();
//		currentRenderSize = Vector2i(
//			(int)(render_target->GetWidth()  * currentCamera->render_scale),
//			(int)(render_target->GetHeight() * currentCamera->render_scale)
//		);
//	}
//	else
//	{
//		unscaledRenderSize = internal_chain_current->buffer_deferred_rt->GetSize();
//		currentRenderSize = Vector2i(
//			(int)(internal_chain_current->buffer_deferred_rt->GetWidth()  * currentCamera->render_scale),
//			(int)(internal_chain_current->buffer_deferred_rt->GetHeight() * currentCamera->render_scale)
//		);
//	}
//	// Set up viewport
//	GL.setupViewport( 0, 0, currentRenderSize.x, currentRenderSize.y );
//
//	// Bind the MRT to render to
//	internal_chain_current->buffer_deferred_mrt->BindBuffer();
//	
//	i = -1;	// Starts at -1 in case first object requires forward renderer
//	while ( i < sortedListSize )
//	{
//		// Grab the next two render objects
//		if ( i < 0 )
//		{
//			// First object can never be forward - so we make a fake deferred object:
//			renderRQ_current.obj = NULL;
//			renderRQ_current.renderLayer = kRenderLayer_BEGIN;
//			renderRQ_current.forward = false;
//			renderRQ_current.screenshader = false;
//			renderRQ_current.pass = 0;
//		}
//		else
//		{
//			renderRQ_current = sortedRenderList[i];
//		}
//		if ( i < sortedListSize-1 )
//		{
//			renderRQ_next = sortedRenderList[i+1];
//		}
//		else
//		{
//			renderRQ_next.obj = NULL;
//		}
//
//		// Render the object
//		if ( !renderRQ_current.forward && renderRQ_current.obj && renderRQ_current.obj->visible )
//		{
//			if ( renderRQ_current.transparent )
//			{	// There should be no transparent objects in the deferred pass.
//				throw std::exception();
//			}
//			else
//			{
//				//GL.prepareDraw();
//				if ( !renderRQ_current.obj->Render( renderRQ_current.pass ) )
//				{
//					throw std::exception();
//				}
//				GL.CheckError();
//				//GL.cleanupDraw();
//				rendered = true;
//			}
//		}
//		else if ( renderRQ_current.forward )
//		{	// There should be no forward objects in the deferred pass.
//			throw std::exception();
//		}
//
//		// Check for layer state changes:
//		
//		// Check if should perform deferred pass on everything that was currently rendered:
//		if ( ( i >= 0 ) && ( rendered ) && ( renderRQ_current.renderLayer == kRLWorld ) && ( 
//			// Render out if this will be the last deferred object period:
//			  ( renderRQ_next.obj == NULL ) ||
//			// Render out if this is last deferred object on this list or layer:
//			  ( !renderRQ_current.forward && (renderRQ_next.forward || renderRQ_current.renderLayer != renderRQ_next.renderLayer) ) 
//			))
//		{
//			TimeProfiler.BeginTimeProfile( "rs_render_lightpush" );
//
//			// Bind the MRT's temporary results area in order to render the deferred result to it
//			internal_chain_current->buffer_deferred_rt->BindBuffer();
//			// Set up viewport. Deferred renderer runs need to hit the entire buffer, so we give it the entire screen.
//			GL.setupViewport( 0, 0, unscaledRenderSize.x, unscaledRenderSize.y );
//
//			// Choose lighting pass to use
//			RrMaterial* targetPass = LightingPass;
//			switch ( pipelineMode )
//			{
//			case renderer::kPipelineModeEcho:
//				targetPass = EchoPass;
//				break;
//			case renderer::kPipelineModeShaft:
//				targetPass = ShaftPass;
//				break;
//			case renderer::kPipelineMode2DPaletted:
//				targetPass = Lighting2DPass;
//				break;
//			}
//
//			// Change the projection to identity-type 2D (similar to orthographic)
//			{
//				// Perform a lighting pass
//				targetPass->setSampler( TEX_SLOT0, internal_chain_current->buffer_deferred_mrt->GetBufferTexture(0), GL.Enum(Texture2D) );
//				targetPass->setSampler( TEX_SLOT1, internal_chain_current->buffer_deferred_mrt->GetBufferTexture(1), GL.Enum(Texture2D) );
//				targetPass->setSampler( TEX_SLOT2, internal_chain_current->buffer_deferred_mrt->GetBufferTexture(2), GL.Enum(Texture2D) );
//				targetPass->setSampler( TEX_SLOT3, internal_chain_current->buffer_deferred_mrt->GetBufferTexture(3), GL.Enum(Texture2D) );
//				targetPass->setSampler( TEX_SLOT4, internal_chain_current->buffer_deferred_mrt->GetDepthSampler(), GL.Enum(Texture2D) );
//				//targetPass->prepareShaderConstants();
//				targetPass->bindPassForward(0);
//
//				// TODO: Make the following configurable. (This can likely be used for some of the game-driving of later effects).
//				//glUniform4f(renderer::UNI_LIGHTING_PARAM_HACK, 0.5F, 1.0F, 1.0F, 1.0F); // luvppl
//				glUniform4f(renderer::UNI_LIGHTING_PARAM_HACK, 1.0F, 1.0F, 0.0F, 1.0F); // otherwise.
//
//				// Disable alpha blending
//				glDisable( GL_BLEND );
//				glBlendFunc( GL_ONE, GL_ZERO );
//				// Disable writing to depth and stencil
//				glStencilMask( GL_FALSE );
//				glDepthMask( GL_FALSE );
//				// Disable tests
//				glDisable( GL_STENCIL_TEST );
//				glDisable( GL_DEPTH_TEST );
//				// Draw the screen quad
//				GLd.DrawScreenQuad(targetPass);
//			}
//			// Finish rendering
//
//			// Pop the main buffer off the render stack, returning to the MRT target
//			internal_chain_current->buffer_deferred_rt->UnbindBuffer();
//
//			// Clear the MRT's color results now that we no longer need them
//			glClear( GL_COLOR_BUFFER_BIT );
//
//			TimeProfiler.EndAddTimeProfile( "rs_render_lightpush" );
//		}
//
//		// Check if should temporarily switch to forward mode
//		if ( renderRQ_next.obj && renderRQ_current.forward == false && renderRQ_next.forward == true ) // Last deferred object, switch to forward mode
//		{
//			rendered = false;
//
//			// Copy over the depth to use when drawing in forward mode
//			internal_chain_current->buffer_deferred_rt->BindBuffer();
//			// Set up viewport. Forward geometry pass of the renderer again renders at the same size as the deferred geometry pass.
//			GL.setupViewport( 0, 0, currentRenderSize.x, currentRenderSize.y );
//
//			// The forward and deferred buffers share their depth and stencil, so we can continue render without copying anything:
//
//			// Set to forward mode
//			renderMode = kRenderModeForward;
//			bool renderForward = true;
//			while ( renderForward )
//			{
//				i += 1;
//				// Grab the next two render objects
//				renderRQ_current = sortedRenderList[i];
//				if ( i < sortedListSize-1 ) {
//					renderRQ_next = sortedRenderList[i+1];
//				}
//				else {
//					renderRQ_next.obj = NULL;
//				}
//				// Render the object
//				if ( renderRQ_current.obj->visible && renderRQ_current.forward )
//					/*( (RrMaterial::special_mode == renderer::kPipelineModeNormal || RrMaterial::special_mode == renderer::kPipelineModeShaft || RrMaterial::special_mode == renderer::kPipelineMode2DPaletted) && renderRQ_current.forward )
//					||( RrMaterial::special_mode == renderer::kPipelineModeEcho && renderRQ_current.forward  && renderRQ_current.renderLayer == V2D ) // (unless echopass, then only do v2d)
//					))*/
//				{
//					if ( !renderRQ_current.obj->Render( renderRQ_current.pass ) ) {
//						throw std::exception();
//					}
//					GL.CheckError();
//				}
//				else if ( !renderRQ_current.forward ) 
//				{	// Shouldn't be rendering a deferred object here.
//					throw std::exception();
//				}
//				// Check the next object to see if should keep rendering
//				if ( renderRQ_next.obj == NULL || // No next object
//					 !renderRQ_next.forward ) // Next object is not forward
//				{	// Stop rendering forward mode
//					renderForward = false;
//				}
//				// If next object is on another layer, clear depth
//				else if ( renderRQ_current.renderLayer != renderRQ_next.renderLayer )
//				{
//					// Clear depth between layers
//					glStencilMask( GL_TRUE );
//					glDepthMask( GL_TRUE );
//					glClear( GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
//				}
//			}
//
//			// Switch back to deferred renderer
//			renderMode = kRenderModeDeferred;
//
//			// Go back to deferred buffer
//			internal_chain_current->buffer_deferred_rt->UnbindBuffer();
//		}
//
//		// Check for main layer switch (rendering is ending, or new layer)
//		if ( renderRQ_next.obj == NULL || renderRQ_current.renderLayer != renderRQ_next.renderLayer )
//		{
//			// Clear data between layers
//			glStencilMask( GL_TRUE );
//			glDepthMask( GL_TRUE );
//			glClear( GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
//
//			// Copy the layer over alpha-blended
//			render_target->BindBuffer();
//			{
//				// Set up the viewport to copy to the entire screen:
//				GL.setupViewport(0, 0, render_target->GetWidth(), render_target->GetHeight());
//				{
//					// Set the current main screen buffer as the texture
//					CopyScaled->setTexture( TEX_MAIN, internal_chain_current->buffer_deferred_rt );
//					CopyScaled->m_diffuse[0] = currentRenderSize.x / (Real)unscaledRenderSize.x;
//					CopyScaled->m_diffuse[1] = currentRenderSize.y / (Real)unscaledRenderSize.y;
//					CopyScaled->prepareShaderConstants();
//					CopyScaled->bindPassForward(0);
//
//					// Set up an always-rendered
//					glStencilMask( GL_FALSE );
//					glDepthMask( GL_FALSE );
//
//					glDisable( GL_STENCIL_TEST );
//					glDisable( GL_DEPTH_TEST );
//
//					glEnable( GL_BLEND );
//					glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
//
//					GLd.DrawScreenQuad(CopyScaled);
//				}
//			}
//			render_target->UnbindBuffer();
//
//			// Clear out the buffer now that we no longer need it.
//			internal_chain_current->buffer_deferred_rt->BindBuffer();
//			{
//				glStencilMask( GL_TRUE );
//				glDepthMask( GL_TRUE );
//				glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
//			}
//			internal_chain_current->buffer_deferred_rt->UnbindBuffer();
//
//			// Reset render size in subsequent targets
//			//if ( renderRQ_next.renderLayer > kRLWorld )
//			//{
//			//	// Reset render scale-down after the main layer
//			//	currentRenderSize = unscaledRenderSize;
//			//} // TODO: May want this to be configurable.
//
//			// Reset the viewport for the next deferred geometry stage
//			GL.setupViewport( 0, 0, currentRenderSize.x, currentRenderSize.y );
//		}
//
//		// Go to the next job
//		i += 1;
//	}
//
//	// Set up the viewport to copy to the entire screen:
//	GL.setupViewport(0, 0, render_target->GetWidth(), render_target->GetHeight());
//
//	// Finally, work on postrender
//	for ( i = 0; i < (int)sortSinglePassList.size(); ++i )
//	{
//		sortSinglePassList[i]->PostRender();
//	}
//	// End render loop
//
//	// Unbind fbo when done rendering
//	internal_chain_current->buffer_deferred_mrt->UnbindBuffer();
//}

//
//
//// Specialized Rendering Routines
//void RrRenderer::PreRenderBeginLighting ( std::vector<RrLight*> & lightsToUse )
//{
//	//vSpecialRender_LightList = lightsToUse;
//	vSpecialRender_LightList = *RrLight::GetLightList();
//	*RrLight::GetLightList() = lightsToUse;
//
//	bSpecialRender_ResetLights = true;
//
//	// Calculate pass globals
//	renderer::Settings.fogScale = 1.0f / (renderer::Settings.fogEnd - renderer::Settings.fogStart);//currentCamera->zFar
//	// Update UBOs
//	RrMaterial::updateStaticUBO();
//}
//// RenderSingleObject renders an object, assuming the projection has been already set up.
//void RrRenderer::RenderSingleObject ( RrRenderObject* objectToRender )
//{
//	GL_ACCESS;
//	char maxPass = objectToRender->GetPassNumber();
//	for ( char pass = 0; pass < maxPass; ++pass )
//	{
//		if ( objectToRender->GetPass( pass )->m_hint & kRenderHintWorld )
//		{
//			//GL.prepareDraw();
//			objectToRender->Render( pass );
//			//GL.cleanupDraw();
//		}
//	}
//
//	if ( bSpecialRender_ResetLights ) {
//		*RrLight::GetLightList() = vSpecialRender_LightList;
//		bSpecialRender_ResetLights = false;
//	}
//}
//// RenderObjectArray() renders a null terminated list of objects, assuming the projection has been already set up.
//void RrRenderer::RenderObjectArray ( RrRenderObject** objectsToRender )
//{
//	GL_ACCESS;
//	int i = 0;
//	while ( objectsToRender[i] != NULL ) {
//		char maxPass = objectsToRender[i]->GetPassNumber();
//		for ( char pass = 0; pass < maxPass; ++pass )
//		{
//			//GL.prepareDraw();
//			objectsToRender[i]->Render( pass );
//			//GL.cleanupDraw();
//		}
//	}
//
//	if ( bSpecialRender_ResetLights ) {
//		*RrLight::GetLightList() = vSpecialRender_LightList;
//		bSpecialRender_ResetLights = false;
//	}
//}