
// Includes
#include "CRenderState.h"
#include "Settings.h"

#include "core/system/Screen.h"

#include "renderer/object/CRenderableObject.h"
#include "renderer/logic/CLogicObject.h"

#include "renderer/material/glMaterial.h"
#include "renderer/texture/CTexture.h"
#include "renderer/texture/CRenderTexture.h"

#include "renderer/debug/CDebugDrawer.h"
#include "renderer/debug/CDebugRTInspector.h"
#include "renderer/object/sprite/CSpriteContainer.h"

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
		internal_settings.mainDepthFormat = Depth24;
		internal_settings.mainStencilFormat = StencilIndex8;
	}
	// Set initial rendertarget states
	{
		internal_buffer_forward_rt = NULL;
		internal_buffer_depth = 0;
		internal_buffer_stencil = 0;
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
	
	// Create the default material
	if ( glMaterial::Default == NULL )
	{
		// The default material must be a single pass in both modes.
		// This to provide compatibility with the default system implementation. (A lot of early engine code is implemented lazily)
		glMaterial::Default = new glMaterial;
		glMaterial::Default->setTexture( 0, new CTexture( "textures/white.jpg" ) );			// Diffuse
		glMaterial::Default->setTexture( 1, new CTexture( "textures/black.jpg" ) );			// Glow
		glMaterial::Default->setTexture( 2, new CTexture( "textures/default_specular.jpg" ) );	// Specular
		glMaterial::Default->setTexture( 3, new CTexture( "textures/default_normals.jpg" ) );	// Normals
		// Setup forward pass
		glMaterial::Default->passinfo.push_back( glPass() );
		glMaterial::Default->passinfo[0].shader = new glShader( "shaders/d/diffuse.glsl" );
		// Setup deferred pass
		glMaterial::Default->deferredinfo.push_back( glPass_Deferred() );
	}
	if ( glMaterial::Copy == NULL )
	{
		glMaterial::Copy = new glMaterial;
		// Setup forward pass
		glMaterial::Copy->passinfo.push_back( glPass() );
		glMaterial::Copy->passinfo[0].shader = new glShader( "shaders/sys/copy_buffer.glsl" );
		glMaterial::Copy->passinfo[0].m_face_mode = Renderer::FM_FRONTANDBACK;
		// Setup deferred pass
		// Hilariously, this one doesn't use a deferred pass.
	}
	// Create the fallback shader
	if ( glMaterial::Fallback == NULL )
	{
		glMaterial::Fallback = new glMaterial;
		// Setup forward pass
		glMaterial::Fallback->passinfo.push_back( glPass() );
		glMaterial::Fallback->passinfo[0].shader = new glShader( "shaders/sys/fullbright.glsl" );
		// Setup deferred pass
		glMaterial::Fallback->deferredinfo.push_back( glPass_Deferred() );
	}
	// Create the default hint options
	if ( Renderer::m_default_hint_options == NULL )
	{
		Renderer::m_default_hint_options = new Renderer::_n_hint_rendering_information();
	}
	bSpecialRender_ResetLights = false;

	// Create the debug tools
	new Debug::CDebugDrawer;
	new Debug::CDebugRTInspector;
	// Create the sprite renderer
	new SpriteContainer;

	// Create the passes for rendering the screen:
	{
		LightingPass = new glMaterial();
		// Setup forward pass
		LightingPass->passinfo.push_back( glPass() );
		LightingPass->passinfo[0].shader = new glShader( "shaders/def_screen/pass_lighting.glsl" );
		LightingPass->passinfo[0].m_face_mode = Renderer::FM_FRONTANDBACK;
	}
	{
		EchoPass = new glMaterial();
		// Setup forward pass
		EchoPass->passinfo.push_back( glPass() );
		EchoPass->passinfo[0].shader = new glShader( "shaders/def_screen/pass_lighting_echo.glsl" );
		EchoPass->passinfo[0].m_face_mode = Renderer::FM_FRONTANDBACK;
	}
	{
		ShaftPass = new glMaterial();
		// Setup forward pass
		ShaftPass->passinfo.push_back( glPass() );
		ShaftPass->passinfo[0].shader = new glShader( "shaders/def_screen/pass_lighting_shaft.glsl" );
		ShaftPass->passinfo[0].m_face_mode = Renderer::FM_FRONTANDBACK;
		// Set effect textures
		ShaftPass->setTexture( 5, new CTexture( "textures/ditherdots.jpg" ) );
	}
	{
		Lighting2DPass = new glMaterial();
		// Setup forward pass
		Lighting2DPass->passinfo.push_back( glPass() );
		Lighting2DPass->passinfo[0].shader = new glShader( "shaders/def_screen/pass_lighting2d.glsl" );
		Lighting2DPass->passinfo[0].m_face_mode = Renderer::FM_FRONTANDBACK;
	}

}

