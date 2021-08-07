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
#include "renderer/state/PipelineModes.h"

//Todo: make following prototypes
#include "gpuw/Texture.h"
#include "gpuw/RenderTarget.h"
#include "gpuw/Device.h"
#include "gpuw/GraphicsContext.h"
#include "gpuw/Buffers.h"
#include "gpuw/Pipeline.h"

#include <vector>

class RrRenderObject;
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

class RrPipelineStateRenderer;
class RrPipelineOptions;

//	class RrWorld : A container of objects that can be rendered.
class RrWorld
{
public:
	explicit				RrWorld ( void )
		{}
							~RrWorld ( void )
		{}

	friend RrRenderObject;
	rrId					AddObject ( RrRenderObject* renderable );
	bool					RemoveObject ( RrRenderObject* renderable );
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
	renderer::PipelineMode
						pipeline_mode = renderer::PipelineMode::kNormal;
	RrPipelineOptions*	pipeline_options = nullptr;

	uint				world_index = UINT32_MAX;

	std::vector<RrRenderObject*>
						objects;
	std::vector<RrLogicObject*>
						logics;
};

struct rrRenderFrameState
{
	gpu::Buffer			cbuffer_perFrame;
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
		FreeContexts();
	}

	void					Update ( RrOutputInfo* output_info, rrRenderFrameState* frame_state );
	
private:
	void					FreeContexts ( void );

public:
	RrOutputInfo*		output_info = nullptr;
	// Counter used for updating the output.
	int					update_interval_counter = 0;

	Vector2i			output_size;
	bool				first_frame_after_creation = false;

	// Render info and structures for the given frame.
	rrRenderFrameState*	frame_state;

public:
	// Current pipeline mode used for rendering.
	renderer::PipelineMode
						pipeline_mode = renderer::PipelineMode::kNormal;
	// Pipeline state renderer. Used for setting up additional pass information.
	RrPipelineStateRenderer*
						pipeline_renderer = nullptr;

public:
	gpu::GraphicsContext*
						graphics_context = nullptr;
};

//
struct rrDepthBufferRequest
{
	Vector2i			size = Vector2i(0, 0);

	core::gfx::tex::arColorFormat
						depth = core::gfx::tex::kDepthFormat32;
	core::gfx::tex::arColorFormat
						stencil = core::gfx::tex::KStencilFormatIndex16;

	// Number of frames this request should persist for
	int32				persist_for = 2;
};

//
struct rrRTBufferRequest
{
	Vector2i			size = Vector2i(0, 0);

	core::gfx::tex::arColorFormat
						format = core::gfx::tex::kColorFormatRGBA16F;

	// Number of frames this request should persist for
	int32				persist_for = 2;
};

//
struct rrMRTBufferRequest
{
	Vector2i			size = Vector2i(0, 0);

	uint8				count;
	const core::gfx::tex::arColorFormat*
						formats = nullptr;

	// Number of frames this request should persist for
	int32				persist_for = 2;
};

//
struct rrStoredRenderTexture
{
	Vector2i			size;
	int					frame_of_request;
	int					persist_for;
	core::gfx::tex::arColorFormat
						format;
	gpu::Texture		texture;
};

//
struct rrStoredRenderDepthTexture
{
	Vector2i			size;
	int					frame_of_request;
	int					persist_for;
	core::gfx::tex::arColorFormat
						depth_format;
	core::gfx::tex::arColorFormat
						stencil_format;
	gpu::Texture		depth_texture;
	gpu::WOFrameAttachment
						stencil_texture;
};

//	rrRenderRequest : Render request
struct rrRenderRequest
{
	RrRenderObject*	obj = nullptr;
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

private:
	void					InitializeResourcesWithDevice ( gpu::Device* device );
	void					InitializeCommonPipelineResources ( gpu::Device* device );
	void					FreeCommonPipelineResources ( gpu::Device* device );
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

	// Public Render routine
	// ================================

	// Performs internal render loop
	RENDER_API void			Render ( void );

