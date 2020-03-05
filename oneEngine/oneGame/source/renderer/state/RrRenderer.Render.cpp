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
#include "renderer/object/CRenderableObject.h"

#include "gpuw/Pipeline.h"
#include "gpuw/ShaderPipeline.h"
#include "gpuw/OutputSurface.h"
#include "renderer/material/RrPass.h"
#include "renderer/material/RrShaderProgram.h"
#include "renderer/state/RrPipelinePasses.h"

#include <algorithm>

//#include "renderer/resource/CResourceManager.h"

using namespace renderer;

//===============================================================================================//
// RrRenderer: Rendering
//===============================================================================================//

void RrRenderer::StepPreRender ( void )
{
	unsigned int i;

	// Wait for the PostStep to finish:
	Jobs::System::Current::WaitForJobs( Jobs::kJobTypeRenderStep );

	// Update the camera system for the next frame
	auto l_cameraList = RrCamera::CameraList();
	for ( i = 0; i < l_cameraList.size(); ++i )
	{
		RrCamera* currentCamera = l_cameraList[i];
		if ( currentCamera != NULL )
		{
			currentCamera->LateUpdate();
		}
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
		internal_cbuffers_frames[internal_chain_index].upload(NULL, &frameInfo, sizeof(renderer::cbuffer::rrPerFrame), gpu::kTransferStream);
	}

	// Update the per-pass constant buffer data:
	for (int iLayer = renderer::kRenderLayer_BEGIN; iLayer < renderer::kRenderLayer_MAX; ++iLayer)
	{
		renderer::cbuffer::rrPerPassLightingInfo passInfo = {}; // Unused. May want to remove later...

		internal_cbuffers_passes[internal_chain_index * renderer::kRenderLayer_MAX + iLayer]
			.upload(NULL, &passInfo, sizeof(renderer::cbuffer::rrPerPassLightingInfo), gpu::kTransferStream);
	}

	// Begin the logic jobs
	for ( i = 0; i < mLoCurrentIndex; ++i ) {
		if ( mLogicObjects[i] && mLogicObjects[i]->active ) {
			Jobs::System::Current::AddJobRequest( Jobs::kJobTypeRenderStep, &(RrLogicObject::PreStep), mLogicObjects[i] );
		}
	}
	// Perform the synchronous logic jobs
	for ( i = 0; i < mLoCurrentIndex; ++i ) {
		if ( mLogicObjects[i] && mLogicObjects[i]->active ) {
			mLogicObjects[i]->PreStepSynchronus();
		}
	}

	// Update the streamed loading system
	//if ( mResourceManager ) {
	//	mResourceManager->RenderUpdate();
	//}
	// TODO:
	auto resourceManager = core::ArResourceManager::Active();
	resourceManager->Update();

	// Wait for all the PreStep to finish:
	Jobs::System::Current::WaitForJobs( Jobs::kJobTypeRenderStep );
}

void RrRenderer::StepPostRender ( void )
{
	unsigned int i;

	// Call end render
	TimeProfiler.BeginTimeProfile( "rs_end_render" );
	for ( i = 0; i < iCurrentIndex; i += 1 )
	{
		if ( pRenderableObjects[i] ) {
			pRenderableObjects[i]->EndRender();
		}
	}
	TimeProfiler.EndTimeProfile( "rs_end_render" );

	// Begin the post-step render jobs
	for ( i = 0; i < mLoCurrentIndex; ++i ) {
		if ( mLogicObjects[i] && mLogicObjects[i]->active ) {
			Jobs::System::Current::AddJobRequest( Jobs::kJobTypeRenderStep, &(RrLogicObject::PostStep), mLogicObjects[i] );
		}
	}
	// Perform the synchronous logic jobs
	for ( i = 0; i < mLoCurrentIndex; ++i ) {
		if ( mLogicObjects[i] && mLogicObjects[i]->active ) {
			mLogicObjects[i]->PostStepSynchronus();
		}
	}
}

