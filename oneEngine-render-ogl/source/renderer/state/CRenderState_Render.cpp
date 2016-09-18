
// Std Libary (render sorting)
#include <algorithm>
using std::sort;

// Includes
#include "CRenderState.h"
#include "renderer/state/Settings.h"
#include "core/settings/CGameSettings.h"
#include "core/math/Math.h"
#include "core-ext/profiler/CTimeProfiler.h"
#include "core/system/Screen.h"
#include "core-ext/threads/Jobs.h"

#include "renderer/exceptions/exceptions.h"

#include "renderer/camera/CCamera.h"
#include "renderer/light/CLight.h"
#include "renderer/texture/CRenderTexture.h"
#include "renderer/texture/CMRTTexture.h"

#include "renderer/material/glMaterial.h"
#include "renderer/object/CRenderableObject.h"

#include "renderer/resource/CResourceManager.h"

#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

using namespace Renderer;

//===============================================================================================//
// CRenderState: Rendering
//===============================================================================================//

// Called during the window's rendering routine.
void CRenderState::Render ( void )
{
	GL_ACCESS GLd_ACCESS
	unsigned int i;

	// Begin the logic jobs
	//Jobs::System::Current::WaitForJobs( Jobs::JOBTYPE_RENDERSTEP );
	for ( i = 0; i < mLoCurrentIndex; ++i ) {
		if ( mLogicObjects[i] && mLogicObjects[i]->active ) {
			Jobs::System::Current::AddJobRequest( Jobs::JOBTYPE_RENDERSTEP, &(CLogicObject::PreStep), mLogicObjects[i] );
		}
	}
	Jobs::System::Current::WaitForJobs( Jobs::JOBTYPE_RENDERSTEP );
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
	//Jobs::System::Current::WaitForJobs( Jobs::JOBTYPE_RENDERSTEP );

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
	SwapBuffers(COglWindow::pActive->getDevicePointer());				// Swap Buffers (Double Buffering) (VSYNC)
	//wglSwapLayerBuffers(aWindow.getDevicePointer(),WGL_SWAP_MAIN_PLANE );
	TimeProfiler.EndTimeProfile( "WD_Swap" );*/

	// Prepare frame
	TimeProfiler.BeginTimeProfile( "rs_present" );
	GL.BeginFrame();
	TimeProfiler.EndTimeProfile( "rs_present" );
	// Clear Screen And Depth Buffer
	GL.ClearBuffer();	
	// Set up main render settings
	//GL.setupAmbient();
	//GL.setupFog();
	
	TimeProfiler.BeginTimeProfile( "rs_camera_matrix" );
	// Loop through all cameras (todo: sort render order for each camera)
	CCamera* prevActiveCam = CCamera::activeCamera;
	for ( std::vector<CCamera*>::iterator it = CCamera::vCameraList.begin(); it != CCamera::vCameraList.end(); ++it )
	{
		CCamera* currentCamera = *it;
		// Update the camera positions and matrices
		if ( currentCamera->GetRender() ) {
			currentCamera->UpdateMatrix();
		}
	}
	// Check for zero camera to render to
	if ( CCamera::vCameraList[0] != prevActiveCam ) {
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
		//CCamera::vCameraList[0]->active = true;
		//throw std::exception( "Bad camera" );
		return; // Don't render past here, then.
	}
	TimeProfiler.EndTimeProfile( "rs_camera_matrix" );
	
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
	// Set active camera back to default
	CCamera::activeCamera = prevActiveCam;
	// End camera loop
	TimeProfiler.EndTimeProfile( "rs_render" );
	
	TimeProfiler.BeginTimeProfile( "rs_buffer_push" );
	// Now, if we're in the buffer mode
	if ( CGameSettings::Active()->b_ro_UseBufferModel )
	{
		GL.setupViewport(0,0,Screen::Info.width,Screen::Info.height);
		// Change the projection to orthographic
		//GL.beginOrtho( 0,1, 1,-1, -1,1, false );
		GL.pushModelMatrix( Matrix4x4() );

		//glMaterial::Copy->setTexture( 0, GL.GetMainScreenBuffer() );
		//static CTexture* dbgTexture = new CTexture("textures/caustics.png");
		/*static CTexture* dbgTexture = NULL;
		if ( dbgTexture == NULL ) {
			dbgTexture = new CTexture("textures/system/cursor.png");
		}*/
		glMaterial::Copy->setTexture( 0, GL.GetMainScreenBuffer() );
		//glMaterial::Copy->setTexture( 0, dbgTexture );
		glMaterial::Copy->bindPassForward(0);

		glDepthMask( false );
		glDepthFunc( GL_ALWAYS );

		glEnable( GL_BLEND );
		//glBlendFunc( GL_ONE, GL_ZERO );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		glDisable( GL_CULL_FACE );

		/*GLd.BeginPrimitive( GL_TRIANGLE_STRIP );
			GLd.P_PushTexcoord( 0,1 );
			GLd.P_AddVertex( -1,1 );
			GLd.P_PushTexcoord( 0,0 );
			GLd.P_AddVertex( -1,-1 );
			GLd.P_PushTexcoord( 1,1 );
			GLd.P_AddVertex( 1,1 );
			GLd.P_PushTexcoord( 1,0 );
			GLd.P_AddVertex( 1,-1 );
		GLd.EndPrimitive();*/
		GLd.DrawScreenQuad();

		glDepthFunc( GL_LEQUAL );

		//GL.endOrtho();
		GL.popModelMatrix();
	}
	TimeProfiler.EndTimeProfile( "rs_buffer_push" );

	// Call present
	GL.Present();	// Swap to the new frame now

	TimeProfiler.BeginTimeProfile( "rs_end_render" );
	// Call end render
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
			Jobs::System::Current::AddJobRequest( Jobs::JOBTYPE_RENDERSTEP, &(CLogicObject::PostStep), mLogicObjects[i] );
		}
	}
	Jobs::System::Current::WaitForJobs( Jobs::JOBTYPE_RENDERSTEP );
	// Perform the synchronous logic jobs
	for ( i = 0; i < mLoCurrentIndex; ++i ) {
		if ( mLogicObjects[i] && mLogicObjects[i]->active ) {
			mLogicObjects[i]->PostStepSynchronus();
		}
	}

	// Queue transform update
	/*static ftype updateTimer = 0;
	updateTimer += Time::deltaTime;
	if ( updateTimer > 1/30.0f ) {
		CTransform::updateRenderSide = true;
		updateTimer = std::min<ftype>( 1/30.0f, updateTimer-(1/30.0f) );
	}
	else {
		CTransform::updateRenderSide = false;
	}*/
	//CTransform::updateRenderSide = true;

	// Do error check at this point
	if ( glMaterial::Default->passinfo.size() > 16 ) throw std::exception();
	GL.CheckError();

	// Frame cleanup
	GL.EndFrame();
}

