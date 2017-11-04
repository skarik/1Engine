
// Includes
#include "CRenderState.h"
#include "Settings.h"

#include "core/system/Screen.h"
#include "core-ext/threads/Jobs.h"
#include "core/debug/console.h"

#include "renderer/object/CRenderableObject.h"
#include "renderer/logic/CLogicObject.h"

#include "renderer/material/RrMaterial.h"
#include "renderer/texture/CTexture.h"
#include "renderer/texture/CRenderTexture.h"
#include "renderer/texture/CMRTTexture.h"

#include "renderer/debug/CDebugDrawer.h"
#include "renderer/debug/CDebugRTInspector.h"
//#include "renderer/object/sprite/CSpriteContainer.h"

#include "renderer/resource/CResourceManager.h"

#include "renderer/gpuw/Textures.h"

//===Class static member data===

CRenderState* CRenderState::Active	= NULL;
CRenderState* SceneRenderer			= NULL;

//===Class functions===

// Class constructor
//  saves current instance into pActive
//  initializes list of renderers
CRenderState::CRenderState ( CResourceManager* nResourceManager )
	: mResourceManager(nResourceManager)
{
	Active = this;
	SceneRenderer = this;

	// Set up default rendering targets
	{
		internal_settings.mainColorAttachmentCount = 4;
		internal_settings.mainColorAttachmentFormat = RGBA16F;
		internal_settings.mainDepthFormat = Depth32;
		internal_settings.mainStencilFormat = StencilIndex16;
	}
	// Set initial rendertarget states
	{
		/*internal_buffer_forward_rt = NULL;
		internal_buffer_depth = 0;
		internal_buffer_stencil = 0;*/
		internal_chain_current = NULL;
		internal_chain_index = 0;
	}

	// Create resource manager
	if ( mResourceManager == NULL )
	{
		mResourceManager = new CResourceManager;
		mResourceManager->m_renderStateOwned = true;
	}
	
	// Set up arrays
	iListSize		= 500; // Start with 500 slots
	iCurrentIndex	= 0;
	pRenderableObjects.resize( iListSize, NULL );

	mLoListSize		= 50; // Start with 50 logic slots
	mLoCurrentIndex	= 0;
	mLogicObjects.resize( mLoListSize, NULL );

	// Create default textures
	{
		CTexture* white_texture = new CTexture("");
		{
			pixel_t white;
			white.r = 255;
			white.g = 255;
			white.b = 255;
			white.a = 255;
			white_texture->Upload( &white, 1,1, Repeat, Repeat, MipmapNone, SamplingPoint );
		}
		renderer::Resources::AddTexture(renderer::TextureWhite, white_texture);
		white_texture->RemoveReference();

		CTexture* black_texture = new CTexture("");
		{
			pixel_t black;
			black.r = 0;
			black.g = 0;
			black.b = 0;
			black.a = 255;
			black_texture->Upload( &black, 1,1, Repeat, Repeat, MipmapNone, SamplingPoint );
		}
		renderer::Resources::AddTexture(renderer::TextureBlack, black_texture);
		black_texture->RemoveReference();

		CTexture* gray0_texture = new CTexture("");
		{
			pixel_t gray0;
			gray0.r = 127;
			gray0.g = 127;
			gray0.b = 127;
			gray0.a = 0;
			gray0_texture->Upload( &gray0, 1,1, Repeat, Repeat, MipmapNone, SamplingPoint );
		}
		renderer::Resources::AddTexture(renderer::TextureGrayA0, gray0_texture);
		gray0_texture->RemoveReference();
	}
	
	// Create the default material
	if ( RrMaterial::Default == NULL )
	{
		// The default material must be a single pass in both modes.
		// This to provide compatibility with the default system implementation. (A lot of early engine code is implemented lazily)
		RrMaterial::Default = new RrMaterial;
		RrMaterial::Default->setTexture( TEX_DIFFUSE, renderer::Resources::GetTexture(renderer::TextureWhite) );
		RrMaterial::Default->setTexture( TEX_NORMALS, new CTexture( "textures/default_normals.jpg" ) );
		RrMaterial::Default->setTexture( TEX_SURFACE, renderer::Resources::GetTexture(renderer::TextureBlack) );
		RrMaterial::Default->setTexture( TEX_OVERLAY, renderer::Resources::GetTexture(renderer::TextureGrayA0) );
		// Setup forward pass
		RrMaterial::Default->passinfo.push_back( RrPassForward() );
		RrMaterial::Default->passinfo[0].shader = new RrShader( "shaders/d/diffuse.glsl" );
		// Setup deferred pass
		RrMaterial::Default->deferredinfo.push_back( RrPassDeferred() );
	}
	if ( RrMaterial::Copy == NULL )
	{
		RrMaterial::Copy = new RrMaterial;
		// Setup forward pass
		RrMaterial::Copy->passinfo.push_back( RrPassForward() );
		RrMaterial::Copy->passinfo[0].shader = new RrShader( "shaders/sys/copy_buffer.glsl" );
		RrMaterial::Copy->passinfo[0].m_face_mode = renderer::FM_FRONTANDBACK;
		RrMaterial::Copy->passinfo[0].b_depthtest = false;
		// No deferred pass.
	}
	// Create the fallback shader
	if ( RrMaterial::Fallback == NULL )
	{
		RrMaterial::Fallback = new RrMaterial;
		// Setup forward pass
		RrMaterial::Fallback->passinfo.push_back( RrPassForward() );
		RrMaterial::Fallback->passinfo[0].shader = new RrShader( "shaders/sys/fullbright.glsl" );
		// No deferred pass.
	}
	// Create the default hint options
	if ( renderer::m_default_hint_options == NULL )
	{
		renderer::m_default_hint_options = new renderer::_n_hint_rendering_information();
	}
	// Create the render copy upscaling shader
	{
		CopyScaled = new RrMaterial();
		// Setup forward pass
		CopyScaled->passinfo.push_back( RrPassForward() );
		CopyScaled->passinfo[0].shader = new RrShader( "shaders/sys/copy_buffer_scaled.glsl" );
		CopyScaled->passinfo[0].m_face_mode = renderer::FM_FRONTANDBACK;
		CopyScaled->passinfo[0].b_depthtest = false;
	}

	bSpecialRender_ResetLights = false;

	// Create the debug tools
	new debug::CDebugDrawer;
	new debug::CDebugRTInspector;

	// Create the passes for rendering the screen:
	{
		LightingPass = new RrMaterial();
		// Setup forward pass
		LightingPass->passinfo.push_back( RrPassForward() );
		LightingPass->passinfo[0].shader = new RrShader( "shaders/def_screen/pass_lighting.glsl" );
		LightingPass->passinfo[0].m_face_mode = renderer::FM_FRONTANDBACK;
	}
	{
		EchoPass = new RrMaterial();
		// Setup forward pass
		EchoPass->passinfo.push_back( RrPassForward() );
		EchoPass->passinfo[0].shader = new RrShader( "shaders/def_screen/pass_lighting_echo.glsl" );
		EchoPass->passinfo[0].m_face_mode = renderer::FM_FRONTANDBACK;
	}
	{
		ShaftPass = new RrMaterial();
		// Setup forward pass
		ShaftPass->passinfo.push_back( RrPassForward() );
		ShaftPass->passinfo[0].shader = new RrShader( "shaders/def_screen/pass_lighting_shaft.glsl" );
		ShaftPass->passinfo[0].m_face_mode = renderer::FM_FRONTANDBACK;
		// Set effect textures
		ShaftPass->setTexture( TEX_SLOT5, new CTexture( "textures/ditherdots.jpg" ) );
	}
	{
		Lighting2DPass = new RrMaterial();
		// Setup forward pass
		Lighting2DPass->passinfo.push_back( RrPassForward() );
		Lighting2DPass->passinfo[0].shader = new RrShader( "shaders/def_screen/pass_lighting2d.glsl" );
		Lighting2DPass->passinfo[0].m_face_mode = renderer::FM_FRONTANDBACK;
	}

}

