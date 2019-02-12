#include "RrRenderer.h"
#include "Settings.h"

#include "core/settings/CGameSettings.h"
#include "core/system/Screen.h"
#include "core-ext/threads/Jobs.h"
#include "core/debug/console.h"

#include "renderer/object/CRenderableObject.h"
#include "renderer/logic/RrLogicObject.h"

//#include "renderer/material/RrMaterial.h"
#include "renderer/texture/RrTexture.h"
#include "renderer/texture/RrRenderTexture.h"
//#include "renderer/texture/CMRTTexture.h"

#include "renderer/debug/RrDebugDrawer.h"
#include "renderer/debug/RrDebugRTInspector.h"
//#include "renderer/object/sprite/CSpriteContainer.h"

#include "core-ext/resources/ResourceManager.h"
#include "renderer/texture/RrTextureMasterSubsystem.h"
#include "renderer/material/RrShaderMasterSubsystem.h"
#include "renderer/logic/model/RrModelMasterSubsystem.h"

//#include "renderer/resource/CResourceManager.h"

#include "renderer/gpuw/Texture.h"
#include "renderer/gpuw/RenderTarget.h"

#include "renderer/windowing/RrWindow.h"

#include "renderer/material/RrPass.h"
#include "renderer/material/RrPass.Presets.h"
#include "renderer/material/RrShaderProgram.h"

#include "renderer/state/RrPipelinePasses.h"

//===Class static member data===

RrRenderer* RrRenderer::Active	= NULL;
RrRenderer* SceneRenderer			= NULL;

//===Class functions===

// Class constructor
//  saves current instance into pActive
//  initializes list of renderers
RrRenderer::RrRenderer ( CResourceManager* nResourceManager )
{
	Active = this;
	SceneRenderer = this;

	// Create resource manager
	/*if ( mResourceManager == NULL )
	{
		mResourceManager = new CResourceManager;
		mResourceManager->m_renderStateOwned = true;
	}*/
	// Set up resource manangers
	auto resourceManager = core::ArResourceManager::Active();
	if (resourceManager->GetSubsystem(core::kResourceTypeRrTexture) == NULL)
		resourceManager->SetSubsystem(core::kResourceTypeRrTexture, new RrTextureMasterSubsystem());
	if (resourceManager->GetSubsystem(core::kResourceTypeRrShader) == NULL)
		resourceManager->SetSubsystem(core::kResourceTypeRrShader, new RrShaderMasterSubsystem());
	if (resourceManager->GetSubsystem(core::kResourceTypeRrMeshGroup) == NULL)
		resourceManager->SetSubsystem(core::kResourceTypeRrMeshGroup, new RrModelMasterSubsystem());

	// Set up reandering options
	{
		bufferedMode	= CGameSettings::Active()->b_ro_UseBufferModel ? true : false;
		shadowMode		= CGameSettings::Active()->b_ro_EnableShadows ? true : false;
		//renderMode		= (eRenderMode)CGameSettings::Active()->i_ro_RendererMode;
		pipelineMode	= renderer::kPipelineModeNormal;
	}
	
	// Set up arrays
	iListSize		= 500; // Start with 500 slots
	iCurrentIndex	= 0;
	pRenderableObjects.resize( iListSize, NULL );

	mLoListSize		= 50; // Start with 50 logic slots
	mLoCurrentIndex	= 0;
	mLogicObjects.resize( mLoListSize, NULL );
}

