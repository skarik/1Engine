// Game Renderer class
// Does not take pointer ownership of renderable objects

#ifndef _C_TO_BE_SEEN_H_
#define _C_TO_BE_SEEN_H_

#include "core/types/types.h"
#include "core/containers/arstring.h"
#include "renderer/types/types.h"
#include "renderer/types/ObjectSettings.h"
#include "renderer/types/RrGpuTexture.h"
#include "renderer/state/InternalSettings.h"

//Todo: make following prototypes
#include "renderer/gpuw/Textures.h"
#include "renderer/gpuw/RenderTargets.h"
#include "renderer/gpuw/Device.h"
#include "renderer/gpuw/GraphicsContext.h"

#include <vector>

class CRenderableObject;
class CLogicObject;
class CLight;
class CCamera;
class CResourceManager;
class RrMaterial;
class RrRenderTexture;
class CMRTTexture;

//	CRenderState : main render state & swapchain manager class
class CRenderState
{
public:
	// Structure Definitions
	// ================================

	//	tRenderRequest : Render request
	struct tRenderRequest
	{
		CRenderableObject*	obj;
		char				pass;
		bool				transparent;
		bool				forward;
		bool				screenshader;
		uchar				renderType;

		// Default constructor
		tRenderRequest ( void ) :
			obj(NULL), pass(0), transparent(false), forward(true), screenshader(false), renderType(2)
		{ ; }
	};

	//	tReplacementRule : Render material replacement rule
	// Material settings must be done before the Render() call
	struct tReplacementRule
	{
		arstring<32>	hintToReplace;
		RrMaterial*		materialToUse;
	};

	enum rrConstants : int
	{
		kMRTColorAttachmentCount = 4,
	};

	//	rrInternalBufferChain : internal storage of buffers used for rendering
	struct rrInternalBufferChain
	{
		//glHandle		buffer_depth;
		//glHandle		buffer_stencil;
		gpu::Texture	buffer_depth;
		gpu::WOFrameAttachment	buffer_stencil;
		//RrRenderTexture*	buffer_forward_rt;
		gpu::Texture		buffer_color;
		gpu::RenderTarget	buffer_forward_rt;

		//CMRTTexture*	buffer_deferred_mrt;
		//RrRenderTexture*	buffer_deferred_rt;
		gpu::Texture		buffer_deferred_color_composite;
		gpu::Texture		buffer_deferred_color[kMRTColorAttachmentCount];
		gpu::RenderTarget	buffer_deferred_mrt;
		gpu::RenderTarget	buffer_deferred_rt;
	};

public:
	// Constructor and Destructor
	// ================================

	RENDER_API CRenderState ( CResourceManager* nResourceManager );
	RENDER_API ~CRenderState ( void );


	// Buffer management
	// ================================

	// Creates buffers for rendering to.
	RENDER_API void CreateTargetBuffers ( void );
	// Recreates buffers for the given chain. Returns success.
	RENDER_API bool CreateTargetBufferChain ( rrInternalBufferChain& bufferChain );
	RENDER_API gpu::RenderTarget* GetForwardBuffer ( void );
	RENDER_API gpu::RenderTarget* GetDeferredBuffer ( void );
	RENDER_API gpu::Texture* GetDepthTexture ( void );
	RENDER_API gpu::WOFrameAttachment* GetStencilTexture ( void );


	// Public Render routine
	// ================================

	RENDER_API void Render ( void );


	// Full Scene Rendering Routines
	// ================================

	// Normal rendering routine.
	RENDER_API void RenderSceneForward ( const uint32_t n_renderHint );
	// Deferred rendering routine.
	RENDER_API void RenderSceneDeferred ( const uint32_t n_renderHint );


	// Specialized Rendering Routines
	// ================================

