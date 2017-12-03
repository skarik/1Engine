
// Std Libary (render sorting)
#include <algorithm>
using std::sort;

// Includes
#include "CRenderState.h"
#include "renderer/state/Settings.h"
//#include "core/settings/CGameSettings.h"
#include "core/math/Math.h"
#include "core-ext/profiler/CTimeProfiler.h"
#include "core/system/Screen.h"
#include "core-ext/threads/Jobs.h"

#include "renderer/exceptions/exceptions.h"

#include "renderer/camera/CCamera.h"
#include "renderer/light/CLight.h"
#include "renderer/texture/CRenderTexture.h"
#include "renderer/texture/CMRTTexture.h"

#include "renderer/material/RrMaterial.h"
#include "renderer/object/CRenderableObject.h"

#include "renderer/resource/CResourceManager.h"

#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

using namespace renderer;

//===============================================================================================//
// CRenderState: Rendering
//===============================================================================================//

// Called during the window's rendering routine.
void CRenderState::Render ( void )
{
	GL_ACCESS GLd_ACCESS
	unsigned int i;

	// Begin the logic jobs
	Jobs::System::Current::WaitForJobs( Jobs::kJobTypeRenderStep );
	for ( i = 0; i < mLoCurrentIndex; ++i ) {
		if ( mLogicObjects[i] && mLogicObjects[i]->active ) {
			Jobs::System::Current::AddJobRequest( Jobs::kJobTypeRenderStep, &(CLogicObject::PreStep), mLogicObjects[i] );
		}
	}
	// Perform the synchronous logic jobs
	for ( i = 0; i < mLoCurrentIndex; ++i ) {
		if ( mLogicObjects[i] && mLogicObjects[i]->active ) {
			mLogicObjects[i]->PreStepSynchronus();
		}
	}

	// Update the streamed loading system
	if ( mResourceManager ) {
		mResourceManager->RenderUpdate();
	}

	// Call to step all the render jobs
	Jobs::System::Current::WaitForJobs( Jobs::kJobTypeRenderStep );

	// Update the camera system
	for ( i = 0; i < CCamera::vCameraList.size(); ++i )
	{
		CCamera* currentCamera = CCamera::vCameraList[i];
		if ( currentCamera != NULL ) {
			CCamera::vCameraList[i]->LateUpdate();
		}
	}
	// Check for a main camera to work with
	if ( CCamera::activeCamera == NULL ) {
		return; // Don't render if there's no camera to render with...
	}
	mainBufferCamera = CCamera::activeCamera;

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
	GL.BeginFrame();
	TimeProfiler.EndTimeProfile( "rs_present" );
	// Clear Screen And Depth Buffer
	//GL.ClearBuffer();	
	
	// Loop through all cameras (todo: sort render order for each camera)
	TimeProfiler.BeginTimeProfile( "rs_camera_matrix" );
	CCamera* prevActiveCam = CCamera::activeCamera;
	for ( std::vector<CCamera*>::iterator it = CCamera::vCameraList.begin(); it != CCamera::vCameraList.end(); ++it )
	{
		CCamera* currentCamera = *it;
		// Update the camera positions and matrices
		if ( currentCamera->GetRender() ) {
			currentCamera->UpdateMatrix();
		}
	}
	// Double check to make sure the zero camera is the active camera
	if ( CCamera::vCameraList[0] != prevActiveCam )
	{
		std::cout << "Error in rendering: invalid zero index camera! (Seriously, this CAN'T happen)" << std::endl;
		// Meaning, resort the camera list
		for ( auto it = CCamera::vCameraList.begin(); it != CCamera::vCameraList.end(); )
		{
			if ( *it == prevActiveCam ) {
				it = CCamera::vCameraList.erase(it);
				CCamera::vCameraList.insert( CCamera::vCameraList.begin(), prevActiveCam );
			}
			else {
				++it;
			}
		}
		return; // Don't render past here, then.
	}
	TimeProfiler.EndTimeProfile( "rs_camera_matrix" );
	

	// Loop through all cameras:
	TimeProfiler.BeginTimeProfile( "rs_render" );
	for ( std::vector<CCamera*>::reverse_iterator it = CCamera::vCameraList.rbegin(); it != CCamera::vCameraList.rend(); ++it )
	{
		CCamera* currentCamera = *it;

		// Only render with current camera if should be rendering
		if ( currentCamera->GetRender() )
		{
			// Perform sorting now
			for ( i = 0; i < iCurrentIndex; i += 1 )
			{	// Put into it's own loop, since it's the same calculation across all objects
				if ( pRenderableObjects[i] ) { 
					pRenderableObjects[i]->UpdateRenderInfo();
				}
			}

			// Render from camera
			CCamera::activeCamera = currentCamera;
			currentCamera->RenderScene();
		}
	}
	TimeProfiler.EndTimeProfile( "rs_render" );
	
	// Set active camera back to default
	CCamera::activeCamera = prevActiveCam;


	// Are we in buffer mode? (This should always be true)
	TimeProfiler.BeginTimeProfile( "rs_buffer_push" );
	if ( bufferedMode )
	{
		// Render the current result to the screen
		GL.setupViewport(0, 0, internal_chain_current->buffer_forward_rt->GetWidth(), internal_chain_current->buffer_forward_rt->GetHeight());
		//GL.pushModelMatrix( Matrix4x4() );
		{
			// Set the current main screen buffer as the texture
			RrMaterial::Copy->setTexture( TEX_MAIN, internal_chain_current->buffer_forward_rt );
			//RrMaterial::Copy->prepareShaderConstants(NULL);
			RrMaterial::Copy->bindPassForward(0);

			// Set up an always-rendered
			glStencilMask( GL_FALSE );
			glDepthMask( GL_FALSE );
		
			glDisable( GL_STENCIL_TEST );
			glDisable( GL_DEPTH_TEST );

			glDisable( GL_BLEND );
			glBlendFunc( GL_ONE, GL_ZERO );

			GLd.DrawScreenQuad(RrMaterial::Copy);
		}
		//GL.popModelMatrix();

		// Clear out the buffer now that we no longer need it.
		internal_chain_current->buffer_forward_rt->BindBuffer();
		{
			glStencilMask( GL_TRUE );
			glDepthMask( GL_TRUE );
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
		}
		internal_chain_current->buffer_forward_rt->UnbindBuffer();
	}
	TimeProfiler.EndTimeProfile( "rs_buffer_push" );


	// Call end render
	TimeProfiler.BeginTimeProfile( "rs_end_render" );
	for ( i = 0; i < iCurrentIndex; i += 1 )
	{
		if ( pRenderableObjects[i] ) {
			pRenderableObjects[i]->EndRender();
		}
	}
	TimeProfiler.EndTimeProfile( "rs_end_render" );


	// Signal frame ended
	GL.EndFrame();

	// Clear Screen And Depth Buffer
	GL.ClearBuffer();

	// Cycle the draw inputs
	{
		// Go to next input
		internal_chain_index = (internal_chain_index + 1) % internal_chain_list.size();
		// Pull its pointer
		internal_chain_current = &internal_chain_list[internal_chain_index];
	}

	// Begin the post-step render jobs
	for ( i = 0; i < mLoCurrentIndex; ++i ) {
		if ( mLogicObjects[i] && mLogicObjects[i]->active ) {
			Jobs::System::Current::AddJobRequest( Jobs::kJobTypeRenderStep, &(CLogicObject::PostStep), mLogicObjects[i] );
		}
	}
	// Perform the synchronous logic jobs
	for ( i = 0; i < mLoCurrentIndex; ++i ) {
		if ( mLogicObjects[i] && mLogicObjects[i]->active ) {
			mLogicObjects[i]->PostStepSynchronus();
		}
	}

	// Do error check at this point
	if ( RrMaterial::Default->passinfo.size() > 16 ) throw std::exception();
	GL.CheckError();
}

