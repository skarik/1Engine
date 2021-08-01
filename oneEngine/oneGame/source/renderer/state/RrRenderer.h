#ifndef RENDERER_RENDER_STATE_SYSTEM_H_
#define RENDERER_RENDER_STATE_SYSTEM_H_

#include "core/types/types.h"
#include "core/containers/arstring.h"
#include "renderer/types/types.h"
#include "renderer/types/ObjectSettings.h"
#include "renderer/types/RrGpuTexture.h"
#include "renderer/types/id.h"
#include "renderer/types/viewport.h"
#include "renderer/state/InternalSettings.h"
#include "renderer/state/RrHybridBufferChain.h"

//Todo: make following prototypes
#include "gpuw/Texture.h"
#include "gpuw/RenderTarget.h"
#include "gpuw/Device.h"
#include "gpuw/GraphicsContext.h"
#include "gpuw/Buffers.h"

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
class RrWindow;
class RrRenderer;
namespace renderer
{
	namespace pipeline
	{
		class RrPipelinePasses;
	}
}

class RrPipelineOptions;

//	class RrWorld : A container of objects that can be rendered.
class RrWorld
{
public:
	explicit				RrWorld ( void )
		{}
							~RrWorld ( void )
		{}

	friend CRenderableObject;
	rrId					AddObject ( CRenderableObject* renderable );
	bool					RemoveObject ( CRenderableObject* renderable );
	bool					RemoveObject ( const rrId& renderable_id );

	friend RrLogicObject;
	rrId					AddLogic ( RrLogicObject* logic );
	bool					RemoveLogic ( RrLogicObject* logic );
	bool					RemoveLogic ( const rrId& logic_id );

	void					FrameUpdate ( void );

private:
	struct rrWorldState
	{
		enum WorkStep
		{
			kWorkStepSortObjects = 0,
			kWorkStepSortLogics = 1,
			kWorkStepCompactObjects = 2,
			kWorkStepCompactLogics = 3,

			kWorkStep_MAX,
		};

		WorkStep		work_step = kWorkStepSortObjects;
	};

	rrWorldState			state;

	void					CompactObjectListing ( void );
	void					SortObjectListing ( void );

	void					CompactLogicListing ( void ) {}
	void					SortLogicListing ( void ) {}

public:
	renderer::ePipelineMode
						pipeline_mode = renderer::kPipelineModeNormal;
	RrPipelineOptions*	pipeline_options = nullptr;

	uint				world_index = UINT32_MAX;

	std::vector<CRenderableObject*>
						objects;
	std::vector<RrLogicObject*>
						logics;
};

//	class RrOutputInfo : Defines an output's properties
// Does not provide any logic, simply provides information.
class RrOutputInfo
{
public:

	RrOutputInfo ( RrWorld* world, RrWindow* window )
	{
		this->world = world;
		output_window = window;

		type = Type::kWindow;
	}

public:
	enum class Type
	{
		kUinitialized,
		kWindow,
		kRenderTarget,
	};

	arstring64			name;
	bool				enabled = true;
	// Update interval normally
	int					update_interval = -1;
	// Update interval when the view isn't focused. Only valid for Type::kRenderTarget.
	int					update_interval_when_not_focused = -1;

	// World this output shows
	RrWorld*			world = nullptr;
	// Camera used for rendering this world
	RrCamera*			camera = nullptr;
	
	// Is the viewport scaled up to the output each frame?
	bool				scale_viewport_to_output = true;
	// If scale_viewport_to_output is false, provides the viewport.
	rrViewport			viewport;

	uint8				backbuffer_count = 3;
	rrBufferChainInfo	requested_buffers;

	// Output type - window or render target
	Type				type = Type::kUinitialized;
	// Output window if type is Window
	RrWindow*			output_window = nullptr;
	// Output target if type is RenderTarget
	RrRenderTexture*	output_target = nullptr;

public:

	//	GetOutputSize() : Returns the desired output size for the given output
	RENDER_API Vector2i		GetOutputSize ( void ) const;

	//	GetRenderTarget() : Returns the render target for the output
	RENDER_API gpu::RenderTarget*
							GetRenderTarget ( void ) const;

	//	GetOutputViewport() : Returns the size of the output viewport.
	RENDER_API rrViewport	GetOutputViewport ( void ) const
	{
		if (scale_viewport_to_output)
		{
			//return rrViewport{.corner=Vector2i(0, 0), .size=GetOutputSize(), .pixel_density=viewport.pixel_density};
			return rrViewport{Vector2i(0, 0), GetOutputSize(), viewport.pixel_density};
		}
		else
		{
			return viewport;
		}
	}
};

