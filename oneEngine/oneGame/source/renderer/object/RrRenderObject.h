//===============================================================================================//
//
//	class RrRenderObject
//
// base class for all objects that need to be rendered by the device
//
//===============================================================================================//

#ifndef C_RENDERABLE_OBJECT_
#define C_RENDERABLE_OBJECT_

// Includes
#include "core/types/types.h"
#include "core/math/Vector3.h"
#include "core-ext/transform/Transform.h"

#include "core/containers/araccessor.h"

//#include "GLCommon.h"
//#include "glMainSystem.h"
//#include "renderer/material/RrMaterial.h"

#include "renderer/camera/CameraPass.h"

#include "renderer/types/RrObjectMaterialProperties.h"
#include "renderer/types/ObjectSettings.h"
#include "renderer/types/id.h"

#include "renderer/material/RrPass.h"
#include "gpuw/Pipeline.h"
#include "gpuw/Buffers.h"

//#include "RrCamera.h"

// Class prototypes
class RrRenderer;
class RrPass;
class RrCamera;
class RrWorld;
namespace renderer
{
	class Material;
}
struct rrRenderRequestSorter;

typedef void (*rrMaterialRenderFunction)(renderer::Material*);

// Defines
#ifndef RegisterRenderClassName
#	define RegisterRenderClassName(_strname) public: virtual string GetTypeName ( void ) { return string(_strname); };
#endif
#ifndef RegisterRenderBaseClass
#	define RegisterRenderBaseClass(_strname) public: virtual string GetBaseClassName ( void ) { return string(_strname); };
#endif
#ifndef delete_safe
#	define delete_safe(_ptr) { if ( _ptr ) { delete (_ptr); (_ptr) = NULL; } };
#endif
#ifndef delete_safe_array
#	define delete_safe_array(_ptr) { if ( _ptr ) { delete[] (_ptr); (_ptr) = NULL; } };
#endif

// Base class, virtual/abstract
class RrRenderObject
{
public:

	//	rrRenderParams
	// Provided as input to all functions render routine.
	struct rrRenderParams
	{
		int8_t			pass;
		gpu::Buffer*	cbuf_perPass = nullptr;
		gpu::Buffer*	cbuf_perFrame = nullptr;
		gpu::Buffer*	cbuf_perCamera = nullptr;
		gpu::GraphicsContext*
						context_graphics = nullptr;
	};

private:
	// No copying with "="
	RrRenderObject & operator= (const RrRenderObject & other) =delete;

public:
	RENDER_API explicit		RrRenderObject ( void );
	RENDER_API virtual		~RrRenderObject ( void );

	//	AddToWorld(world) : Adds the given object to a world before initialization.
	// This cannot be called after an object is already in a world.
	RENDER_API void			AddToWorld ( RrWorld* world );

	//	IsInstantiatable() : Can this instance be instantiated?
	// Not intended to be overriden by the user manually. Instead, use the DEFINE_RENDER_CLASS() macro. (TODO: macro doesn't yet exist)
	RENDER_API virtual bool	IsInstantiatable ( void ) { return false; }

public:
	// 
	// Rendering Prototypes
	// If any of the rendering functions return false, the renderer will either throw an exception to attempt to debug break.

	//	PreRender() : Called before the internal render-loop executes.
	// Can be called multiple times per frame, but generally only once per camera.
	// Use to calculate transformation matrices w/ the given camera before sending to the GPU.
	RENDER_API virtual bool	PreRender ( rrCameraPass* cameraPass )
		{ return true; }
	//	Render(const rrRenderParams* params) : Current pass
	RENDER_API virtual bool	Render ( const rrRenderParams* params ) =0;
	//	BeginRender() : Called before the render-loop executes, outside of the world loop.
	// Called once per frame.
	RENDER_API virtual bool	BeginRender ( void )
		{ return true; }
	//	EndRender() : Called after the render-loop executes, outside of the world loop.
	// Called once per frame.
	RENDER_API virtual bool	EndRender ( void )
		{ return true; }

	// 
	// Setters

	// Change visible state
	RENDER_API virtual void	SetVisible ( const bool nextState )
		{ visible = nextState; }

	//
	// Getters

	// Returns visible state
	RENDER_API virtual bool	GetVisible ( void ) const
		{ return visible; }

	//
	// Object Passes
	