void RrRenderer::InitializeWithDeviceAndSurface ( gpu::Device* device, gpu::OutputSurface* surface )
{
	// Grab that device!
	mDevice = device;
	// Grab the surface!
	mOutputSurface	= surface;
	// And pull out a graphics queue from the rendering device:
	mGfxContext		= mDevice->getContext();
	mComputeContext	= mDevice->getComputeContext();

	ARCORE_ASSERT(mGfxContext->validate() == 0);

	// Set up default rendering targets
	{
		internal_settings.mainColorAttachmentCount = 4;
		internal_settings.mainColorAttachmentFormat = core::gfx::tex::kColorFormatRGBA16F;
		internal_settings.mainDepthFormat = core::gfx::tex::kDepthFormat32;
		internal_settings.mainStencilFormat = core::gfx::tex::KStencilFormatIndex16;
	}
	// Set initial rendertarget states
	{
		internal_chain_current = NULL;
		internal_chain_index = 0;
	}

	// Create the rendertargets now
	ResizeSurface(); // Will create the new sized render targets.
	ARCORE_ASSERT(mGfxContext->validate() == 0);

	// Create the cbuffers relying on the surface counts
	internal_cbuffers_frames.resize(internal_chain_list.size());
	internal_cbuffers_passes.resize(internal_chain_list.size() * renderer::kRenderLayer_MAX);
	for (gpu::Buffer& buffer : internal_cbuffers_frames)
		buffer.initAsConstantBuffer(NULL, sizeof(renderer::cbuffer::rrPerFrame));
	for (gpu::Buffer& buffer : internal_cbuffers_passes)
		buffer.initAsConstantBuffer(NULL, sizeof(renderer::cbuffer::rrPerPassLightingInfo));

	ARCORE_ASSERT(mGfxContext->validate() == 0);

	// Create default textures
	{
		RrTexture* white_texture = RrTexture::CreateUnitialized(renderer::kTextureWhite); //new RrTexture("");
		{
			core::gfx::arPixel white (255, 255, 255, 255);
			white_texture->Upload( false, &white, 1,1, core::gfx::tex::kColorFormatRGBA8,
				core::gfx::tex::kWrappingRepeat, core::gfx::tex::kWrappingRepeat,
				core::gfx::tex::kMipmapGenerationNone, core::gfx::tex::kSamplingPoint );
		}
		//renderer::Resources::AddTexture(renderer::TextureWhite, white_texture);
		ARCORE_ASSERT(mGfxContext->validate() == 0);

		RrTexture* black_texture = RrTexture::CreateUnitialized(renderer::kTextureBlack);
		{
			core::gfx::arPixel black (0, 0, 0, 255);
			black_texture->Upload( false, &black, 1,1, core::gfx::tex::kColorFormatRGBA8,
				core::gfx::tex::kWrappingRepeat, core::gfx::tex::kWrappingRepeat,
				core::gfx::tex::kMipmapGenerationNone, core::gfx::tex::kSamplingPoint );
		}
		//renderer::Resources::AddTexture(renderer::TextureBlack, black_texture);

		RrTexture* gray0_texture = RrTexture::CreateUnitialized(renderer::kTextureGrayA0);
		{
			core::gfx::arPixel gray0 (127, 127, 127, 0);
			gray0_texture->Upload( false, &gray0, 1,1, core::gfx::tex::kColorFormatRGBA8,
				core::gfx::tex::kWrappingRepeat, core::gfx::tex::kWrappingRepeat,
				core::gfx::tex::kMipmapGenerationNone, core::gfx::tex::kSamplingPoint );
		}
		//renderer::Resources::AddTexture(renderer::TextureGrayA0, gray0_texture);
	}
	ARCORE_ASSERT(mGfxContext->validate() == 0);

	// Create the default material
	//if ( renderer::pass::Default == NULL )
	//{
	//	// The default material must be a single pass in both modes.
	//	// This to provide compatibility with the default system implementation. (A lot of early engine code is implemented lazily)
	//	renderer::pass::Default = new RrPass;
	//	renderer::pass::Default->setTexture( TEX_DIFFUSE, RrTexture::Load(renderer::kTextureWhite) );
	//	renderer::pass::Default->setTexture( TEX_NORMALS, RrTexture::Load( "textures/default_normals.jpg" ) );
	//	renderer::pass::Default->setTexture( TEX_SURFACE, RrTexture::Load(renderer::kTextureBlack) );
	//	renderer::pass::Default->setTexture( TEX_OVERLAY, RrTexture::Load(renderer::kTextureGrayA0) );
	//	renderer::pass::Default->m_type = kPassTypeForward;
	//	renderer::pass::Default->m_program = RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/d/diffuse_vv.spv", "shaders/d/diffuse_p.spv"});
	//	// Setup deferred pass
	//	// todo?
	//}
	if ( renderer::pass::Copy == NULL )
	{
		renderer::pass::Copy = new RrPass;
		renderer::pass::Copy->m_type = kPassTypeForward;
		renderer::pass::Copy->m_program = RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/sys/copy_buffer_simple_vv.spv", "shaders/sys/copy_buffer_simple_p.spv"});
		renderer::pass::Copy->m_depthTest = gpu::kCompareOpAlways;
		renderer::pass::Copy->m_cullMode = gpu::kCullModeNone;
	}
	// Create the fallback shader
	if ( renderer::pass::Fullbright == NULL )
	{
		renderer::pass::Fullbright = new RrPass;
		renderer::pass::Fullbright->m_type = kPassTypeForward;
		renderer::pass::Fullbright->m_program = RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/sys/fullbright_vv.spv", "shaders/sys/fullbright_p.spv"});
	}
	//// Create the default hint options
	//if ( renderer::m_default_hint_options == NULL )
	//{
	//	renderer::m_default_hint_options = new renderer::_n_hint_rendering_information();
	//}
	ARCORE_ASSERT(mGfxContext->validate() == 0);

	// Create the pipeline utils
	pipelinePasses = new renderer::pipeline::RrPipelinePasses();

	bSpecialRender_ResetLights = false;

	// Create the debug tools
	new debug::RrDebugDrawer;
	new debug::RrDebugRTInspector;

	ARCORE_ASSERT(mGfxContext->validate() == 0);
}