// Class destructor
//  sets pActive pointer to null
//  frees list of renderers, but not the renderers
CRenderState::~CRenderState ( void )
{
	// Delete the sprite renderer
	delete SpriteContainer::Active;
	// Delete the debug tools
	delete Debug::Drawer;
	delete Debug::RTInspector;

	// Free state info
	Active			= NULL;
	SceneRenderer	= NULL;

	// Reset arrays
	iCurrentIndex = 0;
	iListSize = 0;

	// Free system default material
	while ( glMaterial::Default->hasReference() ) 
		glMaterial::Default->removeReference();
	delete glMaterial::Default;
	glMaterial::Default = NULL;

	glMaterial::Copy->removeReference();
	delete glMaterial::Copy;
	glMaterial::Copy = NULL;

	glMaterial::Fallback->removeReference();
	delete glMaterial::Fallback;
	glMaterial::Fallback = NULL;

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
	mLogicObjects[id] = NULL;
}


// Rendering configuration
// ================================

// Returns the material used for rendering a screen's pass in the given effect
glMaterial* CRenderState::GetScreenMaterial ( const eRenderMode mode, const Renderer::eSpecialModes mode_type )
{
	if ( mode == RENDER_MODE_DEFERRED )
	{
		switch (mode_type)
		{
		case Renderer::SP_MODE_NORMAL: return LightingPass;
		case Renderer::SP_MODE_ECHO: return EchoPass;
		case Renderer::SP_MODE_SHAFT: return ShaftPass;
		case Renderer::SP_MODE_2DPALETTE: return Lighting2DPass;
		}
	}
	return NULL;
}

// Settings and query
// ================================

// Returns internal settings that govern the current render setup
const Renderer::internalSettings_t& CRenderState::GetSettings ( void ) const
{
	return internal_settings;
}




void CRenderState::CreateBuffer ( void )
{
	if ( internal_buffer_forward_rt != NULL )
	{
		delete internal_buffer_forward_rt;
		internal_buffer_forward_rt = NULL;

		GPU::TextureFree( internal_buffer_depth );
		internal_buffer_depth = 0;

		GPU::TextureFree( internal_buffer_stencil );
		internal_buffer_stencil = 0;
	}
	if ( internal_buffer_forward_rt == NULL )
	{
		if ( internal_settings.mainDepthFormat != DepthNone )
			internal_buffer_depth	= GPU::TextureAllocate( Texture2D, internal_settings.mainDepthFormat, Screen::Info.width, Screen::Info.height );
		if ( internal_settings.mainStencilFormat != StencilNone )
			internal_buffer_stencil	= GPU::TextureAllocate( Texture2D, internal_settings.mainStencilFormat, Screen::Info.width, Screen::Info.height );

		internal_buffer_forward_rt = new CRenderTexture(
			Screen::Info.width, Screen::Info.height,
			Clamp, Clamp,
			internal_settings.mainColorAttachmentFormat,
			glTexture(internal_buffer_depth, internal_settings.mainDepthFormat), internal_settings.mainDepthFormat != DepthNone,
			glTexture(internal_buffer_stencil, internal_settings.mainStencilFormat), internal_settings.mainStencilFormat != StencilNone
		);
	}
}

CRenderTexture* CRenderState::GetForwardBuffer ( void )
{
	return internal_buffer_forward_rt;
}

glTexture CRenderState::GetDepthTexture ( void )
{
	return glTexture( internal_buffer_depth, internal_settings.mainDepthFormat );
}
glTexture CRenderState::GetStencilTexture ( void )
{
	return glTexture( internal_buffer_stencil, internal_settings.mainStencilFormat );
}