//	class RrOutputState : State for the output views.
// Persistent between frames.
class RrOutputState
{
public:
	explicit				RrOutputState ( void )
		{}
							~RrOutputState ( void )
	{
		ResizeBufferChain(0);
		FreeContexts();
	}

	void					Update ( RrOutputInfo* output_info );
	
private:
	void					ResizeBufferChain ( uint sizing );
	void					FreeContexts ( void );

public:
	RrOutputInfo*		output_info = nullptr;
	// Counter used for updating the output.
	int					update_interval_counter = 0;

	Vector2i			output_size;
	bool				first_frame_after_creation = false;

	// List of buffer targets that are used to render to.
	std::vector<RrHybridBufferChain>
						internal_chain_list;
	// Current buffer target being recorded/rendered to.
	RrHybridBufferChain*
						internal_chain_current = nullptr;
	// Index of the buffer target being recorded/rendered to.
	uint				internal_chain_index = 0;

	// These are per output:
	// Per-pass constant buffers. See renderer::cbuffer:rrPerPass.
	// Each group of kRenderLayer_MAX refers to a layer used in a buffer target of internal_chain_list.
	// Ex: If kRenderLayer_MAX is 7, and the engine is using triple (3) buffering, there will be 21 cbuffers.
	std::vector<gpu::Buffer>
						internal_cbuffers_passes;

public:
	// Current pipeline mode used for rendering.
	renderer::ePipelineMode
						pipeline_mode = renderer::kPipelineModeNormal;

public:
	gpu::GraphicsContext*
						graphics_context = nullptr;
};

//	rrRenderRequest : Render request
struct rrRenderRequest
{
	CRenderableObject*	obj = nullptr;
	uint8_t				pass = UINT8_MAX;
};

struct rrRenderRequestSorter
{
	bool operator() ( const rrRenderRequest& i, const rrRenderRequest& j );
};

// Global active instance
RENDER_API extern RrRenderer* SceneRenderer;

//	RrRenderer : main render state & swapchain manager class
class RrRenderer
{
public:
	// Structure Definitions
	// ================================

	//	rrOutputPair : Pair of output info & state
	struct rrOutputPair
	{
		RrOutputInfo		info;
		RrOutputState*		state = nullptr;
	};

public:
	// Constructor, Destructor, and Initialization
	// ================================

	RENDER_API explicit		RrRenderer ( void );
	RENDER_API				~RrRenderer ( void );

	// we want to initialize the comon stuff still here
	//	and also store the output surface
	// but we arent making the swapchain here. those are created 

private:
	void					InitializeResourcesWithDevice ( gpu::Device* device );
public:

	// Output Management
	// ================================

	//	AddOutput(info) : Adds output to be rendered
	RENDER_API uint			AddOutput ( const RrOutputInfo& info );

	//	GetOutput<Index>() : Gets output with given index.
	template <int Index>
	RrOutputInfo&			GetOutput ( void )
	{
		ARCORE_ASSERT(Index >= 0 && (size_t)(Index) < render_outputs.size());
		return render_outputs[Index].info;
	}
	//	GetOutput(Index) : Gets output with given index.
	RrOutputInfo&			GetOutput ( const uint Index )
	{
		ARCORE_ASSERT(Index >= 0 && (size_t)(Index) < render_outputs.size());
		return render_outputs[Index].info;
	}
	//	GetOutput<Index>() : Gets output with given index.
	template <int Index>
	const RrOutputInfo&		GetOutput ( void ) const
	{
		ARCORE_ASSERT(Index >= 0 && (size_t)(Index) < render_outputs.size());
		return render_outputs[Index].info;
	}
	//	GetOutput(Index) : Gets output with given index.
	const RrOutputInfo&		GetOutput ( const uint Index ) const
	{
		ARCORE_ASSERT(Index >= 0 && (size_t)(Index) < render_outputs.size());
		return render_outputs[Index].info;
	}

	//	FindOutputWithTarget(Window) : Finds output with the given window. Returns its index.
	RENDER_API uint			FindOutputWithTarget ( RrWindow* window );
	//	FindOutputWithTarget(Window) : Finds output with the given window. Returns its index.
	RENDER_API uint			FindOutputWithTarget ( RrRenderTexture* target );
	//	RemoveOutput(Index) : Removes the output with the given index.
	RENDER_API void			RemoveOutput ( const uint Index );

	// World Management
	// ================================

	//	AddWorld(world) : Adds world to the renderer for update.
	RENDER_API uint			AddWorld ( RrWorld* world );
	//	AddWorldDefault() : Adds a world with default settings to the renderer for update.
	RENDER_API uint			AddWorldDefault ( void );