void RrRenderer::StepBufferPush ( void )
{
	gpu::GraphicsContext* gfx = mGfxContext;

	// Are we in buffer mode? (This should always be true)
	TimeProfiler.BeginTimeProfile( "rs_buffer_push" );
	if ( bufferedMode )
	{
		// Render the current result to the screen
		gfx->setRenderTarget(mOutputSurface->getRenderTarget());
		gfx->setViewport(0, 0, Screen::Info.width, Screen::Info.height);
		{
			float clearColor[] = {1, 1, 1, 0};
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
				                      internal_chain_current->buffer_forward_rt.getAttachment(gpu::kRenderTargetSlotColor0));
			gfx->draw(4, 0);
		}
		gfx->clearPipelineAndWait(); // Wait until we're done using the buffer...

		// Clear the buffer after we're done with it.
		gfx->setRenderTarget(&internal_chain_current->buffer_forward_rt);
		gfx->setViewport(0, 0, Screen::Info.width, Screen::Info.height);
		gfx->clearDepthStencil(true, 1.0F, true, 0x00);
		float clearColor[] = {0, 0, 0, 0};
		gfx->clearColor(clearColor);
	}
	TimeProfiler.EndTimeProfile( "rs_buffer_push" );
}

// Called during the window's rendering routine.
void RrRenderer::Render ( void )
{
	gpu::GraphicsContext* gfx = mGfxContext;
	unsigned int i;

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

	// Update pre-render steps:
	//	- RrLogicObject::PostStep finish
	//	- RrCamera::LateUpdate
	//	- RrLogicObject::PreStep, RrLogicObject::PreStepSynchronous
	//	- ResourceManager::update
	StepPreRender(); 

	// Check for a main camera to work with
	if ( RrCamera::activeCamera == NULL )
	{
		return; // Don't render if there's no camera to render with...
	}
	mainBufferCamera = RrCamera::activeCamera;

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
	
	// Call begin render
	TimeProfiler.BeginTimeProfile( "rs_begin_render" );
	for ( i = 0; i < iCurrentIndex; i += 1 )
	{
		if ( pRenderableObjects[i] ) {
			pRenderableObjects[i]->BeginRender(); //massive slowdown
		}
	}
	TimeProfiler.EndTimeProfile( "rs_begin_render" );

	// Update window buffer
	/*TimeProfiler.BeginTimeProfile( "WD_Swap" );
	SwapBuffers(RrWindow::pActive->getDevicePointer());				// Swap Buffers (Double Buffering) (VSYNC)
	//wglSwapLayerBuffers(aWindow.getDevicePointer(),WGL_SWAP_MAIN_PLANE );
	TimeProfiler.EndTimeProfile( "WD_Swap" );*/

	// Prepare frame
	TimeProfiler.BeginTimeProfile( "rs_present" );
	//GL.BeginFrame();
	gfx->reset();
	TimeProfiler.EndTimeProfile( "rs_present" );
	
	// Loop through all cameras (todo: sort render order for each camera)
	TimeProfiler.BeginTimeProfile( "rs_camera_matrix" );
	RrCamera* prevActiveCam = RrCamera::activeCamera;
	auto l_cameraList = RrCamera::CameraList();
	for ( i = 0; i < l_cameraList.size(); ++i )
	{
		RrCamera* currentCamera = l_cameraList[i];
		// Update the camera positions and matrices
		if ( currentCamera != NULL && currentCamera->GetRender() )
		{
			currentCamera->UpdateMatrix();
		}
	}
	// Double check to make sure the zero camera is the active camera
	if ( l_cameraList[0] != prevActiveCam )
	{
		std::cout << "Error in rendering: invalid zero index camera! (Seriously, this CAN'T happen)" << std::endl;
		// Meaning, resort the camera list
		/*for ( auto it = RrCamera::vCameraList.begin(); it != RrCamera::vCameraList.end(); )
		{
			if ( *it == prevActiveCam )
			{
				it = RrCamera::vCameraList.erase(it);
				RrCamera::vCameraList.insert( RrCamera::vCameraList.begin(), prevActiveCam );
			}
			else
			{
				++it;
			}
		}*/
		return; // Don't render past here, then.
	}
	TimeProfiler.EndTimeProfile( "rs_camera_matrix" );
	
	// Loop through all cameras:
	TimeProfiler.BeginTimeProfile( "rs_render" );
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
			RenderScene(currentCamera);
		}
	}
	TimeProfiler.EndTimeProfile( "rs_render" );
	
	// Set active camera back to default
	RrCamera::activeCamera = prevActiveCam;

	// Push buffer to screen
	StepBufferPush();

	// Signal frame ended
	gfx->submit();

	// Cycle the draw inputs
	{
		// Go to next input
		internal_chain_index = (internal_chain_index + 1) % internal_chain_list.size();
		// Pull its pointer
		internal_chain_current = &internal_chain_list[internal_chain_index];
	}

	// Do error check at this point
	//if ( RrMaterial::Default->passinfo.size() > 16 ) throw std::exception();
	//GL.CheckError();
	gfx->validate();

	// Present the output buffer
	mOutputSurface->present(); // TODO: On Vulkan and other platforms, this will likely need a fence.

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
//	CRenderableObject * pRO;
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
//		std::vector<CRenderableObject*> sortSinglePassList;
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