// Class destructor
//  sets pActive pointer to null
//  frees list of renderers, but not the renderers
CRenderState::~CRenderState ( void )
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
	while ( RrMaterial::Default->hasReference() ) 
		RrMaterial::Default->removeReference();
	delete RrMaterial::Default;
	RrMaterial::Default = NULL;

	RrMaterial::Copy->removeReference();
	delete RrMaterial::Copy;
	RrMaterial::Copy = NULL;

	RrMaterial::Fallback->removeReference();
	delete RrMaterial::Fallback;
	RrMaterial::Fallback = NULL;

	// Free the other materials
	CopyScaled->removeReference();
	delete_safe(CopyScaled);
	LightingPass->removeReference();
	delete_safe(LightingPass);
	EchoPass->removeReference();
	delete_safe(EchoPass);
	ShaftPass->removeReference();
	delete_safe(ShaftPass);
	Lighting2DPass->removeReference();
	delete_safe(Lighting2DPass);

	// Stop the resource manager
	if ( mResourceManager && mResourceManager->m_renderStateOwned ) {
		mResourceManager->m_renderStateOwned = false;
		delete mResourceManager;
		mResourceManager = NULL;
	}
}


//-Adding and Removing Renderable/Logic Objects-

// AddRO ( pointer to new RO )
//  adds an RO to be drawn and returns the index
//  used only by the RO constructor
unsigned int CRenderState::AddRO ( CRenderableObject * pRO )
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
void CRenderState::RemoveRO ( unsigned int id )
{
	pRenderableObjects[id] = NULL;
}