// Class destructor
//  sets pActive pointer to null
//  frees list of renderers, but not the renderers
RrRenderer::~RrRenderer ( void )
{
	// Delete the debug tools
	delete debug::Drawer;
	delete debug::RTInspector;

	// Free state info
	Active			= NULL;
	SceneRenderer	= NULL;

	// Reset arrays
	iCurrentIndex = 0;
	iListSize = 0;

	// Free system default material
	//delete_safe(renderer::pass::Default);
	delete_safe(renderer::pass::Copy);
	delete_safe(renderer::pass::Fullbright);

	// Free the other materials
	delete_safe(pipelinePasses);

	// Stop the resource manager
	/*if ( mResourceManager && mResourceManager->m_renderStateOwned ) {
		mResourceManager->m_renderStateOwned = false;
		delete mResourceManager;
		mResourceManager = NULL;
	}*/
	// TODO
}


//-Adding and Removing Renderable/Logic Objects-

// AddRO ( pointer to new RO )
//  adds an RO to be drawn and returns the index
//  used only by the RO constructor
unsigned int RrRenderer::AddRO ( CRenderableObject * pRO )
{
	// Randomly reshift the renderable objects
	if ( (rand()%10) == 0 )
	{
		CRenderableObject* tmpRO;
		// Shift them suckers
		unsigned int newIndex = 0;
		for ( unsigned int i = 0; i < iCurrentIndex; i += 1 )
		{
			tmpRO = pRenderableObjects[i];
			if ( tmpRO != NULL ) {
				pRenderableObjects[i] = NULL;
				pRenderableObjects[newIndex] = tmpRO;
				tmpRO->id = newIndex;
				newIndex += 1;
			}
		}
		iCurrentIndex = newIndex;
	}

	// Check if current index is valid
	// If the index is not valid, resize the array
	if ( iCurrentIndex >= iListSize )
	{
		// Double list size
		iListSize *= 2;
		// Resize list
		pRenderableObjects.resize( iListSize, NULL );
	}
	// Set current object to pRO
	pRenderableObjects[iCurrentIndex] = pRO;
	// Increment index for next time
	iCurrentIndex += 1;
	return iCurrentIndex-1;
}