void RrRenderer::RenderScene ( RrCamera* camera )
{
	RenderObjectList(camera, pRenderableObjects.data(), iCurrentIndex);
}

//	RenderObjectList () : Renders the object list from the given camera with DeferredForward+.
void RrRenderer::RenderObjectList ( RrCamera* camera, CRenderableObject** objectsToRender, const uint32_t objectCount )
{
	// Get a pass list from the camera
	const int kMaxCameraPasses = 8;
	rrCameraPass cameraPasses [kMaxCameraPasses];
	rrCameraPassInput cameraPassInput;
	cameraPassInput.m_maxPasses = kMaxCameraPasses;
	cameraPassInput.m_bufferingCount = (uint16_t)std::min<size_t>(internal_chain_list.size(), 0xFFFF);
	cameraPassInput.m_bufferingIndex = internal_chain_index;

	int passCount = camera->PassCount();
	camera->PassRetrieve(&cameraPassInput, cameraPasses);

	// Begin rendering now
	camera->RenderBegin();

	// Loop through each pass and render with them:
	for (int iCameraPass = 0; iCameraPass < passCount; ++iCameraPass)
	{
		if (cameraPasses[iCameraPass].m_passType == kCameraRenderWorld)
		{
			RenderObjectListWorld(&cameraPasses[iCameraPass], objectsToRender, objectCount);
		}
		else if (cameraPasses[iCameraPass].m_passType == kCameraRenderShadow)
		{
			//RenderObjectListShadows(&cameraPasses[iCameraPass], objectsToRender, objectCount);
			ARCORE_ERROR("Shadow pass not yet implemented.");
		}
	}

	// Finish up the rendering
	camera->RenderEnd();
}