// Normal rendering routine. Called by a camera. Camera passes in its render hint.
// This function will render to the scene targets needed.
void CRenderState::RenderScene ( const uint32_t n_renderHint )
{
	GL_ACCESS GLd_ACCESS
	// Save current mode
	int t_currentRenderMode = CGameSettings::Active()->i_ro_RendererMode;
	// Override it with forward
	CGameSettings::Active()->i_ro_RendererMode = RENDER_MODE_FORWARD;

	CRenderableObject * pRO;
	tRenderRequest	renderRQ;
	int i;
	unsigned char passCount;
	CCamera* currentCamera = CCamera::activeCamera;

#ifndef _ENGINE_RELEASE
	GLd.vertexCount = 0;
	GLd.triangleCount = 0;
#endif

	// Loop through each hint (reverse mode)
	for ( uint currentLayer = 1<<RL_LAYER_COUNT; currentLayer != 0; currentLayer >>= 1 )
	{
		int layer = Math.log2( currentLayer );
		// Skip non-drawn hints
		if ( !(currentLayer & n_renderHint) ) {
			continue;
		}
		// Skip non-world if no buffer model
		if ( !CGameSettings::Active()->b_ro_UseBufferModel ) {
			if ( currentLayer != RL_WORLD ) {
				continue;
			}
		}

		// Create a sorted render list.
		std::vector<tRenderRequest> sortedRenderList;
		for ( i = 0; i < (int)iCurrentIndex; i += 1 )
		{
			pRO = pRenderableObjects[i];
			if ( pRO /*&& pRO->visible*/ )
			{
				// Only add to list if visible in this hint.
				if ( pRO->renderSettings.renderHints & currentLayer )
				{
					// Add each pass to the render list
					passCount = pRO->GetPassNumber();
					for ( unsigned char p = 0; p < passCount; ++p )
					{
						glPass* pass = pRO->GetPass(p);
						// Mask the render hint (multipass across multiple targets)
						if ( pass->m_hint & currentLayer )
						{
							tRenderRequest newRequest;
							newRequest.obj  = pRO;
							newRequest.pass = p;
							sortedRenderList.push_back( newRequest );
						}
					}
				}
			}
		}
		// Sort the render requests
		std::sort( sortedRenderList.begin(), sortedRenderList.end(), OrderComparatorForward ); 

		// If using buffer model
		if ( CGameSettings::Active()->b_ro_UseBufferModel )
		{
			// For the final camera, render to a buffer
			if ( currentLayer == RL_WORLD && currentCamera == mainBufferCamera )
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
		Renderer::Settings.fogScale = 1.0f / (Renderer::Settings.fogEnd - Renderer::Settings.fogStart);//currentCamera->zFar
		// Update UBOs
		glMaterial::updateStaticUBO();

		if ( currentCamera->clearColor ) {
			// Clear with BG color
			//glClear( GL_COLOR_BUFFER_BIT ); // use rendereing options for this
		}

		int sortedListSize = sortedRenderList.size();

		// First work on prerender
		for ( i = 0; i < (int)sortedListSize; ++i )
		{
			sortedRenderList[i].obj->PreRender( sortedRenderList[i].pass );
			GL.CheckError();
		}

		// Then work on the actual render
		for ( int rt = Background; rt <= V2D; rt += 1 )
		{
			if ( !currentCamera->layerVisibility[rt] ) { // move to renderer generator
				continue;
			}
			if ( currentCamera->clearDepthAfterLayer ) {
				glDepthMask( GL_TRUE );
				glClear( GL_DEPTH_BUFFER_BIT );
			}
			for ( i = 0; i < (int)sortedListSize; ++i )
			{
				renderRQ = sortedRenderList[i];
				pRO = renderRQ.obj;
				//if (( pRO )&&( pRO->renderType == rt ))
				if ( pRO->visible && pRO->renderType == rt )
				{
					GL.prepareDraw();
					if ( !pRO->Render( renderRQ.pass ) ) {
						throw std::exception();
					}
					GL.CheckError();
					GL.cleanupDraw();
				}
			}
		}

		// Finally, work on postrender
		for ( i = 0; i < (int)sortedListSize; i += 1 )
		{
			sortedRenderList[i].obj->PostRender( sortedRenderList[i].pass );
		}
		// End render loop

		// Unbind fbo when done rendering
		if ( CGameSettings::Active()->b_ro_UseBufferModel )
		{
			// For the final camera, render to a buffer
			if ( currentLayer == RL_WORLD && currentCamera == mainBufferCamera )
			{
				//GL.GetMainScreenBuffer()->UnbindBuffer();
				GL.GetMainScreenBuffer()->UnbindBuffer();
			}
			else
			{
				// Unbind the proper layer buffer
				int layer = Math.log2( currentLayer );
			}
		}
	}

	// Restore render mode
	CGameSettings::Active()->i_ro_RendererMode = t_currentRenderMode;

#ifndef _ENGINE_RELEASE
	//printf( "%d - %d ::\n", GL.vertexCount, GL.triangleCount );
#endif

}

// Some debug compiler flags
//#define SKIP_NON_WORLD_STUFF
#define FORCE_BUFFER_CLEAR

// Deferred rendering routine.
void CRenderState::RenderSceneDeferred ( const uint32_t n_renderHint )
{
	GL_ACCESS GLd_ACCESS
	CRenderableObject * pRO;
	int i;
	unsigned char passCount;
	CCamera* currentCamera = CCamera::activeCamera;
	bool firstRender = true;

	TimeProfiler.BeginTimeProfile( "rs_render_makelist" );
	// Create a sorted render list.
	std::vector<tRenderRequest> sortedRenderList;
	for ( i = 0; i < (int)iCurrentIndex; i += 1 )
	{
		pRO = pRenderableObjects[i];
		if ( pRO )
		{
#ifdef SKIP_NON_WORLD_STUFF
			// Only add 2D or world objects for now
			if ( pRO->renderType != Renderer::World && pRO->renderType != Renderer::V2D ) { 
				continue;
			}
#endif
			// Only add to list if visible in this hint.
			if ( pRO->renderSettings.renderHints & n_renderHint )
			{
				// Add each pass to the render list
				passCount = pRO->GetPassNumber();
				for ( unsigned char p = 0; p < passCount; ++p )
				{
					glPass_Deferred* pass = pRO->GetPassDeferred(p);
					// Mask the render hint (multipass across multiple targets)
					//if ( pass->m_hint & currentLayer )
					{
						tRenderRequest newRequest;
						newRequest.obj  = pRO;
						newRequest.pass = p;
						newRequest.forward = false;
						newRequest.renderType = pRO->renderType;
						sortedRenderList.push_back( newRequest );
					}
				}
				if ( passCount == 0 ) // No deferred passes? Must be a world object.
				{
					if ( pRO->GetMaterial() && pRO->GetMaterial()->passinfo.size() )
					{
						glPass* pass = pRO->GetPass(0);
						if ( pass->m_hint & RL_WORLD )
						{
							tRenderRequest newRequest;
							newRequest.obj  = pRO;
							newRequest.pass = 0;
							newRequest.forward = true;
							newRequest.renderType = pRO->renderType;
							sortedRenderList.push_back( newRequest );
						}
					}
				}
			}
		} //
	}
	if ( CGameSettings::Active()->i_ro_RendererMode == RENDER_MODE_DEFERRED ) {
		std::sort( sortedRenderList.begin(), sortedRenderList.end(), OrderComparatorDeferred ); 
		//std::sort( sortedRenderList.begin(), sortedRenderList.end(), OrderComparatorForward ); 
	}
	else {
		//throw std::exception( "Bad renderer mode! Only Forward-1 and Deferred-2 are supported!" );
		throw std::exception( "Inside RenderScene() when not in Forward-1!" );
	}
	TimeProfiler.EndAddTimeProfile( "rs_render_makelist" );

	// For the final camera, render to a buffer
	/*if ( currentCamera == mainBufferCamera )
	{
		GL.GetMainScreenBuffer()->BindBuffer();
		GL.CheckError();
	}*/
	if ( !currentCamera->IsRTCamera() )
	{
		CRenderTexture* currentMainRenderTarget = currentCamera->GetRenderTexture();
		currentMainRenderTarget->BindBuffer();
		GL.setupViewport(0,0,currentMainRenderTarget->GetWidth(),currentMainRenderTarget->GetHeight());
		GL.CheckError();
	}

	// Calculate pass globals
	Renderer::Settings.fogScale = 1.0f / (Renderer::Settings.fogEnd - Renderer::Settings.fogStart);//currentCamera->zFar
	// Update UBOs
	glMaterial::updateStaticUBO();
	GL.CheckError();
	glMaterial::updateLightTBO();
	GL.CheckError();

	int sortedListSize = sortedRenderList.size();

	// First work on prerender
	TimeProfiler.BeginTimeProfile( "rs_render_pre" );
	for ( i = 0; i < sortedListSize; ++i )
	{
		sortedRenderList[i].obj->PreRender( sortedRenderList[i].pass );
	}
	TimeProfiler.EndAddTimeProfile( "rs_render_pre" );
	GL.CheckError();

	/*for ( int rt = Background; rt <= V2D; rt += 1 )
	{
		//if ( !currentCamera->layerVisibility[rt] ) { // move to renderer generator
		if ( rt != World ) { //only render world for now
			continue;
		}
		if ( currentCamera->clearDepthAfterLayer ) {
			GL.prepareDraw();
			glDepthMask( GL_TRUE );
			glClear( GL_DEPTH_BUFFER_BIT );
			GL.cleanupDraw();
		}
		for ( i = 0; i < sortedListSize; ++i )
		{
			renderRQ = sortedRenderList[i];
			pRO = renderRQ.obj;
			//if (( pRO )&&( pRO->renderType == rt ))
			if ( pRO->visible && pRO->renderType == rt )
			{
				GL.prepareDraw();
				if ( !pRO->Render( renderRQ.pass ) ) {
					throw std::exception();
				}
				GL.CheckError();
				GL.cleanupDraw();
			}
		}
	}*/

	// Then work on the actual render
	tRenderRequest	renderRQ_current;
	tRenderRequest	renderRQ_next;
	i = -1; // Starts at -1 in case first object requires forward renderer
	bool rendered = false;
	while ( i < sortedListSize )
	{
		// Grab the next two render objects
		if ( i < 0 ) {
			renderRQ_current.obj = NULL;
			renderRQ_current.renderType = Background;
			renderRQ_current.forward = false;
			renderRQ_current.screenshader = false;
			renderRQ_current.pass = 0;
		}
		else {
			renderRQ_current = sortedRenderList[i];
		}
		if ( i < sortedListSize-1 ) {
			renderRQ_next = sortedRenderList[i+1];
		}
		else {
			renderRQ_next.obj = NULL;
		}
		// Render the object
		if ( !renderRQ_current.forward && renderRQ_current.obj && renderRQ_current.obj->visible )
		{
			if ( renderRQ_current.transparent ) {
				//throw std::exception();
				// Skip these for now
			}
			else {
				GL.prepareDraw();
				if ( !renderRQ_current.obj->Render( renderRQ_current.pass ) ) {
					throw std::exception();
				}
				GL.CheckError();
				GL.cleanupDraw();
				rendered = true;
			}
		}
		else if ( renderRQ_current.forward )
		{
			throw std::exception();
		}

		// If there's another object to render, check for layer state changes
		if ( renderRQ_next.obj )
		{
			// Check for hook to perform lighting on the layer
			if ( (i >= 0) && (rendered) && ( renderRQ_current.renderType == World ) && ( 
				//( renderRQ_next.obj == NULL && renderRQ_current.transparent == false ) // Render out if this will be the last solid object period
				//||( renderRQ_current.transparent == false && renderRQ_next.obj && renderRQ_next.transparent == true ) // Render out if this will be the last solid object on this list
				//||( renderRQ_next.obj && renderRQ_current.screenshader == false && renderRQ_next.screenshader == true ) // Render out if the next object is a screenshader
				( renderRQ_next.obj == NULL ) // Render out if this will be the last deferred object period
				||( renderRQ_next.obj && renderRQ_current.forward == false && renderRQ_next.forward == true ) // Render out if this is last deferred object on this list
				||( renderRQ_next.obj && renderRQ_current.forward == false && renderRQ_current.renderType != renderRQ_next.renderType ) // Render out if this will be the last deferred object on this layer
				))
			{
				
				TimeProfiler.BeginTimeProfile( "rs_render_lightpush" );

				// Set up the scene rendering types
				static glMaterial* LightingPass = NULL;
				if ( LightingPass == NULL ) {
					LightingPass = new glMaterial();
					// Setup forward pass
					LightingPass->passinfo.push_back( glPass() );
					LightingPass->passinfo[0].shader = new glShader( "shaders/def_screen/pass_lighting.glsl" );
					LightingPass->passinfo[0].m_face_mode = Renderer::FM_FRONTANDBACK;
				}
				static glMaterial* EchoPass = NULL;
				if ( EchoPass == NULL ) {
					EchoPass = new glMaterial();
					// Setup forward pass
					EchoPass->passinfo.push_back( glPass() );
					EchoPass->passinfo[0].shader = new glShader( "shaders/def_screen/pass_lighting_echo.glsl" );
					EchoPass->passinfo[0].m_face_mode = Renderer::FM_FRONTANDBACK;
				}
				static glMaterial* ShaftPass = NULL;
				if ( ShaftPass == NULL ) {
					ShaftPass = new glMaterial();
					// Setup forward pass
					ShaftPass->passinfo.push_back( glPass() );
					ShaftPass->passinfo[0].shader = new glShader( "shaders/def_screen/pass_lighting_shaft.glsl" );
					ShaftPass->passinfo[0].m_face_mode = Renderer::FM_FRONTANDBACK;
					// Set effect textures
					ShaftPass->setTexture( 5, new CTexture( "textures/ditherdots.jpg" ) );
				}

				// Bind main screen in order to render the deferred result to it
				CRenderTexture* currentMainRenderTarget = GL.GetMainScreenBuffer();
				{
					currentMainRenderTarget->BindBuffer();
					GL.setupViewport(0,0,currentMainRenderTarget->GetWidth(),currentMainRenderTarget->GetHeight());
				}
				// Clear on first render
				/*if ( firstRender ) {
					if ( currentCamera->clearColor ) {
						// Clear with BG color
						glClear( GL_COLOR_BUFFER_BIT ); // use rendering options for this
					}
					firstRender = false;
				}*/
				GL.CheckError();

				// Choose lighting pass to use
				glMaterial* targetPass = LightingPass;
				if ( glMaterial::special_mode == Renderer::SP_MODE_ECHO ) {
					targetPass = EchoPass;
				}
				else if ( glMaterial::special_mode == Renderer::SP_MODE_SHAFT ) {
					targetPass = ShaftPass;
				}

				// Change the projection to identity-type 2D (similar to orthographic)
				GL.pushModelMatrix( Matrix4x4() );
					// Perform a lighting pass
					targetPass->setSampler( 0, ((CMRTTexture*)(currentCamera->GetRenderTexture()))->GetBufferTexture(0), GL.Enum(Texture2D) );
					targetPass->setSampler( 1, ((CMRTTexture*)(currentCamera->GetRenderTexture()))->GetBufferTexture(1), GL.Enum(Texture2D) );
					targetPass->setSampler( 2, ((CMRTTexture*)(currentCamera->GetRenderTexture()))->GetBufferTexture(2), GL.Enum(Texture2D) );
					targetPass->setSampler( 3, ((CMRTTexture*)(currentCamera->GetRenderTexture()))->GetBufferTexture(3), GL.Enum(Texture2D) );
					targetPass->setSampler( 4, ((CMRTTexture*)(currentCamera->GetRenderTexture()))->GetDepthSampler(), GL.Enum(Texture2D) );
					targetPass->bindPassForward(0);
					// Pass in all the lighting information
					{
						glShader* shader = targetPass->getUsingShader();
						// Light number
						int uniformLocation = shader->get_uniform_location( "sys_LightNumber" );
						if ( uniformLocation >= 0 )
						{
							glUniform1i( uniformLocation, std::min<int>( glMaterial::m_lightCount, (renderRQ_current.obj->renderType==World)?64:3 ) );
						}
						// Light data
						uniformLocation = shader->get_uniform_block_location( "def_LightingInfo" );
						if ( uniformLocation >= 0 )
						{
							glUniformBlockBinding( shader->get_program(), uniformLocation, 5 );
							glBindBufferRange( GL_UNIFORM_BUFFER, 5, glMaterial::m_ubo_deflightinginfo, NIL, sizeof(Matrix4x4)*4 );
						}
					}
					// Enable alpha blending
					glEnable( GL_BLEND );
					glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
					// Disable writing to the depth buffer
					glDepthMask( false );
					glDepthFunc( GL_ALWAYS );
					// Draw the screen quad
					GLd.DrawScreenQuad();
					// Change depth equation back to the previous value
					glDepthFunc( GL_LEQUAL );
				// Finish rendering
				GL.popModelMatrix();

				// Pop the main buffer off the render stack, returning to the MRT target
				currentMainRenderTarget->UnbindBuffer();

				// Clear with BG color
#ifdef FORCE_BUFFER_CLEAR
				glClear( GL_COLOR_BUFFER_BIT ); // use rendering options for this
#endif

				TimeProfiler.EndAddTimeProfile( "rs_render_lightpush" );
			}
			// Check if should temporarily switch to forward mode (for alphamode items)
			if (
				( renderRQ_next.obj && renderRQ_current.forward == false && renderRQ_next.forward == true ) // Last deferred object, switch to forward mode
				//||( renderRQ_current.transparent == false && renderRQ_next.obj && renderRQ_next.transparent == true ) // Last solid object, switch to forward mode
				//||( renderRQ_next.obj && renderRQ_current.screenshader == false && renderRQ_next.screenshader == true ) // Last non-screenshader object, switch to forward mode
				)
			{
				rendered = false;

				// Copy over the depth to use when drawing in forward mode
				GL.GetMainScreenBuffer()->BindBuffer();
				GL.setupViewport(0,0,GL.GetMainScreenBuffer()->GetWidth(),GL.GetMainScreenBuffer()->GetHeight());
				glBindFramebuffer( GL_READ_FRAMEBUFFER, currentCamera->GetRenderTexture()->GetRTInfo().findex );
				glBindFramebuffer( GL_DRAW_FRAMEBUFFER, GL.GetMainScreenBuffer()->GetRTInfo().findex );
				glBlitFramebuffer(
					0,0,Screen::Info.width,Screen::Info.height,
					0,0,Screen::Info.width,Screen::Info.height,
					GL_DEPTH_BUFFER_BIT, GL_NEAREST ); // Color doesn't need to be blit, since the deferred compositor outputs color
				GL.CheckError();

				if ( false ) // Driver depth-blit unit test
				{
					float depth_test_0 [4];
					float depth_test_1 [4];
					glBindFramebuffer( GL_READ_FRAMEBUFFER, currentCamera->GetRenderTexture()->GetRTInfo().findex );
					glReadPixels( 10,10, 2,2, GL_DEPTH_COMPONENT, GL_FLOAT, &depth_test_0 );
					glBindFramebuffer( GL_READ_FRAMEBUFFER, GL.GetMainScreenBuffer()->GetRTInfo().findex );
					glReadPixels( 10,10, 2,2, GL_DEPTH_COMPONENT, GL_FLOAT, &depth_test_1 );
				
					for ( int n = 0; n < 4; ++n )
					{
						if ( depth_test_0[n] != depth_test_1[n] )
						{
							throw Renderer::GLStateException();
						}
					}
				}

				// Set to forward mode
				CGameSettings::Active()->i_ro_RendererMode = RENDER_MODE_FORWARD;
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
					//if ( renderRQ_current.obj->visible && renderRQ_current.renderType == V2D && (
					if ( renderRQ_current.obj->visible && (
						( (glMaterial::special_mode == Renderer::SP_MODE_NORMAL || glMaterial::special_mode == Renderer::SP_MODE_SHAFT) && renderRQ_current.forward )
						||( glMaterial::special_mode == Renderer::SP_MODE_ECHO && renderRQ_current.forward  && renderRQ_current.renderType == V2D ) // (unless echopass, then only do v2d)
						))
					{
						GL.prepareDraw();
						if ( !renderRQ_current.obj->Render( renderRQ_current.pass ) ) {
							throw std::exception();
						}
						GL.CheckError();
						GL.cleanupDraw();
					}
					else if ( !renderRQ_current.forward ) 
					{
						throw std::exception();
					}
					// Check the next object to see if should keep rendering
					if ( 
						( renderRQ_next.obj == NULL ) // No next object
						||( !renderRQ_next.forward ) // Next object is not forward
						//||( !renderRQ_next.transparent && !renderRQ_next.screenshader ) // Next object is not transparent or screenshader
						)
					{	// Stop rendering forward mode
						renderForward = false;
					}
					// If next object is on another layer, just swap modes (check for main layer switch)
					else if ( renderRQ_current.renderType != renderRQ_next.renderType )
					{
						if ( currentCamera->clearDepthAfterLayer ) {
							glDepthMask( GL_TRUE );
							glClear( GL_DEPTH_BUFFER_BIT );
						}
					}
				}
				// Switch back to deferred renderer
				CGameSettings::Active()->i_ro_RendererMode = RENDER_MODE_DEFERRED;
				// Go back to deferred buffer
				//GL.GetMainScreenBuffer()->UnbindBuffer();
				GL.GetMainScreenBuffer()->UnbindBuffer();
			}
			// Check for main layer switch
			if ( renderRQ_next.obj && renderRQ_current.renderType != renderRQ_next.renderType )
			{
				if ( currentCamera->clearDepthAfterLayer ) {
					glDepthMask( GL_TRUE );
					glClear( GL_DEPTH_BUFFER_BIT );
				}
			}
		}
		// Go to the next job
		i += 1;
	}

	// Finally, work on postrender
	for ( i = 0; i < sortedListSize; i += 1 )
	{
		sortedRenderList[i].obj->PostRender( sortedRenderList[i].pass );
	}
	GL.CheckError();
	// End render loop

	// Unbind fbo when done rendering
	if ( !currentCamera->IsRTCamera() )
	{
		//GL.GetMainScreenBuffer()->UnbindBuffer();
		currentCamera->GetRenderTexture()->UnbindBuffer();
		GL.CheckError();
	}

	// For the final camera, render to a buffer
	if ( currentCamera == mainBufferCamera )
	{
		/*GL.GetMainScreenBuffer()->BindBuffer();
		GL.CheckError();

		// Change the projection to orthographic
		GL.beginOrtho(-1,-1,2,2,-1,1,false);
			// Copy buffer over
			glMaterial::Copy->setSampler( 0, ((CMRTTexture*)(currentCamera->GetRenderTexture()))->GetBufferTexture(0) );
			//glMaterial::Copy->setSampler( 0, currentCamera->GetRenderTexture()->GetColorSampler() );
			glMaterial::Copy->bindPassForward(0);

			glEnable( GL_BLEND );
			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
			glDepthMask( false );
			glDepthFunc( GL_ALWAYS );
				GLd.DrawScreenQuad();
			glDepthFunc( GL_LEQUAL );
		GL.endOrtho();

		GL.GetMainScreenBuffer()->UnbindBuffer();*/
		/*GL.GetMainScreenBuffer()->BindBuffer();
		glBindFramebuffer( GL_READ_FRAMEBUFFER, currentCamera->GetRenderTexture()->GetRTInfo().findex );
		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, GL.GetMainScreenBuffer()->GetRTInfo().findex );
		glBlitFramebuffer(
			0,0,Screen::Info.width,Screen::Info.height,
			0,0,Screen::Info.width,Screen::Info.height,
			GL_COLOR_BUFFER_BIT, GL_LINEAR );
		GL.GetMainScreenBuffer()->UnbindBuffer();*/
	}

}