	//	PassInitWithInput(pass, passData) : Sets up a new pass on the given slot.
	// Creates a copy of passData without changing reference counts.
	RENDER_API void			PassInitWithInput ( int pass, RrPass* passData );
	//	PassFree(pass) : Cleans up and removes resources used by a pass.
	RENDER_API void			PassFree ( int pass );
	//	PassGetSurface(pass) : Get the writable surface options.
	// This marks the surface as edited, which forces the cbuffer to update next frame.
	RENDER_API renderer::cbuffer::rrPerObjectSurface&
							PassGetSurface ( int pass );
	//	PassAccess(pass) : Get the safe accessor for the pass.
	// Depending on access, various elements may mark the surface as edited.
	RENDER_API RrPass::SafeAccessor
							PassAccess ( int pass );

	//	PassesFree() : Cleans up and removes resources used by all passes.
	RENDER_API void			PassesFree ( void );

	RENDER_API bool			PassEnabled ( int pass )
	{
		ARCORE_ASSERT(pass >= 0 && pass < kPass_MaxPassCount);
		return m_passEnabled[pass];
	}
	RENDER_API renderer::rrRenderLayer
							PassLayer ( int pass )
	{
		ARCORE_ASSERT(pass >= 0 && pass < kPass_MaxPassCount);
		return m_passes[pass].m_layer;
	}
	RENDER_API rrPassType	PassType ( int pass )
	{
		ARCORE_ASSERT(pass >= 0 && pass < kPass_MaxPassCount);
		return m_passes[pass].m_type;
	}
	RENDER_API bool			PassDepthWrite ( int pass )
	{
		ARCORE_ASSERT(pass >= 0 && pass < kPass_MaxPassCount);
		return m_passes[pass].m_depthWrite;
	}

public:
	// Positional transform
	core::Transform			transform;
	// miscelleneous render settings
	renderer::objectSettings
							renderSettings;		
	RrObjectMaterialProperties
							shaderConstants;

	// Distance from the render pivot
	float					renderDistance;

private:
	friend rrRenderRequestSorter;

	RrPass					m_passes [kPass_MaxPassCount];
	bool					m_passEnabled [kPass_MaxPassCount];
	bool					m_passSurfaceSynced [kPass_MaxPassCount];

	gpu::Pipeline			m_pipelines [kPass_MaxPassCount];
	bool					m_pipelineReady [kPass_MaxPassCount];

protected:
	friend renderer::Material; // Give access to the utility binding class. TODO: Improve this hack.

	bool					visible;

	gpu::Buffer				m_cbufPerObjectMatrices;
	gpu::Buffer				m_cbufPerObjectSurfaces [kPass_MaxPassCount];
public:
	//	GetCbufferPerObjectMatrices() : Get the cbuffer used for the object matrices. Use for external shader binding.
	RENDER_API const gpu::Buffer&
							GetCbufferPerObjectMatrices ( void )
		{ return m_cbufPerObjectMatrices; }
	//	GetCbufferPerObjectSurfaces(pass) : Get the cbuffer used for the pass's surface params for the given pass. Use for external shader binding.
	RENDER_API const gpu::Buffer&
							GetCbufferPerObjectSurfaces ( int passIndex )
		{ return m_cbufPerObjectSurfaces[passIndex]; }
protected:
	// ==Render Setup==

	//	GetPipeline(pass) : Creates a pipeline if needed, and returns it.
	// The engine attempts to track some pass changes, and will recreate a pipeline if needed. Generally, this is not recommended.
	RENDER_API gpu::Pipeline*
							GetPipeline ( const uchar pass );
	//	FreePipelines() : Frees all allocated pipelines.
	RENDER_API void			FreePipelines ( void );


	RENDER_API void			PushCbufferPerObject ( const XrTransform& worldTransform, const rrCameraPass* cameraPass );

	// ==Render Status==
	RENDER_API float		GetRenderDistance ( void );

private:
	rrId id;
public:
	const rrId& GetId ( void ) const
		{ return id; }

	ARACCESSOR_PRIVATE_EXCLUSIVE_SET(RrRenderer, RrRenderObject, rrId, id);
	ARACCESSOR_PRIVATE_EXCLUSIVE_SET(RrWorld, RrRenderObject, rrId, id);
};

// typedef for persons coming from Unity
typedef RrRenderObject RenderObject;

#endif//C_RENDERABLE_OBJECT_