// Normal rendering routine. Called by a camera. Camera passes in its render hint.
// This function will render to the scene targets needed.
void CRenderState::RenderSceneForward ( const uint32_t n_renderHint )
{
	// TODO: FIX ALL THIS

	GL_ACCESS GLd_ACCESS
	// Save current mode
	eRenderMode t_currentRenderMode = renderMode;
	// Override it with forward
	renderMode = kRenderModeForward;

	CRenderableObject * pRO;
	tRenderRequest	renderRQ;
	int i;
	unsigned char passCount;
	CCamera* currentCamera = CCamera::activeCamera;

	// Loop through each hint (reverse mode)
	for ( uint currentLayer = 1<<kRenderHintCOUNT; currentLayer != 0; currentLayer >>= 1 )
	{
		int layer = math::log2( currentLayer );
		// Skip non-drawn hints
		if ( !(currentLayer & n_renderHint) ) {
			continue;
		}
		// Skip non-world if no buffer model
		if ( !bufferedMode ) {
			if ( currentLayer != kRenderHintWorld ) {
				continue;
			}
		}

		// Create a sorted render list.
		std::vector<tRenderRequest> sortedRenderList;
		std::vector<CRenderableObject*> sortSinglePassList;
		for ( i = 0; i < (int)iCurrentIndex; i += 1 )
		{
			pRO = pRenderableObjects[i];
			if ( pRO /*&& pRO->visible*/ )
			{
				// Only add to list if visible in this hint.
				if ( pRO->renderSettings.renderHints & currentLayer )
				{
					bool added = false;
					// Add each pass to the render list
					passCount = pRO->GetPassNumber();
					for ( unsigned char p = 0; p < passCount; ++p )
					{
						RrPassForward* pass = pRO->GetPass(p);
						// Mask the render hint (multipass across multiple targets)
						if ( pass->m_hint & currentLayer )
						{
							// Check for depth writing (depth write comes first)
							bool depthmask = (pass->m_transparency_mode!=ALPHAMODE_TRANSLUCENT)&&(pass->b_depthmask);

							tRenderRequest newRequest;
							newRequest.obj  = pRO;
							newRequest.pass = p;	
							newRequest.transparent = !depthmask;
							newRequest.screenshader = pRO->m_material->m_isScreenShader;

							sortedRenderList.push_back( newRequest );
							added = true;
						}
					}
					// Add the single-pass object to the listing.
					if (added)
					{
						sortSinglePassList.push_back(pRO);
					}
				}
			}
		}
		// Sort the render requests
		std::sort( sortedRenderList.begin(), sortedRenderList.end(), OrderComparatorForward ); 

		// If using buffer model
		if ( bufferedMode )
		{
			// For the final camera, render to a buffer
			if ( currentLayer == kRenderHintWorld && currentCamera == mainBufferCamera )
			{
				CRenderTexture* currentMainRenderTarget = GL.GetMainScreenBuffer();
				currentMainRenderTarget->BindBuffer();
				GL.setupViewport(0,0,currentMainRenderTarget->GetWidth(),currentMainRenderTarget->GetHeight());
				GL.CheckError();
			}
			/*else
			{
				// Bind the proper layer buffer
				int layer = Math.log2( currentLayer );
			}*/
		}

		// Calculate pass globals
		renderer::Settings.fogScale = 1.0f / (renderer::Settings.fogEnd - renderer::Settings.fogStart);//currentCamera->zFar
		// Update UBOs
		RrMaterial::updateStaticUBO();

		// Clear with BG color
		glClear( GL_COLOR_BUFFER_BIT ); // use rendereing options for this

		int sortedListSize = sortedRenderList.size();

		// First work on prerender
		for ( i = 0; i < (int)sortSinglePassList.size(); ++i )
		{
			sortSinglePassList[i]->PreRender();
		}

		// Then work on the actual render
		for ( int rt = kRL_BEGIN; rt < kRL_MAX; rt += 1 )
		{
			if ( !currentCamera->layerVisibility[rt] ) { // move to renderer generator
				continue;
			}

			for ( i = 0; i < (int)sortedListSize; ++i )
			{
				renderRQ = sortedRenderList[i];
				pRO = renderRQ.obj;
				//if (( pRO )&&( pRO->renderType == rt ))
				if ( pRO->visible && pRO->renderType == rt )
				{
					//GL.prepareDraw();
					if ( !pRO->Render( renderRQ.pass ) ) {
						throw std::exception();
					}
					GL.CheckError();
					//GL.cleanupDraw();
				}
			}
			// Between layers, clear depth
			glDepthMask( GL_TRUE );
			glClear( GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
		}

		// Finally, work on postrender
		for ( i = 0; i < (int)sortSinglePassList.size(); ++i )
		{
			sortSinglePassList[i]->PostRender();
			GL.CheckError();
		}
		// End render loop

		// Unbind fbo when done rendering
		if ( bufferedMode )
		{
			// For the final camera, render to a buffer
			if ( currentLayer == kRenderHintWorld && currentCamera == mainBufferCamera )
			{
				GL.GetMainScreenBuffer()->UnbindBuffer();
			}
			else
			{
				// Unbind the proper layer buffer
				int layer = math::log2( currentLayer );
			}
		}
	}

	// Restore render mode
	renderMode = t_currentRenderMode;
}

// Some debug compiler flags
//#define SKIP_NON_WORLD_STUFF
#define FORCE_BUFFER_CLEAR
//#define ENABLE_RUNTIME_BLIT_TEST

// Deferred rendering routine.
void CRenderState::RenderSceneDeferred ( const uint32_t n_renderHint )
{
	GL_ACCESS GLd_ACCESS
	CRenderableObject * pRO;
	int i;
	unsigned char passCount;
	CCamera* currentCamera = CCamera::activeCamera;
	bool firstRender = true;

	// Create a sorted render list:
	TimeProfiler.BeginTimeProfile( "rs_render_makelist" );
	// Build the unsorted data
	std::vector<tRenderRequest> sortedRenderList;
	std::vector<CRenderableObject*> sortSinglePassList;
	for ( i = 0; i < (int)iCurrentIndex; i += 1 )
	{
		pRO = pRenderableObjects[i];
		if ( pRO )
		{
#ifdef SKIP_NON_WORLD_STUFF
			// Only add 2D or world objects for now
			if ( pRO->renderType != renderer::kRLWorld && pRO->renderType != renderer::kRLV2D ) { 
				continue;
			}
#endif
			// Only add to list if visible in this hint.
			if ( pRO->renderSettings.renderHints & n_renderHint )
			{
				bool added = false;

				// Add each pass to the render list
				passCount = pRO->GetPassNumber();
				for ( unsigned char p = 0; p < passCount; ++p )
				{
					RrPassDeferred* pass = pRO->GetPassDeferred(p);
					// Mask the render hint (multipass across multiple targets)
					//if ( pass->m_hint & currentLayer )
					{
						tRenderRequest newRequest;
						newRequest.obj  = pRO;
						newRequest.pass = p;
						newRequest.forward = false;
						newRequest.renderType = pRO->renderType;
						newRequest.transparent = false;
						newRequest.screenshader = false;
						sortedRenderList.push_back( newRequest );
						added = true;
					}
				}
				if ( passCount == 0 ) // No deferred passes? Must be a world object.
				{
					if ( pRO->GetMaterial() && pRO->GetMaterial()->passinfo.size() )
					{
						RrPassForward* pass = pRO->GetPass(0);
						if ( pass->m_hint & kRenderHintWorld )
						{
							// Check for depth writing
							bool depthmask = (pass->m_transparency_mode!=ALPHAMODE_TRANSLUCENT)&&(pass->b_depthmask);

							tRenderRequest newRequest;
							newRequest.obj  = pRO;
							newRequest.pass = 0;
							newRequest.forward = true;
							newRequest.renderType = pRO->renderType;
							newRequest.transparent = !depthmask;
							newRequest.screenshader = pRO->m_material->m_isScreenShader;
							sortedRenderList.push_back( newRequest );
							added = true;
						}
					}
				}

				// Add the single-pass object to the listing.
				if (added)
				{
					sortSinglePassList.push_back(pRO);
				}
			}
		} //
	}
	// Perform the sorting
	if ( renderMode == kRenderModeDeferred )
		std::sort( sortedRenderList.begin(), sortedRenderList.end(), OrderComparatorDeferred ); 
	else
		throw std::exception( "Inside RenderSceneDeferred() when not in Deferred-2!" );
	TimeProfiler.EndAddTimeProfile( "rs_render_makelist" );

	// Calculate pass globals
	renderer::Settings.fogScale = 1.0f / (renderer::Settings.fogEnd - renderer::Settings.fogStart);//currentCamera->zFar
	// Update UBOs
	RrMaterial::updateStaticUBO();
	RrMaterial::updateLightTBO();
	RrMaterial::pushConstantsPerPass();
	RrMaterial::pushConstantsPerFrame();
	GL.CheckError();

	int sortedListSize = sortedRenderList.size();

	// First work on prerender
	TimeProfiler.BeginTimeProfile( "rs_render_pre" );
	for ( i = 0; i < (int)sortSinglePassList.size(); ++i )
	{
		sortSinglePassList[i]->PreRender();
	}
	TimeProfiler.EndAddTimeProfile( "rs_render_pre" );
	GL.CheckError();

	// Then work on the actual render:

	tRenderRequest	renderRQ_current;		// Current object rendering.
	tRenderRequest	renderRQ_next;			// Next object rendering. Used to check for changing render states
	bool rendered = false;					// Holds if an object has been rendered in the current stage.

	// Get the current main buffer to render to, as well as the size.
	CRenderTexture* render_target = currentCamera->GetRenderTexture();
	if ( render_target == NULL )
	{	// If not a render-camera, bind to the internal common buffer
		render_target = internal_chain_current->buffer_forward_rt;
	}

	// Get the size of the viewport we should render at
	Vector2i unscaledRenderSize;
	Vector2i currentRenderSize;
	if ( render_target != NULL )
	{	
		// Scaled target only enabled when screen copy enabled
		unscaledRenderSize = render_target->GetSize();
		currentRenderSize = Vector2i(
			(int)(render_target->GetWidth()  * currentCamera->render_scale),
			(int)(render_target->GetHeight() * currentCamera->render_scale)
		);
	}
	else
	{
		unscaledRenderSize = internal_chain_current->buffer_deferred_rt->GetSize();
		currentRenderSize = Vector2i(
			(int)(internal_chain_current->buffer_deferred_rt->GetWidth()  * currentCamera->render_scale),
			(int)(internal_chain_current->buffer_deferred_rt->GetHeight() * currentCamera->render_scale)
		);
	}
	// Set up viewport
	GL.setupViewport( 0, 0, currentRenderSize.x, currentRenderSize.y );

	// Bind the MRT to render to
	internal_chain_current->buffer_deferred_mrt->BindBuffer();
	
	i = -1;	// Starts at -1 in case first object requires forward renderer
	while ( i < sortedListSize )
	{
		// Grab the next two render objects
		if ( i < 0 )
		{
			// First object can never be forward - so we make a fake deferred object:
			renderRQ_current.obj = NULL;
			renderRQ_current.renderType = kRL_BEGIN;
			renderRQ_current.forward = false;
			renderRQ_current.screenshader = false;
			renderRQ_current.pass = 0;
		}
		else
		{
			renderRQ_current = sortedRenderList[i];
		}
		if ( i < sortedListSize-1 )
		{
			renderRQ_next = sortedRenderList[i+1];
		}
		else
		{
			renderRQ_next.obj = NULL;
		}

		// Render the object
		if ( !renderRQ_current.forward && renderRQ_current.obj && renderRQ_current.obj->visible )
		{
			if ( renderRQ_current.transparent )
			{	// There should be no transparent objects in the deferred pass.
				throw std::exception();
			}
			else
			{
				//GL.prepareDraw();
				if ( !renderRQ_current.obj->Render( renderRQ_current.pass ) )
				{
					throw std::exception();
				}
				GL.CheckError();
				//GL.cleanupDraw();
				rendered = true;
			}
		}
		else if ( renderRQ_current.forward )
		{	// There should be no forward objects in the deferred pass.
			throw std::exception();
		}

		// Check for layer state changes:
		
		// Check if should perform deferred pass on everything that was currently rendered:
		if ( ( i >= 0 ) && ( rendered ) && ( renderRQ_current.renderType == kRLWorld ) && ( 
			// Render out if this will be the last deferred object period:
			  ( renderRQ_next.obj == NULL ) ||
			// Render out if this is last deferred object on this list or layer:
			  ( !renderRQ_current.forward && (renderRQ_next.forward || renderRQ_current.renderType != renderRQ_next.renderType) ) 
			))
		{
			TimeProfiler.BeginTimeProfile( "rs_render_lightpush" );

			// Bind the MRT's temporary results area in order to render the deferred result to it
			internal_chain_current->buffer_deferred_rt->BindBuffer();
			// Set up viewport. Deferred renderer runs need to hit the entire buffer, so we give it the entire screen.
			GL.setupViewport( 0, 0, unscaledRenderSize.x, unscaledRenderSize.y );

			// Choose lighting pass to use
			RrMaterial* targetPass = LightingPass;
			switch ( pipelineMode )
			{
			case renderer::kPipelineModeEcho:
				targetPass = EchoPass;
				break;
			case renderer::kPipelineModeShaft:
				targetPass = ShaftPass;
				break;
			case renderer::kPipelineMode2DPaletted:
				targetPass = Lighting2DPass;
				break;
			}

			// Change the projection to identity-type 2D (similar to orthographic)
			{
				// Perform a lighting pass
				targetPass->setSampler( TEX_SLOT0, internal_chain_current->buffer_deferred_mrt->GetBufferTexture(0), GL.Enum(Texture2D) );
				targetPass->setSampler( TEX_SLOT1, internal_chain_current->buffer_deferred_mrt->GetBufferTexture(1), GL.Enum(Texture2D) );
				targetPass->setSampler( TEX_SLOT2, internal_chain_current->buffer_deferred_mrt->GetBufferTexture(2), GL.Enum(Texture2D) );
				targetPass->setSampler( TEX_SLOT3, internal_chain_current->buffer_deferred_mrt->GetBufferTexture(3), GL.Enum(Texture2D) );
				targetPass->setSampler( TEX_SLOT4, internal_chain_current->buffer_deferred_mrt->GetDepthSampler(), GL.Enum(Texture2D) );
				//targetPass->prepareShaderConstants();
				targetPass->bindPassForward(0);

				// Disable alpha blending
				glDisable( GL_BLEND );
				glBlendFunc( GL_ONE, GL_ZERO );
				// Disable writing to depth and stencil
				glStencilMask( GL_FALSE );
				glDepthMask( GL_FALSE );
				// Disable tests
				glDisable( GL_STENCIL_TEST );
				glDisable( GL_DEPTH_TEST );
				// Draw the screen quad
				GLd.DrawScreenQuad(targetPass);
			}
			// Finish rendering

			// Pop the main buffer off the render stack, returning to the MRT target
			internal_chain_current->buffer_deferred_rt->UnbindBuffer();

			// Clear the MRT's color results now that we no longer need them
			glClear( GL_COLOR_BUFFER_BIT );

			TimeProfiler.EndAddTimeProfile( "rs_render_lightpush" );
		}

		// Check if should temporarily switch to forward mode
		if ( renderRQ_next.obj && renderRQ_current.forward == false && renderRQ_next.forward == true ) // Last deferred object, switch to forward mode
		{
			rendered = false;

			// Copy over the depth to use when drawing in forward mode
			internal_chain_current->buffer_deferred_rt->BindBuffer();
			// Set up viewport. Forward geometry pass of the renderer again renders at the same size as the deferred geometry pass.
			GL.setupViewport( 0, 0, currentRenderSize.x, currentRenderSize.y );

			// The forward and deferred buffers share their depth and stencil, so we can continue render without copying anything:

			// Set to forward mode
			renderMode = kRenderModeForward;
			bool renderForward = true;
			while ( renderForward )
			{
				i += 1;
				// Grab the next two render objects
				renderRQ_current = sortedRenderList[i];
				if ( i < sortedListSize-1 ) {
					renderRQ_next = sortedRenderList[i+1];
				}
				else {
					renderRQ_next.obj = NULL;
				}
				// Render the object
				if ( renderRQ_current.obj->visible && renderRQ_current.forward )
					/*( (RrMaterial::special_mode == renderer::kPipelineModeNormal || RrMaterial::special_mode == renderer::kPipelineModeShaft || RrMaterial::special_mode == renderer::kPipelineMode2DPaletted) && renderRQ_current.forward )
					||( RrMaterial::special_mode == renderer::kPipelineModeEcho && renderRQ_current.forward  && renderRQ_current.renderType == V2D ) // (unless echopass, then only do v2d)
					))*/
				{
					if ( !renderRQ_current.obj->Render( renderRQ_current.pass ) ) {
						throw std::exception();
					}
					GL.CheckError();
				}
				else if ( !renderRQ_current.forward ) 
				{	// Shouldn't be rendering a deferred object here.
					throw std::exception();
				}
				// Check the next object to see if should keep rendering
				if ( renderRQ_next.obj == NULL || // No next object
					 !renderRQ_next.forward ) // Next object is not forward
				{	// Stop rendering forward mode
					renderForward = false;
				}
				// If next object is on another layer, clear depth
				else if ( renderRQ_current.renderType != renderRQ_next.renderType )
				{
					// Clear depth between layers
					glStencilMask( GL_TRUE );
					glDepthMask( GL_TRUE );
					glClear( GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
				}
			}

			// Switch back to deferred renderer
			renderMode = kRenderModeDeferred;

			// Go back to deferred buffer
			internal_chain_current->buffer_deferred_rt->UnbindBuffer();
		}

		// Check for main layer switch (rendering is ending, or new layer)
		if ( renderRQ_next.obj == NULL || renderRQ_current.renderType != renderRQ_next.renderType )
		{
			// Clear data between layers
			glStencilMask( GL_TRUE );
			glDepthMask( GL_TRUE );
			glClear( GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

			// Copy the layer over alpha-blended
			render_target->BindBuffer();
			{
				// Set up the viewport to copy to the entire screen:
				GL.setupViewport(0, 0, render_target->GetWidth(), render_target->GetHeight());
				{
					// Set the current main screen buffer as the texture
					CopyScaled->setTexture( TEX_MAIN, internal_chain_current->buffer_deferred_rt );
					CopyScaled->m_diffuse[0] = currentRenderSize.x / (Real)unscaledRenderSize.x;
					CopyScaled->m_diffuse[1] = currentRenderSize.y / (Real)unscaledRenderSize.y;
					CopyScaled->prepareShaderConstants();
					CopyScaled->bindPassForward(0);

					// Set up an always-rendered
					glStencilMask( GL_FALSE );
					glDepthMask( GL_FALSE );

					glDisable( GL_STENCIL_TEST );
					glDisable( GL_DEPTH_TEST );

					glEnable( GL_BLEND );
					glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

					GLd.DrawScreenQuad(CopyScaled);
				}
			}
			render_target->UnbindBuffer();

			// Clear out the buffer now that we no longer need it.
			internal_chain_current->buffer_deferred_rt->BindBuffer();
			{
				glStencilMask( GL_TRUE );
				glDepthMask( GL_TRUE );
				glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
			}
			internal_chain_current->buffer_deferred_rt->UnbindBuffer();

			// Reset render size in subsequent targets
			//if ( renderRQ_next.renderType > kRLWorld )
			//{
			//	// Reset render scale-down after the main layer
			//	currentRenderSize = unscaledRenderSize;
			//} // TODO: May want this to be configurable.

			// Reset the viewport for the next deferred geometry stage
			GL.setupViewport( 0, 0, currentRenderSize.x, currentRenderSize.y );
		}

		// Go to the next job
		i += 1;
	}

	// Set up the viewport to copy to the entire screen:
	GL.setupViewport(0, 0, render_target->GetWidth(), render_target->GetHeight());

	// Finally, work on postrender
	for ( i = 0; i < (int)sortSinglePassList.size(); ++i )
	{
		sortSinglePassList[i]->PostRender();
	}
	// End render loop

	// Unbind fbo when done rendering
	internal_chain_current->buffer_deferred_mrt->UnbindBuffer();
}



// Specialized Rendering Routines
void CRenderState::PreRenderSetLighting ( std::vector<CLight*> & lightsToUse )
{
	//vSpecialRender_LightList = lightsToUse;
	vSpecialRender_LightList = *CLight::GetLightList();
	*CLight::GetLightList() = lightsToUse;

	bSpecialRender_ResetLights = true;

	// Calculate pass globals
	renderer::Settings.fogScale = 1.0f / (renderer::Settings.fogEnd - renderer::Settings.fogStart);//currentCamera->zFar
	// Update UBOs
	RrMaterial::updateStaticUBO();
}
// RenderSingleObject renders an object, assuming the projection has been already set up.
void CRenderState::RenderSingleObject ( CRenderableObject* objectToRender )
{
	GL_ACCESS;
	char maxPass = objectToRender->GetPassNumber();
	for ( char pass = 0; pass < maxPass; ++pass )
	{
		if ( objectToRender->GetPass( pass )->m_hint & kRenderHintWorld )
		{
			//GL.prepareDraw();
			objectToRender->Render( pass );
			//GL.cleanupDraw();
		}
	}

	if ( bSpecialRender_ResetLights ) {
		*CLight::GetLightList() = vSpecialRender_LightList;
		bSpecialRender_ResetLights = false;
	}
}
// RenderObjectArray() renders a null terminated list of objects, assuming the projection has been already set up.
void CRenderState::RenderObjectArray ( CRenderableObject** objectsToRender )
{
	GL_ACCESS;
	int i = 0;
	while ( objectsToRender[i] != NULL ) {
		char maxPass = objectsToRender[i]->GetPassNumber();
		for ( char pass = 0; pass < maxPass; ++pass )
		{
			//GL.prepareDraw();
			objectsToRender[i]->Render( pass );
			//GL.cleanupDraw();
		}
	}

	if ( bSpecialRender_ResetLights ) {
		*CLight::GetLightList() = vSpecialRender_LightList;
		bSpecialRender_ResetLights = false;
	}
}