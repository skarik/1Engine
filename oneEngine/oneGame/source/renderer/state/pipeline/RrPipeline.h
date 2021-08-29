#ifndef RENDERER_STATE_PIPELINE_H_
#define RENDERER_STATE_PIPELINE_H_

#include "core/types.h"
#include "renderer/state/pipeline/PipelineModes.h"
#include "renderer/types/ObjectSettings.h"

#include "gpuw/Texture.h"

#include <functional>

class RrWorld;
class RrOutputInfo;
class RrOutputState;
class RrShaderProgram;
namespace gpu
{
	class GraphicsContext;
	class Texture;
	class RenderTarget;
	class Pipeline;
}
class RrRenderer;
struct rrCameraPass;

class RrPipelineStateRenderer;
class RrPipelineOptions;

//=====================================
// Pipeline instantiators
//=====================================

namespace renderer
{
	// Create a pipeline renderer for the given mode.
	RENDER_API RrPipelineStateRenderer*
							CreatePipelineRenderer ( const renderer::PipelineMode mode );
	// Create pipeline options for the given mode.
	RENDER_API RrPipelineOptions*
							CreatePipelineOptions ( const renderer::PipelineMode mode );
}

//=====================================
// Helper structures
//=====================================

struct rrPipelinePostDepthInput
{
	renderer::rrRenderLayer
						layer = renderer::kRenderLayerSkip;

	gpu::Texture*		combined_depth = nullptr;

	rrCameraPass*		cameraPass = nullptr;
};

struct rrPipelineCompositeInput
{
	renderer::rrRenderLayer
						layer = renderer::kRenderLayerSkip;

	gpu::Texture*		deferred_albedo = nullptr;
	gpu::Texture*		deferred_normals = nullptr;
	gpu::Texture*		deferred_surface = nullptr;
	gpu::Texture*		deferred_emissive = nullptr;

	gpu::Texture*		combined_depth = nullptr;

	gpu::Texture*		old_forward_color = nullptr;

	rrCameraPass*		cameraPass = nullptr;
};

struct rrPipelinePostOpaqueCompositeInput
{
	renderer::rrRenderLayer
						layer = renderer::kRenderLayerSkip;

	gpu::Texture*		combined_color = nullptr;
	gpu::Texture*		combined_normals = nullptr;
	gpu::Texture*		combined_depth = nullptr;

	rrCameraPass*		cameraPass = nullptr;
};

struct rrPipelineLayerFinishInput
{
	renderer::rrRenderLayer
						layer = renderer::kRenderLayerSkip;

	gpu::Texture*		color = nullptr;
	gpu::Texture*		depth = nullptr;

	rrCameraPass*		cameraPass = nullptr;
};

struct rrCompositeOutput
{
	gpu::Texture		color;
};

struct rrPipelineOutput
{
	// TODO: maybe....
	gpu::Texture		color;
};

//=====================================
// Common pipeline information
//=====================================

class RrPipelineStateRenderer
{
public:
	RENDER_API explicit		RrPipelineStateRenderer ( void )
		{}
	RENDER_API virtual		~RrPipelineStateRenderer ( void )
		{}

	//	IsCompatible()
	// Is this state compatible with the given pipeline? If not, it is destroyed and correct one is created.
	RENDER_API virtual bool	IsCompatible ( const renderer::PipelineMode mode ) const = 0;

	//	PrepareOptions(options) : Set the options for the next frame to be rendered.
	RENDER_API virtual void	PrepareOptions ( const RrPipelineOptions* options )
	{
		m_options = options;
	}

	//	CullObjects() : Called to cull objects.
	RENDER_API virtual void	CullObjects ( gpu::GraphicsContext* gfx, const RrOutputInfo& output, RrOutputState* state, RrWorld* world ) 
		{}

	//	PreparePass()
	// Called before a world begins to render.
	RENDER_API virtual void	PreparePass ( gpu::GraphicsContext* gfx )
		{}

	//	PostDepth()
	// Called before a world begins to render, but after the depth pre-pass has finished.
	// Will always be called even if there are no opaque objects in the scene.
	RENDER_API virtual void	PostDepth ( gpu::GraphicsContext* gfx, const rrPipelinePostDepthInput& postDepthInput, RrOutputState* state )
		{}

	//	CompositeDeferred() : Called when the renderer wants to combine a deferred pass with a forward pass.
	// Should only handle lighting or effectively opaque rendering effects.
	// Will not be called if there are no deferred-rendered objects in the scene.
	RENDER_API virtual rrCompositeOutput
							CompositeDeferred ( gpu::GraphicsContext* gfx, const rrPipelineCompositeInput& compositeInput, RrOutputState* state )
		{ return rrCompositeOutput{*(gpu::Texture*)compositeInput.old_forward_color}; }

	//	CompositePostOpaques() : Called when the renderer is done rendering all deferred+forward opaques.
	// Can be used to apply effects to deferred+forward opaques before rendering translucents.
	// Will not be called if there are no opaque objects in the scene.
	RENDER_API virtual rrCompositeOutput
							CompositePostOpaques ( gpu::GraphicsContext* gfx, const rrPipelinePostOpaqueCompositeInput& compositeInput, RrOutputState* state )
		{ return rrCompositeOutput{*(gpu::Texture*)compositeInput.combined_color}; }

	//	RenderLayerEnd() : Called when the renderer finishes a given layer.
	// Will always be called even if the layer is empty.
	RENDER_API virtual rrPipelineOutput
							RenderLayerEnd ( gpu::GraphicsContext* gfx, const rrPipelineLayerFinishInput& finishInput, RrOutputState* state )
		{ return rrPipelineOutput{*(gpu::Texture*)finishInput.color}; }

public:

	const RrPipelineOptions*
						m_options = nullptr;
};

class RrPipelineOptions
{
public:
	RENDER_API explicit		RrPipelineOptions ( void )
		{}
	RENDER_API virtual		~RrPipelineOptions ( void )
		{}

	//	IsCompatible()
	// Are these options compatible with the given pipeline? If not, assumes no pipeline options.
	RENDER_API virtual bool	IsCompatible ( const renderer::PipelineMode mode ) const = 0;
};

#endif//RENDERER_STATE_PIPELINE_H_