	//	GetWorld<Index>() : Gets world with given index.
	template <int Index>
	RrWorld*				GetWorld ( void )
	{
		ARCORE_ASSERT(Index >= 0 && (size_t)(Index) < worlds.size());
		return worlds[Index];
	}
	//	GetWorld(Index) : Gets world with given index.
	RrWorld*				GetWorld ( const uint Index )
	{
		ARCORE_ASSERT(Index >= 0 && (size_t)(Index) < worlds.size());
		return worlds[Index];
	}

	//	RemoveWorld(Index) : Removes world with the given index.
	RENDER_API void			RemoveWorld ( const uint Index );
	//	RemoveWorld(World) : Removes given world.
	RENDER_API void			RemoveWorld ( RrWorld* world );

	// Buffer management
	// ================================

	// Creates buffers for rendering to.
	/*RENDER_API void			CreateTargetBuffers ( void );
	// Recreates buffers for the given chain. Returns success.
	//RENDER_API bool			CreateTargetBufferChain ( rrInternalBufferChain& bufferChain );
	RENDER_API gpu::RenderTarget*
							GetForwardBuffer ( void );
	RENDER_API gpu::RenderTarget*
							GetDeferredBuffer ( void );
	RENDER_API gpu::Texture*
							GetDepthTexture ( void );
	RENDER_API gpu::WOFrameAttachment*
							GetStencilTexture ( void );*/


	// Public Render routine
	// ================================

	// Performs internal render loop
	RENDER_API void			Render ( void );

	// object state update
	void					StepPreRender ( void );
	void					StepBufferPush ( gpu::GraphicsContext* gfx, const RrOutputInfo& output, RrOutputState* state );
	void					StepPostRender ( void );


	// Full Scene Rendering Routines
	// ================================

	void					RenderOutput ( gpu::GraphicsContext* gfx, const RrOutputInfo& output, RrOutputState* state, RrWorld* world );

	//	RenderScene () : Renders the main scene from the given camera with DeferredForward+.
	// Is shorthand for ``RenderObjectList(m_renderableObjects)``.
	//RENDER_API void			RenderScene ( RrCamera* camera );
	//	RenderObjectList () : Renders the object list from the given camera with DeferredForward+.
	//RENDER_API void			RenderObjectList ( RrCamera* camera, CRenderableObject** objectsToRender, const uint32_t objectCount );

	RENDER_API void			RenderObjectListWorld ( gpu::GraphicsContext* gfx, rrCameraPass* cameraPass, CRenderableObject** objectsToRender, const uint32_t objectCount, RrOutputState* state );
	//RENDER_API void			RenderObjectListShadows ( rrCameraPass* cameraPass, CRenderableObject** objectsToRender, const uint32_t objectCount );

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
	//RENDER_API void			SetPipelineMode ( renderer::ePipelineMode newPipelineMode );


	// Rendering configuration
	// ================================

	// Returns the material used for rendering a screen's pass in the given effect
	RENDER_API RrPass*		GetScreenMaterial ( const eRenderMode mode, const renderer::ePipelineMode mode_type );


	// Settings and query
	// ================================

	// Returns internal settings that govern the current render setup
	//RENDER_API const renderer::rrInternalSettings&
	//						GetSettings ( void ) const;
	// Returns if shadows are enabled for the renderer or not.
	//RENDER_API const bool	GetShadowsEnabled ( void ) const;
	// Returns current pipeline information
	//RENDER_API const eRenderMode
	//						GetRenderMode ( void ) const;
	// Returns current pipeline mode (previously, special render type)
	//RENDER_API const renderer::ePipelineMode
	//						GetPipelineMode ( void ) const;

private:
	//bool bSpecialRender_ResetLights;
	//std::vector<RrLight*> vSpecialRender_LightList;

public:
	// Public active instance pointer
	RENDER_API static RrRenderer* Active;

	// Device & context
	//gpu::Device*			mDevice;
	//gpu::OutputSurface*		mOutputSurface;
	//gpu::GraphicsContext*	mGfxContext;
	//gpu::ComputeContext*	mComputeContext;
private:
	gpu::Device*		gpu_device;
public:
	gpu::Device*			GetGpuDevice ( void ) const
		{ return gpu_device; }

private:
	// Renderable object access and management
	// ================================

	// Objects queued to add to the default world
	std::vector<CRenderableObject*> objects_to_add;
	// Logics queued to add to the default world
	std::vector<RrLogicObject*> logics_to_add;