	// object state update
	void					StepPreRender ( rrRenderFrameState* frameState );
	void					StepBufferPush ( gpu::GraphicsContext* gfx, const RrOutputInfo& output, RrOutputState* state, gpu::Texture texture );
	void					StepPostRender ( void );

	// Full Scene Rendering Routines
	// ================================

	gpu::Texture			RenderOutput ( gpu::GraphicsContext* gfx, const RrOutputInfo& output, RrOutputState* state, RrWorld* world );

	RENDER_API gpu::Texture	RenderObjectListWorld ( gpu::GraphicsContext* gfx, rrCameraPass* cameraPass, RrRenderObject** objectsToRender, const uint32_t objectCount, RrOutputState* state );

	// Specialized Rendering Routines
	// ================================

	// Rendering configuration
	// ================================

	// Resource creation and management
	// ================================

	//	CreatePipeline() : Creates a screen-quad pipeline for the given shader pipeline.
	RENDER_API void			CreatePipeline ( gpu::ShaderPipeline* in_pipeline, gpu::Pipeline& out_pipeline );

private:
	gpu::Pipeline		m_pipelineScreenQuadCopy;
	gpu::Buffer			m_vbufScreenQuad;
	gpu::Buffer			m_vbufScreenQuad_ForOutputSurface; // Per-API flips

	gpu::Buffer			m_vbufDefault;

public:
	RENDER_API const gpu::Pipeline&
							GetScreenQuadCopyPipeline ( void )
		{ return m_pipelineScreenQuadCopy; }
	RENDER_API const gpu::Buffer&
							GetScreenQuadVertexBuffer ( void )
		{ return m_vbufScreenQuad; }
	RENDER_API const gpu::Buffer&
							GetScreenQuadOutputVertexBuffer ( void )
		{ return m_vbufScreenQuad_ForOutputSurface; }

	RENDER_API const gpu::Buffer&
							GetDefaultVertexBuffer ( void )
		{ return m_vbufDefault; }

public:
	// Public active instance pointer
	RENDER_API static RrRenderer* Active;

public:

	RENDER_API void			CreateRenderTexture ( rrDepthBufferRequest* in_out_request, gpu::Texture* depth, gpu::WOFrameAttachment* stencil );
	RENDER_API void			CreateRenderTexture ( const rrRTBufferRequest& in_request, gpu::Texture* color );
	RENDER_API void			CreateRenderTextures ( const rrMRTBufferRequest& in_request, gpu::Texture* colors );

private:
	void					UpdateResourcePools ( void );

private:
	std::vector<rrStoredRenderTexture>
						m_renderTexturePool;
	std::vector<rrStoredRenderDepthTexture>
						m_renderDepthTexturePool;
	rrDepthBufferRequest
						m_currentDepthBufferRequest;

private:
	gpu::Device*		gpu_device;
public:
	gpu::Device*			GetGpuDevice ( void ) const
		{ return gpu_device; }

private:
	// Renderable object access and management
	// ================================

	// Objects queued to add to the default world
	std::vector<RrRenderObject*>
						objects_to_add;
	// Logics queued to add to the default world
	std::vector<RrLogicObject*>
						logics_to_add;

	void					AddQueuedToWorld ( void );

public:
	class Listings
	{
	private:
		friend RrRenderObject;
		friend RrLogicObject;

		static void				AddToUnsorted ( RrRenderObject* object, rrId& out_id )
		{
			SceneRenderer->objects_to_add.push_back(object);
			out_id = rrId();
			out_id.object_index = (uint16)(SceneRenderer->objects_to_add.size() - 1);
			out_id.world_index = rrId::kWorldInvalid;
		}
		static void				RemoveFromUnsorted ( RrRenderObject* object, const rrId& id )
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

	// Render list sorting
	// ================================
	
	rrRenderRequestSorter RenderRequestSorter;

	// Render state
	// ================================

	// Logic list
	// ================================

	// Internal setup state
	// ================================
	uint8					backbuffer_count = 3;
	int						frame_index = 0;

	
	// Deferred pass materials
	// ================================
};

#endif//RENDERER_RENDER_STATE_SYSTEM_H_