	RENDER_API void PreRenderSetLighting ( std::vector<CLight*> & lightsToUse );
	// RenderSingleObject renders an object, assuming the projection has been already set up.
	RENDER_API void RenderSingleObject ( CRenderableObject* objectToRender );
	// RenderObjectArray() renders a null terminated list of objects, assuming the projection has been already set up.
	RENDER_API void RenderObjectArray ( CRenderableObject** objectsToRender );
	//	SetPipelineMode - Sets new pipeline mode.
	// Calling this has the potential to be very slow and completely invalidate rendering state.
	// This should be called as soon as possible to avoid any slowdown.
	RENDER_API void SetPipelineMode ( renderer::ePipelineMode newPipelineMode );


	// Rendering configuration
	// ================================

	// Returns the material used for rendering a screen's pass in the given effect
	RENDER_API RrMaterial* GetScreenMaterial ( const eRenderMode mode, const renderer::ePipelineMode mode_type );


	// Settings and query
	// ================================

	// Returns internal settings that govern the current render setup
	RENDER_API const renderer::internalSettings_t& GetSettings ( void ) const;
	// Returns if shadows are enabled for the renderer or not.
	RENDER_API const bool GetShadowsEnabled ( void ) const;
	// Returns current pipeline information
	RENDER_API const eRenderMode GetRenderMode ( void ) const;
	// Returns current pipeline mode (previously, special render type)
	RENDER_API const renderer::ePipelineMode GetPipelineMode ( void ) const;

private:
	bool bSpecialRender_ResetLights;
	std::vector<CLight*> vSpecialRender_LightList;

public:
	// Public active instance pointer
	RENDER_API static CRenderState* Active;
	// Resource manager
	CResourceManager*	mResourceManager;

	// Device & context
	gpu::Device*			mDevice;
	gpu::GraphicsContext*	mGfxContext;
	gpu::ComputeContext*	mComputeContext;

private:
	// Renderable object access and management
	// ================================

	// Give RO constructor and destructor access to adding and removing
	friend CRenderableObject;
	// Adding and removing renderable objects
	unsigned int AddRO ( CRenderableObject* );
	void RemoveRO ( unsigned int );
	void CleanROList ( void );
	// Reorder the render list
	void ReorderList ( void );

	// Give RO constructor and destructor access to adding and removing
	friend CLogicObject;
	// Adding and removing renderable objects
	unsigned int AddLO ( CLogicObject* );
	void RemoveLO ( unsigned int );

private:
	// Render list
	// ================================
	std::vector<CRenderableObject*> pRenderableObjects;
	unsigned int iCurrentIndex;
	unsigned int iListSize;

	// Render list sorting
	// ================================
	struct structRenderComparison_old {
		bool operator() ( CRenderableObject* i, CRenderableObject* j);
	} RenderOrderComparison_old;
	struct render_forward_comparator_t {
		bool operator() ( tRenderRequest& i, tRenderRequest& j);
	} OrderComparatorForward;
	struct render_deferred_comparator_t {
		bool operator() ( tRenderRequest& i, tRenderRequest& j);
	} OrderComparatorDeferred;

	// Render state
	// ================================
	CCamera*				mainBufferCamera;
	bool					bufferedMode;
	bool					shadowMode;
	eRenderMode				renderMode;
	renderer::ePipelineMode	pipelineMode;

	// Logic list
	// ================================
	std::vector<CLogicObject*>	mLogicObjects;
	unsigned int				mLoCurrentIndex;
	unsigned int				mLoListSize;

	// Internal setup state
	// ================================
	renderer::internalSettings_t		internal_settings;
	std::vector<rrInternalBufferChain>	internal_chain_list;
	rrInternalBufferChain*				internal_chain_current;
	uint								internal_chain_index;

	// Deferred pass materials
	// ================================
	RrMaterial*	CopyScaled;
	RrMaterial* LightingPass;
	RrMaterial* EchoPass;
	RrMaterial* ShaftPass;
	RrMaterial* Lighting2DPass;
};

// Global instance
RENDER_API extern CRenderState* SceneRenderer;

#endif//_C_TO_BE_SEEN_H_