	void					AddQueuedToWorld ( void );

public:
	class Listings
	{
	private:
		friend CRenderableObject;
		friend RrLogicObject;

		static void				AddToUnsorted ( CRenderableObject* object, rrId& out_id )
		{
			SceneRenderer->objects_to_add.push_back(object);
			out_id = rrId();
			out_id.object_index = (uint16)(SceneRenderer->objects_to_add.size() - 1);
			out_id.world_index = rrId::kWorldInvalid;
		}
		static void				RemoveFromUnsorted ( CRenderableObject* object, const rrId& id )
		{
			if (SceneRenderer->objects_to_add.back() == object)
			{
				ARCORE_ASSERT(id.object_index == (SceneRenderer->objects_to_add.size() - 1));
				SceneRenderer->objects_to_add.pop_back();
			}
			else
			{
				ARCORE_ASSERT(SceneRenderer->objects_to_add[id.object_index] == object);
				SceneRenderer->objects_to_add[id.object_index] = nullptr;
			}
		}

		static void				AddToUnsorted ( RrLogicObject* object, rrId& out_id )
		{
			SceneRenderer->logics_to_add.push_back(object);
			out_id = rrId();
			out_id.object_index = (uint16)(SceneRenderer->logics_to_add.size() - 1);
			out_id.world_index = rrId::kWorldInvalid;
		}
		static void				RemoveFromUnsorted ( RrLogicObject* object, const rrId& id )
		{
			if (SceneRenderer->logics_to_add.back() == object)
			{
				ARCORE_ASSERT(id.object_index == (SceneRenderer->logics_to_add.size() - 1));
				SceneRenderer->logics_to_add.pop_back();
			}
			else
			{
				ARCORE_ASSERT(SceneRenderer->logics_to_add[id.object_index] == object);
				SceneRenderer->logics_to_add[id.object_index] = nullptr;
			}
		}

		static RrWorld*			GetWorld ( const rrId& id )
		{
			if (id.world_index != rrId::kWorldInvalid)
			{
				ARCORE_ASSERT(id.world_index < SceneRenderer->worlds.size());
				return SceneRenderer->worlds[id.world_index];
			}
			return nullptr;
		}
	};

	/*
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
	*/
private:
	//
	// ================================
	
	// list of outputs
	std::vector<rrOutputPair>
						render_outputs;

	// list of worlds
	std::vector<RrWorld*>
						worlds;

	// Render list
	// ================================

	// these go to the world
	//std::vector<CRenderableObject*>
	//						pRenderableObjects;
	//unsigned int			iCurrentIndex;
	//unsigned int			iListSize;

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

	
	rrRenderRequestSorter RenderRequestSorter;

	// Render state
	// ================================
	//RrCamera*				mainBufferCamera;
	//bool					bufferedMode;
	//bool					shadowMode;
	//eRenderMode				renderMode;
	//renderer::ePipelineMode	pipelineMode;

	// Logic list
	// ================================
	//std::vector<RrLogicObject*>
	//						mLogicObjects;
	//unsigned int			mLoCurrentIndex;
	//unsigned int			mLoListSize;

	// Internal setup state
	// ================================
	//renderer::rrInternalSettings
	//						internal_settings; // WHERE ARE THESE INITIALIZED TO LMAO

	uint8					backbuffer_count = 3;
	int						frame_index = 0;

	// List of buffer targets that are used to render to.
	//std::vector<RrHybridBufferChain>
	//						internal_chain_list;
	// Current buffer target being recorded/rendered to.
	//RrHybridBufferChain*	internal_chain_current;
	// Index of the buffer target being recorded/rendered to.
	//uint					internal_chain_index;

	// These are per frame:
	// Per-frame constant buffers. See renderer::cbuffer::rrPerFrame.
	// Each index refers directly to a buffer target of internal_chain_list.
	// Ex: If the engine is using triple (3) buffering, there will be three (3) cbuffers.
	std::vector<gpu::Buffer>
							internal_cbuffers_frames;

	// These are per output:
	// Per-pass constant buffers. See renderer::cbuffer:rrPerPass.
	// Each group of kRenderLayer_MAX refers to a layer used in a buffer target of internal_chain_list.
	// Ex: If kRenderLayer_MAX is 7, and the engine is using triple (3) buffering, there will be 21 cbuffers.
	//std::vector<gpu::Buffer>
	//						internal_cbuffers_passes;

	// Deferred pass materials
	// ================================
	renderer::pipeline::RrPipelinePasses*
							pipelinePasses;

};

#endif//RENDERER_RENDER_STATE_SYSTEM_H_