// RemoveRO ( RO index given by AddRO )
//  sets the specified index to NULL (which the cleaner will see later)
//  only to be used by RO destructor
void RrRenderer::RemoveRO ( unsigned int id )
{
	pRenderableObjects[id] = NULL;
}

// AddLO ( pointer to new RO )
//  adds an LO to be drawn and returns the index
//  used only by the LO constructor
unsigned int RrRenderer::AddLO ( RrLogicObject * pLO )
{
	// Randomly reshift the logic objects
	if ( (rand()%10) == 0 )
	{
		RrLogicObject* tmpLO;
		// Shift them suckers
		unsigned int newIndex = 0;
		for ( unsigned int i = 0; i < mLoCurrentIndex; i += 1 )
		{
			tmpLO = mLogicObjects[i];
			if ( tmpLO != NULL ) { // If this object isn't null
				mLogicObjects[i] = NULL;
				mLogicObjects[newIndex] = tmpLO; // Place it in the next spot
				tmpLO->id = newIndex;
				newIndex += 1;
			}
		}
		mLoCurrentIndex = newIndex;
	}

	// Check if current index is valid
	// If the index is not valid, resize the array
	if ( mLoCurrentIndex >= mLoListSize )
	{
		// Double list size
		mLoListSize *= 2;
		// Resize list
		mLogicObjects.resize( mLoListSize, NULL );
	}
	// Set current object to pRO
	mLogicObjects[mLoCurrentIndex] = pLO;
	// Increment index for next time
	mLoCurrentIndex += 1;
	return mLoCurrentIndex-1;
}

// RemoveLO ( RO index given by AddRO )
//  sets the specified index to NULL (which the cleaner will see later)
//  only to be used by RO destructor
void RrRenderer::RemoveLO ( unsigned int id )
{
	// TODO: Optimize this
	Jobs::System::Current::WaitForJobs( Jobs::kJobTypeRenderStep );

	// Now set to null now that the object is pretty much gone
	mLogicObjects[id] = NULL;
}


// Rendering configuration
// ================================

// Returns the material used for rendering a screen's pass in the given effect
RrPass* RrRenderer::GetScreenMaterial ( const eRenderMode mode, const renderer::ePipelineMode mode_type )
{
	if ( mode == kRenderModeDeferred )
	{
		switch (mode_type)
		{
		case renderer::kPipelineModeNormal:	return pipelinePasses->LightingPass;
		case renderer::kPipelineModeEcho:	return pipelinePasses->EchoPass;
		case renderer::kPipelineModeShaft:	return pipelinePasses->ShaftPass;
		case renderer::kPipelineMode2DPaletted:	return pipelinePasses->Lighting2DPass;
		}
	}
	return NULL;
}

//	SetPipelineMode - Sets new pipeline mode.
// Calling this has the potential to be very slow and completely invalidate rendering state.
// This should be called as soon as possible to avoid any slowdown.
void RrRenderer::SetPipelineMode ( renderer::ePipelineMode newPipelineMode )
{
	if ( pipelineMode != newPipelineMode )
	{
		pipelineMode = newPipelineMode;
		// TODO: Invalidate shader resources.
		// NOTE: Do we need to invalidate anything?
		// Games must support switching between different pipelines on demand.
		// Invalidating shaders will make that "on demand" take a hella hot minute.
	}
}


// Settings and query
// ================================

// Returns internal settings that govern the current render setup
const renderer::rrInternalSettings& RrRenderer::GetSettings ( void ) const
{
	return internal_settings;
}

// Returns if shadows are enabled for the renderer or not.
const bool RrRenderer::GetShadowsEnabled ( void ) const
{
	return shadowMode;
}
// Returns current pipeline information
//const eRenderMode RrRenderer::GetRenderMode ( void ) const
//{
//	return renderMode;
//}
// Returns current pipeline mode (previously, special render type)
const renderer::ePipelineMode RrRenderer::GetPipelineMode ( void ) const
{
	return pipelineMode;
}