// AddLO ( pointer to new RO )
//  adds an LO to be drawn and returns the index
//  used only by the LO constructor
unsigned int CRenderState::AddLO ( CLogicObject * pLO )
{
	// Randomly reshift the logic objects
	if ( (rand()%10) == 0 )
	{
		CLogicObject* tmpLO;
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
void CRenderState::RemoveLO ( unsigned int id )
{
	// TODO: Optimize this
	Jobs::System::Current::WaitForJobs( Jobs::JOBTYPE_RENDERSTEP );

	// Now set to null now that the object is pretty much gone
	mLogicObjects[id] = NULL;
}


// Rendering configuration
// ================================

// Returns the material used for rendering a screen's pass in the given effect
RrMaterial* CRenderState::GetScreenMaterial ( const eRenderMode mode, const renderer::eSpecialModes mode_type )
{
	if ( mode == RENDER_MODE_DEFERRED )
	{
		switch (mode_type)
		{
		case renderer::SP_MODE_NORMAL: return LightingPass;
		case renderer::SP_MODE_ECHO: return EchoPass;
		case renderer::SP_MODE_SHAFT: return ShaftPass;
		case renderer::SP_MODE_2DPALETTE: return Lighting2DPass;
		}
	}
	return NULL;
}

// Settings and query
// ================================

// Returns internal settings that govern the current render setup
const renderer::internalSettings_t& CRenderState::GetSettings ( void ) const
{
	return internal_settings;
}

// Returns true if settings dropped. False otherwise.
static bool _DropSettings (renderer::internalSettings_t& io_settings)
{
	if (io_settings.mainStencilFormat == StencilIndex16) 
	{
		debug::Console->PrintError("Dropping Stencil16 to Stencil8.");
		io_settings.mainStencilFormat = StencilIndex8;
		return true;
	}

	if (io_settings.mainDepthFormat == Depth32) 
	{
		debug::Console->PrintError("Dropping Depth32 to Depth16.");
		io_settings.mainDepthFormat = Depth16;
		return true;
	}

	if (io_settings.mainStencilFormat == StencilIndex8) 
	{
		debug::Console->PrintError("Dropping Stencil8 to None. (This may cause visual artifacts!)");
		io_settings.mainStencilFormat = StencilNone;
		return true;
	}

	debug::Console->PrintError("Could not downgrade screen buffer settings.");

	// Couldn't drop any settings.
	return false;
}

void CRenderState::CreateTargetBuffers ( void )
{
	if (internal_chain_list.empty())
	{
		rrInternalBufferChain chain = {0};
		// Three backbuffers
		internal_chain_list.push_back(chain);
		internal_chain_list.push_back(chain);
		internal_chain_list.push_back(chain);
		// Set current buffer to first
		internal_chain_current = &internal_chain_list[0];
	}
	for (rrInternalBufferChain& chain : internal_chain_list)
	{
		bool status = CreateTargetBufferChain(chain);
		if (status == false)
		{
			// There was an error in creating the target buffer chain. We need to break, try another set of formats, then continue.
			if (_DropSettings(internal_settings))
			{
				debug::Console->PrintError("Screen buffer formats not supported. Dropping settings and attempting again.");
				// Attempt to create again.
				CreateTargetBuffers();
				// Stop the loop.
				break; 
			}
			else
			{
				debug::Console->PrintError("Screen buffer formats not supported. Throwing an unsupported error.");
				throw core::DeprecatedFeatureException();
			}
		}
	}
}
bool CRenderState::CreateTargetBufferChain ( rrInternalBufferChain& bufferChain )
{
	// Delete forward buffers
	if ( bufferChain.buffer_forward_rt != NULL )
	{
		delete bufferChain.buffer_forward_rt;
		bufferChain.buffer_forward_rt = NULL;

		gpu::TextureFree( bufferChain.buffer_depth );
		bufferChain.buffer_depth = 0;

		gpu::TextureBufferFree( bufferChain.buffer_stencil );
		bufferChain.buffer_stencil = 0;
	}
	// Delete deferred buffers
	if ( bufferChain.buffer_deferred_mrt != NULL )
	{
		delete bufferChain.buffer_deferred_mrt;
		bufferChain.buffer_deferred_mrt = NULL;

		delete bufferChain.buffer_deferred_rt;
		bufferChain.buffer_deferred_rt = NULL;
	}

	// Create forward buffers
	if ( bufferChain.buffer_forward_rt == NULL )
	{
		// Generate shared depth and stencil buffers
		if ( internal_settings.mainDepthFormat != DepthNone )
		{
			bufferChain.buffer_depth	= gpu::TextureAllocate( Texture2D, internal_settings.mainDepthFormat, Screen::Info.width, Screen::Info.height );
			gpu::TextureSampleSettings( Texture2D, bufferChain.buffer_depth, Clamp, Clamp, Clamp, SamplingPoint, SamplingPoint );
		}
		if ( internal_settings.mainStencilFormat != StencilNone )
			bufferChain.buffer_stencil	= gpu::TextureBufferAllocate( Texture2D, internal_settings.mainStencilFormat, Screen::Info.width, Screen::Info.height );

		bufferChain.buffer_forward_rt = new CRenderTexture(
			Screen::Info.width, Screen::Info.height,
			Clamp, Clamp,
			internal_settings.mainColorAttachmentFormat,
			RrGpuTexture(bufferChain.buffer_depth, internal_settings.mainDepthFormat), internal_settings.mainDepthFormat != DepthNone,
			RrGpuTexture(bufferChain.buffer_stencil, internal_settings.mainStencilFormat), false
		);

		// Check to make sure buffer is valid.
		if (!bufferChain.buffer_forward_rt->IsValid())
		{
			return false;
		}
	}
	// Create deferred buffers
	if ( bufferChain.buffer_deferred_mrt == NULL )
	{
		// Create the internal stage color render target (uses shared forward buffers)
		bufferChain.buffer_deferred_rt = new CRenderTexture(
			Screen::Info.width, Screen::Info.height,
			Clamp, Clamp,
			internal_settings.mainColorAttachmentFormat,
			RrGpuTexture(bufferChain.buffer_depth, internal_settings.mainDepthFormat), internal_settings.mainDepthFormat != DepthNone,
			RrGpuTexture(bufferChain.buffer_stencil, internal_settings.mainStencilFormat), false
		);

		RrGpuTexture		depthTexture = RrGpuTexture(bufferChain.buffer_depth, internal_settings.mainDepthFormat);
		RrGpuTexture		stencilTexture = RrGpuTexture(bufferChain.buffer_stencil, internal_settings.mainStencilFormat);

		// TODO: Make configurable
		RrGpuTexture textureRequests [4];
		memset( textureRequests, 0, sizeof(RrGpuTexture) * 4 );
		textureRequests[0].format = RGBA8;
		textureRequests[1].format = RGBA16F;
		textureRequests[2].format = RGBA8;
		textureRequests[3].format = RGBA8;

		// Create the MRT to be used by the rendering pipeline
		bufferChain.buffer_deferred_mrt = new CMRTTexture(
			Screen::Info.width, Screen::Info.height,
			Clamp, Clamp,
			textureRequests + 0, 4,
			&depthTexture, depthTexture.format != DepthNone,
			&stencilTexture, false
		);

		// Check to make sure buffer is valid.
		if (!bufferChain.buffer_deferred_mrt->IsValid())
		{
			return false;
		}
	}
	// Made it here, so return success.
	return true;
}

CRenderTexture* CRenderState::GetForwardBuffer ( void )
{
	if (internal_chain_current)
		return internal_chain_current->buffer_forward_rt;
	return NULL;
}
CRenderTexture* CRenderState::GetDeferredBuffer ( void )
{
	if (internal_chain_current)
		return internal_chain_current->buffer_deferred_rt;
	return NULL;
}

RrGpuTexture CRenderState::GetDepthTexture ( void )
{
	if (internal_chain_current)
		return RrGpuTexture( internal_chain_current->buffer_depth, internal_settings.mainDepthFormat );
	return RrGpuTexture();
}
RrGpuTexture CRenderState::GetStencilTexture ( void )
{
	if (internal_chain_current)
		return RrGpuTexture( internal_chain_current->buffer_stencil, internal_settings.mainStencilFormat );
	return RrGpuTexture();
}