void RrRenderer::RenderObjectListWorld ( rrCameraPass* cameraPass, CRenderableObject** objectsToRender, const uint32_t objectCount )
{
	struct rrRenderRequestGroup
	{
		bool m_enabled;
		std::vector<rrRenderRequest> m_4rDepthPrepass;
		std::vector<rrRenderRequest> m_4rDeferred;
		std::vector<rrRenderRequest> m_4rForward;
		std::vector<rrRenderRequest> m_4rFog;
		std::vector<rrRenderRequest> m_4rWarp;
	};
	rrRenderRequestGroup	l_4rGroup [renderer::kRenderLayer_MAX];
	std::thread				l_4rThread [renderer::kRenderLayer_MAX];

Prerender_Pass:

	// Now we loop through all the objects, check if they have an enabled pass, and call Pre-Render on them.
	for (uint8_t iLayer = renderer::kRenderLayer_BEGIN; iLayer < renderer::kRenderLayer_MAX; ++iLayer)
	{
		for (uint32_t iObject = 0; iObject < objectCount; ++iObject)
		{
			CRenderableObject* renderable = objectsToRender[iObject];
			if (renderable != NULL)
			{
				bool l_hasPass = false;

				// Loop through each pass to place them in the render lists.
				for (uint8_t iPass = 0; iPass < kPass_MaxPassCount; ++iPass)
				{
					if (renderable->m_passEnabled[iPass] && renderable->m_passes[iPass].m_layer == iLayer)
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
		l_4rThread[iLayer] = std::thread([=, &l_4rGroup]()
		{

		Pass_Collection:

			for (uint32_t iObject = 0; iObject < objectCount; ++iObject)
			{
				CRenderableObject* renderable = objectsToRender[iObject];
				if (renderable != NULL)
				{
					bool l_hasPass = false;

					// Loop through each pass to place them in the render lists.
					for (uint8_t iPass = 0; iPass < kPass_MaxPassCount; ++iPass)
					{
						if (renderable->m_passEnabled[iPass] && renderable->m_passes[iPass].m_layer == iLayer)
						{
							RrPass* pass = &renderable->m_passes[iPass];

							// If part of the world...
							if (pass->m_type == kPassTypeForward || pass->m_type == kPassTypeDeferred)
							{
								if (pass->m_depthWrite)
								{	// Add opaque objects to the prepass list:
									l_4rGroup[iLayer].m_4rDepthPrepass.push_back(rrRenderRequest{renderable, iPass});
								}
								// Add the other objects to the deferred or forward pass
								if (pass->m_type == kPassTypeDeferred)
									l_4rGroup[iLayer].m_4rDeferred.push_back(rrRenderRequest{renderable, iPass});
								else
									l_4rGroup[iLayer].m_4rForward.push_back(rrRenderRequest{renderable, iPass});
							}
							// If part of fog effects...
							else if (pass->m_type == kPassTypeVolumeFog)
							{
								l_4rGroup[iLayer].m_4rFog.push_back(rrRenderRequest{renderable, iPass});
							}
							// If part of warp effects...
							else if (pass->m_type == kPassTypeWarp)
							{
								l_4rGroup[iLayer].m_4rWarp.push_back(rrRenderRequest{renderable, iPass});
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

			// immidiately sort the depth pre-pass:
			std::sort(l_4rGroup[iLayer].m_4rDepthPrepass.begin(), l_4rGroup[iLayer].m_4rDepthPrepass.end(), RenderRequestSorter); 

			// sort the rest:
			std::sort(l_4rGroup[iLayer].m_4rDeferred.begin(), l_4rGroup[iLayer].m_4rDeferred.end(), RenderRequestSorter); 
			std::sort(l_4rGroup[iLayer].m_4rForward.begin(), l_4rGroup[iLayer].m_4rForward.end(), RenderRequestSorter); 
			std::sort(l_4rGroup[iLayer].m_4rFog.begin(), l_4rGroup[iLayer].m_4rFog.end(), RenderRequestSorter); 
			std::sort(l_4rGroup[iLayer].m_4rWarp.begin(), l_4rGroup[iLayer].m_4rWarp.end(), RenderRequestSorter); 
		});
	}

Render_Groups:
	gpu::GraphicsContext* gfx = mGfxContext;

	// select buffer chain we work with
	RrHybridBufferChain* bufferChain = cameraPass->m_bufferChain;
	if (bufferChain == NULL)
	{	// Select the main buffer chain if no RT is defined.
		bufferChain = internal_chain_current;
	}
	ARCORE_ASSERT(bufferChain != NULL);

	// target proper buffer
	gfx->setRenderTarget(&bufferChain->buffer_forward_rt); // TODO: Binding buffers at the right time.
	
	// verified: target set properly here

	for (uint8_t iLayer = renderer::kRenderLayer_BEGIN; iLayer < renderer::kRenderLayer_MAX; ++iLayer)
	{
		if (!l_4rThread[iLayer].joinable())
			continue;

		// wait for the sorting of this layer to finish:
		l_4rThread[iLayer].join();

	Rendering:

		int l_cbuffer_pass_index = internal_chain_index * renderer::kRenderLayer_MAX + iLayer;

		bool dirty_deferred = false;
		bool dirty_forward = false;

		// Set up depth & color draw for pre-pass mode. Draw all opaques.
		{
			gpu::DepthStencilState ds;
			ds.depthTestEnabled   = true;
			ds.depthWriteEnabled  = true;
			ds.depthFunc = gpu::kCompareOpLessEqual;
			ds.stencilTestEnabled = false;
			gfx->setDepthStencilState(ds);

			gpu::BlendState bs;
			bs.channelMask = 0x00; // Disable color masking.
			bs.enable = false;
			gfx->setBlendState(bs);
		}
		
		// Clear depth:
		gfx->clearDepthStencil(true, 1.0F, false, 0x00);
		
		// Do depth pre-pass:
		//for (size_t iObject = 0; iObject < l_4rDepthPrepass.size(); ++iObject)
		//{
		// TODO: Fiddle with the rasterization rules so we only write to the depth.
		//const rrRenderRequest const&  l_4r = l_4rDepthPrepass[iObject];
		//CRenderableObject* renderable = l_4r.obj;
		//renderable->Render(l_4r.pass);
		//}

		// Set up depth & color draw for deferred mode. Depth-equal only.
		{
			gpu::DepthStencilState ds;
			ds.depthTestEnabled   = true;
			ds.depthWriteEnabled  = false;
			ds.depthFunc = gpu::kCompareOpEqual;
			ds.stencilTestEnabled = false;
			gfx->setDepthStencilState(ds);

			gpu::BlendState bs;
			bs.channelMask = 0xFF; // Enable color masking.
			bs.enable = false;
			gfx->setBlendState(bs);
		}

		// Do the deferred pass:
		for (size_t iObject = 0; iObject < l_4rGroup[iLayer].m_4rDeferred.size(); ++iObject)
		{
			const rrRenderRequest&  l_4r = l_4rGroup[iLayer].m_4rDeferred[iObject];
			CRenderableObject* renderable = l_4r.obj;

			CRenderableObject::rrRenderParams params;
			params.pass = l_4r.pass;
			params.cbuf_perCamera = cameraPass->m_cbuffer;
			params.cbuf_perFrame = &internal_cbuffers_frames[internal_chain_index];
			params.cbuf_perPass = &internal_cbuffers_passes[l_cbuffer_pass_index];
			
			renderable->Render(&params);
		}
		// check if rendered anything, mark screen dirty if so.
		if (!l_4rGroup[iLayer].m_4rDeferred.empty())
			dirty_deferred = true;

		// run a composite pass if deferred is dirty
		if (dirty_deferred)
		{
			// Mark forward as dirty so output will happen even without a forward object.
			dirty_forward = true;
		}

		// Set up depth & color draw for forward mode. Draw normally.
		{
			gpu::DepthStencilState ds;
			ds.depthTestEnabled   = true;
			ds.depthWriteEnabled  = true;
			ds.depthFunc = gpu::kCompareOpLessEqual;
			ds.stencilTestEnabled = false;
			gfx->setDepthStencilState(ds);

			gpu::BlendState bs;
			bs.channelMask = 0xFF; // Enable color masking.
			bs.enable = false;
			gfx->setBlendState(bs);
		}

		// Do the forward pass:
		for (size_t iObject = 0; iObject < l_4rGroup[iLayer].m_4rForward.size(); ++iObject)
		{
			const rrRenderRequest&  l_4r = l_4rGroup[iLayer].m_4rForward[iObject];
			CRenderableObject* renderable = l_4r.obj;

			CRenderableObject::rrRenderParams params;
			params.pass = l_4r.pass;
			params.cbuf_perCamera = cameraPass->m_cbuffer;
			params.cbuf_perFrame = &internal_cbuffers_frames[internal_chain_index];
			params.cbuf_perPass = &internal_cbuffers_passes[l_cbuffer_pass_index];

			renderable->Render(&params);
		}
		// check if rendered anything, mark screen dirty if so.
		if (!l_4rGroup[iLayer].m_4rForward.empty())
			dirty_forward = true;

		// run another composite if forward is dirty
		if (dirty_forward)
		{
			// TODO
		}
	}

	// at the end, copy the aggregate forward RT onto the render target (do that outside of this function!)

}

//
//// Deferred rendering routine.
//void RrRenderer::RenderScene ( const uint32_t renderHint, RrCamera* camera )
//{
//	GL_ACCESS GLd_ACCESS
//	CRenderableObject * pRO;
//	int i;
//	unsigned char passCount;
//	RrCamera* currentCamera = RrCamera::activeCamera;
//	bool firstRender = true;
//
//	// Create a sorted render list:
//	TimeProfiler.BeginTimeProfile( "rs_render_makelist" );
//	// Build the unsorted data
//	std::vector<tRenderRequest> sortedRenderList;
//	std::vector<CRenderableObject*> sortSinglePassList;
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
//	CRenderableObject * pRO;
//	int i;
//	unsigned char passCount;
//	RrCamera* currentCamera = RrCamera::activeCamera;
//	bool firstRender = true;
//
//	// Create a sorted render list:
//	TimeProfiler.BeginTimeProfile( "rs_render_makelist" );
//	// Build the unsorted data
//	std::vector<tRenderRequest> sortedRenderList;
//	std::vector<CRenderableObject*> sortSinglePassList;
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
//void RrRenderer::RenderSingleObject ( CRenderableObject* objectToRender )
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
//void RrRenderer::RenderObjectArray ( CRenderableObject** objectsToRender )
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