// Internal buffer handling
// ================================
//
//// Returns true if settings dropped. False otherwise.
//static bool _DropSettings (renderer::rrInternalSettings& io_settings)
//{
//	using namespace core::gfx::tex;
//	if (io_settings.mainStencilFormat == KStencilFormatIndex16) 
//	{
//		debug::Console->PrintError("Dropping Stencil16 to Stencil8.");
//		io_settings.mainStencilFormat = KStencilFormatIndex8;
//		return true;
//	}
//
//	if (io_settings.mainDepthFormat == kDepthFormat32) 
//	{
//		debug::Console->PrintError("Dropping Depth32 to Depth16.");
//		io_settings.mainDepthFormat = kDepthFormat16;
//		return true;
//	}
//
//	if (io_settings.mainStencilFormat == KStencilFormatIndex8) 
//	{
//		debug::Console->PrintError("Dropping Stencil8 to None. (This may cause visual artifacts!)");
//		io_settings.mainStencilFormat = kStencilFormatNone;
//		return true;
//	}
//
//	debug::Console->PrintError("Could not downgrade screen buffer settings.");
//
//	// Couldn't drop any settings.
//	return false;
//}

void RrRenderer::ResizeSurface ( void )
{
	// update Screen::Info.width and Screen::Info.height from the OutputSurface
	Screen::Info.width  = mOutputSurface->getWidth();
	Screen::Info.height = mOutputSurface->getHeight();

	// Force buffers to update
	CreateTargetBuffers();
}