// Specialized Rendering Routines
void CRenderState::PreRenderSetLighting ( std::vector<CLight*> & lightsToUse )
{
	//vSpecialRender_LightList = lightsToUse;
	vSpecialRender_LightList = *CLight::GetLightList();
	*CLight::GetLightList() = lightsToUse;

	bSpecialRender_ResetLights = true;

	// Calculate pass globals
	Renderer::Settings.fogScale = 1.0f / (Renderer::Settings.fogEnd - Renderer::Settings.fogStart);//currentCamera->zFar
	// Update UBOs
	glMaterial::updateStaticUBO();
}
// RenderSingleObject renders an object, assuming the projection has been already set up.
void CRenderState::RenderSingleObject ( CRenderableObject* objectToRender )
{
	GL_ACCESS;
	char maxPass = objectToRender->GetPassNumber();
	for ( char pass = 0; pass < maxPass; ++pass )
	{
		if ( objectToRender->GetPass( pass )->m_hint & RL_WORLD )
		{
			GL.prepareDraw();
			objectToRender->Render( pass );
			GL.cleanupDraw();
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
			GL.prepareDraw();
			objectsToRender[i]->Render( pass );
			GL.cleanupDraw();
		}
	}

	if ( bSpecialRender_ResetLights ) {
		*CLight::GetLightList() = vSpecialRender_LightList;
		bSpecialRender_ResetLights = false;
	}
}