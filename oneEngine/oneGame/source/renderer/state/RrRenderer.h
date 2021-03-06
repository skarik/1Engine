#ifndef RENDERER_RENDER_STATE_SYSTEM_H_
#define RENDERER_RENDER_STATE_SYSTEM_H_

#include "core/types/types.h"
#include "core/containers/arstring.h"
#include "renderer/types/types.h"
#include "renderer/types/ObjectSettings.h"
#include "renderer/types/RrGpuTexture.h"
#include "renderer/state/InternalSettings.h"
#include "renderer/state/RrHybridBufferChain.h"

//Todo: make following prototypes
#include "gpuw/Texture.h"
#include "gpuw/RenderTarget.h"
#include "gpuw/Device.h"
#include "gpuw/GraphicsContext.h"

#include <vector>

class CRenderableObject;
class RrLogicObject;
class RrLight;
class RrCamera;
class CResourceManager;
class RrPass;
class RrRenderTexture;
class CMRTTexture;
struct rrCameraPass;
namespace renderer
{
	namespace pipeline
	{
		class RrPipelinePasses;
	}
}

//	RrRenderer : main render state & swapchain manager class
class RrRenderer
{
public:
	// Structure Definitions
	// ================================

	//	rrRenderRequest : Render request
	struct rrRenderRequest
	{
		CRenderableObject*	obj;
		uint8_t				pass;
		//bool				transparent;
		//bool				forward;
		//bool				screenshader;
		//uchar				renderLayer;

		// Default constructor
		/*rrRenderRequest ( void ) :
			obj(NULL), pass(0)//, transparent(false), forward(true), screenshader(false), renderLayer(2)
		{ ; }*/
	};

	//	tReplacementRule : Render material replacement rule
	// Material settings must be done before the Render() call
	struct tReplacementRule
	{
		arstring<32>		hintToReplace;
		RrMaterial*			materialToUse;
	};

	/*enum rrConstants : int
	{
		kMRTColorAttachmentCount = 4,
	};

	//	rrInternalBufferChain : internal storage of buffers used for rendering
	struct rrInternalBufferChain
	{
		//glHandle		buffer_depth;
		//glHandle		buffer_stencil;
		gpu::Texture		buffer_depth;
		gpu::WOFrameAttachment
							buffer_stencil;
		//RrRenderTexture*	buffer_forward_rt;
		gpu::Texture		buffer_color;
		gpu::RenderTarget	buffer_forward_rt;

		//CMRTTexture*	buffer_deferred_mrt;
		//RrRenderTexture*	buffer_deferred_rt;
		gpu::Texture		buffer_deferred_color_composite;
		gpu::Texture		buffer_deferred_color[kMRTColorAttachmentCount];
		gpu::RenderTarget	buffer_deferred_mrt;
		gpu::RenderTarget	buffer_deferred_rt;
	};*/

public:
	// Constructor, Destructor, and Initialization
	// ================================

	RENDER_API explicit		RrRenderer ( CResourceManager* nResourceManager );
	RENDER_API				~RrRenderer ( void );

	void					InitializeWithDeviceAndSurface ( gpu::Device* device, gpu::OutputSurface* surface );
	void					ResizeSurface ( void );

	// Buffer management
	// ================================

	// Creates buffers for rendering to.
	RENDER_API void			CreateTargetBuffers ( void );
	// Recreates buffers for the given chain. Returns success.
	//RENDER_API bool			CreateTargetBufferChain ( rrInternalBufferChain& bufferChain );
	RENDER_API gpu::RenderTarget*
							GetForwardBuffer ( void );
	RENDER_API gpu::RenderTarget*
							GetDeferredBuffer ( void );
	RENDER_API gpu::Texture*
							GetDepthTexture ( void );
	RENDER_API gpu::WOFrameAttachment*
							GetStencilTexture ( void );


	// Public Render routine
	// ================================

	// Performs internal render loop
	RENDER_API void			Render ( void );

	// object state update
	void					StepPreRender ( void );
	void					StepBufferPush ( void );
	void					StepPostRender ( void );


	// Full Scene Rendering Routines
	// ================================

	// Normal rendering routine.
	//void RenderSceneForward ( const uint32_t n_renderHint );
	// Deferred rendering routine.
	//void RenderSceneDeferred ( const uint32_t n_renderHint );

	//	RenderScene () : Renders the main scene from the given camera with DeferredForward+.
	// Is shorthand for ``RenderObjectList(m_renderableObjects)``.
	RENDER_API void			RenderScene ( RrCamera* camera );
	//	RenderObjectList () : Renders the object list from the given camera with DeferredForward+.
	RENDER_API void			RenderObjectList ( RrCamera* camera, CRenderableObject** objectsToRender, const uint32_t objectCount );

	RENDER_API void			RenderObjectListWorld ( rrCameraPass* cameraPass, CRenderableObject** objectsToRender, const uint32_t objectCount );
	RENDER_API void			RenderObjectListShadows ( rrCameraPass* cameraPass, CRenderableObject** objectsToRender, const uint32_t objectCount );

	// Specialized Rendering Routines
	// ================================