void RrRenderer::CreateTargetBuffers ( void )
{
	if (internal_chain_list.empty())
	{
		RrHybridBufferChain chain = {};
		// Three backbuffers
		internal_chain_list.push_back(chain);
		internal_chain_list.push_back(chain);
		internal_chain_list.push_back(chain);
		// Set current buffer to first
		internal_chain_current = &internal_chain_list[0];
	}
	for (RrHybridBufferChain& chain : internal_chain_list)
	{
		renderer::rrInternalSettings settings = internal_settings;
		gpu::ErrorCode status = chain.CreateTargetBufferChain(&settings, Vector2i(Screen::Info.width, Screen::Info.height));
		if (status != gpu::kError_SUCCESS)
		{
			debug::Console->PrintError("Screen buffer formats not supported. Throwing an unsupported error.");
			throw core::DeprecatedFeatureException();
		}
	}
}
//bool RrRenderer::CreateTargetBufferChain ( rrInternalBufferChain& bufferChain )
//{
//	// Delete shared buffers
//	{
//		bufferChain.buffer_color.free();
//		bufferChain.buffer_depth.free();
//		bufferChain.buffer_stencil.free();
//
//		for (int i = 0; i < kMRTColorAttachmentCount; ++i)
//			bufferChain.buffer_deferred_color[i].free();
//	}
//	// Delete forward buffers
//	if ( bufferChain.buffer_forward_rt.empty() == false )
//	{
//		/*delete bufferChain.buffer_forward_rt;
//		bufferChain.buffer_forward_rt = NULL;
//
//		//gpu::TextureFree( bufferChain.buffer_depth );
//		delete bufferChain.buffer_depth;
//		bufferChain.buffer_depth = 0;
//
//		//gpu::TextureBufferFree( bufferChain.buffer_stencil );
//		delete bufferChain.buffer_stencil;
//		bufferChain.buffer_stencil = 0;*/
//		bufferChain.buffer_forward_rt.destroy(NULL);
//	}
//	// Delete deferred buffers
//	if ( bufferChain.buffer_deferred_mrt.empty() == false )
//	{
//		/*delete bufferChain.buffer_deferred_mrt;
//		bufferChain.buffer_deferred_mrt = NULL;
//
//		delete bufferChain.buffer_deferred_rt;
//		bufferChain.buffer_deferred_rt = NULL;*/
//		bufferChain.buffer_deferred_mrt.destroy(NULL);
//		bufferChain.buffer_deferred_rt.destroy(NULL);
//	}
//
//	// Create forward buffers
//	if ( bufferChain.buffer_forward_rt.empty() )
//	{
//		// Generate unique color buffer
//		bufferChain.buffer_color.allocate(core::gfx::tex::kTextureType2D, internal_settings.mainColorAttachmentFormat, Screen::Info.width, Screen::Info.height);
//		// Generate shared depth and stencil buffers
//		if ( internal_settings.mainDepthFormat != core::gfx::tex::kDepthFormatNone )
//		{
//			//bufferChain.buffer_depth	= gpu::TextureAllocate( Texture2D, internal_settings.mainDepthFormat, Screen::Info.width, Screen::Info.height );
//			//gpu::TextureSampleSettings( Texture2D, bufferChain.buffer_depth, Clamp, Clamp, Clamp, SamplingPoint, SamplingPoint );
//			bufferChain.buffer_depth.allocate(core::gfx::tex::kTextureType2D, internal_settings.mainDepthFormat, Screen::Info.width, Screen::Info.height);
//		}
//		if ( internal_settings.mainStencilFormat != core::gfx::tex::kStencilFormatNone )
//			//bufferChain.buffer_stencil	= gpu::TextureBufferAllocate( Texture2D, internal_settings.mainStencilFormat, Screen::Info.width, Screen::Info.height );
//			bufferChain.buffer_stencil.allocate(core::gfx::tex::kTextureType2D, internal_settings.mainStencilFormat, Screen::Info.width, Screen::Info.height);
//
//		/*bufferChain.buffer_forward_rt = new RrRenderTexture(
//			Screen::Info.width, Screen::Info.height,
//			Clamp, Clamp,
//			internal_settings.mainColorAttachmentFormat,
//			RrGpuTexture(bufferChain.buffer_depth, internal_settings.mainDepthFormat), internal_settings.mainDepthFormat != DepthNone,
//			RrGpuTexture(bufferChain.buffer_stencil, internal_settings.mainStencilFormat), false
//		);*/
//		bufferChain.buffer_forward_rt.attach(gpu::kRenderTargetSlotColor0, &bufferChain.buffer_color);
//		bufferChain.buffer_forward_rt.attach(gpu::kRenderTargetSlotDepth, &bufferChain.buffer_depth);
//		bufferChain.buffer_forward_rt.attach(gpu::kRenderTargetSlotStencil, &bufferChain.buffer_stencil);
//		// "Compile" the render target.
//		bufferChain.buffer_forward_rt.assemble();
//
//		// Check to make sure buffer is valid.
//		if (!bufferChain.buffer_forward_rt.valid())
//		{
//			return false;
//		}
//	}
//	// Create deferred buffers
//	if ( bufferChain.buffer_deferred_mrt.empty() )
//	{
//		// Create the internal stage color render target (uses shared forward buffers)
//		/*bufferChain.buffer_deferred_rt = new RrRenderTexture(
//			Screen::Info.width, Screen::Info.height,
//			Clamp, Clamp,
//			internal_settings.mainColorAttachmentFormat,
//			RrGpuTexture(bufferChain.buffer_depth, internal_settings.mainDepthFormat), internal_settings.mainDepthFormat != DepthNone,
//			RrGpuTexture(bufferChain.buffer_stencil, internal_settings.mainStencilFormat), false
//		);
//
//		RrGpuTexture		depthTexture = RrGpuTexture(bufferChain.buffer_depth, internal_settings.mainDepthFormat);
//		RrGpuTexture		stencilTexture = RrGpuTexture(bufferChain.buffer_stencil, internal_settings.mainStencilFormat);*/
//
//		// Generate unique color buffer
//		bufferChain.buffer_deferred_color_composite.allocate(core::gfx::tex::kTextureType2D, internal_settings.mainColorAttachmentFormat, Screen::Info.width, Screen::Info.height);
//
//		// Put the buffer together
//		bufferChain.buffer_deferred_rt.attach(gpu::kRenderTargetSlotColor0, &bufferChain.buffer_color);
//		bufferChain.buffer_deferred_rt.attach(gpu::kRenderTargetSlotDepth, &bufferChain.buffer_depth);
//		bufferChain.buffer_deferred_rt.attach(gpu::kRenderTargetSlotStencil, &bufferChain.buffer_stencil);
//		// "Compile" the render target.
//		bufferChain.buffer_deferred_rt.assemble();
//
//		// TODO: Make configurable
//		/*RrGpuTexture textureRequests [4];
//		memset( textureRequests, 0, sizeof(RrGpuTexture) * 4 );
//		textureRequests[0].format = RGBA8;
//		textureRequests[1].format = RGBA16F;
//		textureRequests[2].format = RGBA8;
//		textureRequests[3].format = RGBA8;*/
//
//		// TODO: Make configurable
//		bufferChain.buffer_deferred_color[0].allocate(core::gfx::tex::kTextureType2D, core::gfx::tex::kColorFormatRGBA8, Screen::Info.width, Screen::Info.height);
//		bufferChain.buffer_deferred_color[1].allocate(core::gfx::tex::kTextureType2D, core::gfx::tex::kColorFormatRGBA16F, Screen::Info.width, Screen::Info.height);
//		bufferChain.buffer_deferred_color[2].allocate(core::gfx::tex::kTextureType2D, core::gfx::tex::kColorFormatRGBA8, Screen::Info.width, Screen::Info.height);
//		bufferChain.buffer_deferred_color[3].allocate(core::gfx::tex::kTextureType2D, core::gfx::tex::kColorFormatRGBA8, Screen::Info.width, Screen::Info.height);
//
//		// Create the MRT to be used by the rendering pipeline
//		/*bufferChain.buffer_deferred_mrt = new CMRTTexture(
//			Screen::Info.width, Screen::Info.height,
//			Clamp, Clamp,
//			textureRequests + 0, 4,
//			&depthTexture, depthTexture.format != DepthNone,
//			&stencilTexture, false
//		);*/
//		// Attach shared guys
//		for (int i = 0; i < kMRTColorAttachmentCount; ++i)
//			bufferChain.buffer_deferred_mrt.attach(gpu::kRenderTargetSlotColor0 + i, &bufferChain.buffer_deferred_color[i]);
//		// Add the deoth
//		bufferChain.buffer_forward_rt.attach(gpu::kRenderTargetSlotDepth, &bufferChain.buffer_depth);
//		bufferChain.buffer_forward_rt.attach(gpu::kRenderTargetSlotStencil, &bufferChain.buffer_stencil);
//		// "Compile" the render target.
//		bufferChain.buffer_forward_rt.assemble();
//
//		// Check to make sure buffer is valid.
//		if (!bufferChain.buffer_deferred_mrt.valid())
//		{
//			return false;
//		}
//	}
//	// Made it here, so return success.
//	return true;
//}

gpu::RenderTarget* RrRenderer::GetForwardBuffer ( void )
{
	if (internal_chain_current)
		return &internal_chain_current->buffer_forward_rt;
	return NULL;
}
gpu::RenderTarget* RrRenderer::GetDeferredBuffer ( void )
{
	if (internal_chain_current)
		return &internal_chain_current->buffer_deferred_rt;
	return NULL;
}

gpu::Texture* RrRenderer::GetDepthTexture ( void )
{
	if (internal_chain_current)
		return &internal_chain_current->buffer_depth;
	return NULL;
}
gpu::WOFrameAttachment* RrRenderer::GetStencilTexture ( void )
{
	if (internal_chain_current)
		return &internal_chain_current->buffer_stencil;
	return NULL;
}