	//	PreRenderBeginLighting() : Sets up rendering with the given light list.
	// Think very carefully if this should be used, versus scheduling draws directly on the GPU.
	// Most meshes will generate correct internal state on draw, regardless. This allows you to render them onto the current active graphics queue.
	/*RENDER_API void			PreRenderBeginLighting ( std::vector<RrLight*> & lightsToUse );
	//	RenderSingleObject() : renders an object, assuming the projection has been already set up.
	// Think very carefully if this should be used, versus scheduling draws directly on the GPU.
	// Most meshes will generate correct internal state on draw, regardless. This allows you to render them onto the current active graphics queue.
	// This call adds setting up the cbuffers, forcing a certain set of lights.
	// It should be noted the DeferredForward+ pipeline will not work without a Forward RrPass.
	RENDER_API void			RenderSingleObject ( CRenderableObject* objectToRender );
	//	RenderObjectArray() : renders a null terminated list of objects, assuming the projection has been already set up.
	// Think very carefully if this should be used, versus scheduling draws directly on the GPU.
	// Most meshes will generate correct internal state on draw, regardless. This allows you to render them onto the current active graphics queue.
	// This call adds setting up the cbuffers, forcing a certain set of lights.
	// It should be noted the DeferredForward+ pipeline will not work without a Forward RrPass.
	RENDER_API void			RenderObjectArray ( CRenderableObject** objectsToRender );*/

	//	SetPipelineMode - Sets new pipeline mode.
	// Calling this has the potential to be very slow and completely invalidate rendering state.
	// This should be only be called from the GameState, when the pipeline is at no risk of being corrupted.
	RENDER_API void			SetPipelineMode ( renderer::ePipelineMode newPipelineMode );


	// Rendering configuration
	// ================================

	// Returns the material used for rendering a screen's pass in the given effect
	RENDER_API RrPass*		GetScreenMaterial ( const eRenderMode mode, const renderer::ePipelineMode mode_type );


	// Settings and query
	// ================================

	// Returns internal settings that govern the current render setup
	RENDER_API const renderer::rrInternalSettings&
							GetSettings ( void ) const;
	// Returns if shadows are enabled for the renderer or not.
	RENDER_API const bool	GetShadowsEnabled ( void ) const;
	// Returns current pipeline information
	//RENDER_API const eRenderMode
	//						GetRenderMode ( void ) const;
	// Returns current pipeline mode (previously, special render type)
	RENDER_API const renderer::ePipelineMode
							GetPipelineMode ( void ) const;

private:
	bool bSpecialRender_ResetLights;
	std::vector<RrLight*> vSpecialRender_LightList;

public:
	// Public active instance pointer
	RENDER_API static RrRenderer* Active;

	// Device & context
	gpu::Device*			mDevice;
	gpu::OutputSurface*		mOutputSurface;
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
	friend RrLogicObject;
	// Adding and removing renderable objects
	unsigned int AddLO ( RrLogicObject* );
	void RemoveLO ( unsigned int );

private:
	// Render list
	// ================================
	std::vector<CRenderableObject*>
							pRenderableObjects;
	unsigned int			iCurrentIndex;
	unsigned int			iListSize;

	// Render list sorting
	// ================================
	/*struct structRenderComparison_old {
		bool operator() ( CRenderableObject* i, CRenderableObject* j);
	} RenderOrderComparison_old;
	struct render_forward_comparator_t {
		bool operator() ( tRenderRequest& i, tRenderRequest& j);
	} OrderComparatorForward;
	struct render_deferred_comparator_t {
		bool operator() ( tRenderRequest& i, tRenderRequest& j);
	} OrderComparatorDeferred;*/

	struct rrRenderRequestSorter
	{
		bool operator() ( rrRenderRequest& i, rrRenderRequest& j );
	}
	RenderRequestSorter;

	// Render state
	// ================================
	RrCamera*				mainBufferCamera;
	bool					bufferedMode;
	bool					shadowMode;
	//eRenderMode				renderMode;
	renderer::ePipelineMode	pipelineMode;

	// Logic list
	// ================================
	std::vector<RrLogicObject*>
							mLogicObjects;
	unsigned int			mLoCurrentIndex;
	unsigned int			mLoListSize;

	// Internal setup state
	// ================================
	renderer::rrInternalSettings
							internal_settings;
	// List of buffer targets that are used to render to.
	std::vector<RrHybridBufferChain>
							internal_chain_list;
	// Current buffer target being recorded/rendered to.
	RrHybridBufferChain*	internal_chain_current;
	// Index of the buffer target being recorded/rendered to.
	uint					internal_chain_index;

	// Per-frame constant buffers. See renderer::cbuffer::rrPerFrame.
	// Each index refers directly to a buffer target of internal_chain_list.
	// Ex: If the engine is using triple (3) buffering, there will be three (3) cbuffers.
	std::vector<gpu::Buffer>
							internal_cbuffers_frames;
	// Per-pass constant buffers. See renderer::cbuffer:rrPerPass.
	// Each group of kRenderLayer_MAX refers to a layer used in a buffer target of internal_chain_list.
	// Ex: If kRenderLayer_MAX is 7, and the engine is using triple (3) buffering, there will be 21 cbuffers.
	std::vector<gpu::Buffer>
							internal_cbuffers_passes;

	// Deferred pass materials
	// ================================
	renderer::pipeline::RrPipelinePasses*
							pipelinePasses;

};

// Global active instance
RENDER_API extern RrRenderer* SceneRenderer;

#endif//RENDERER_RENDER_STATE_SYSTEM_H_