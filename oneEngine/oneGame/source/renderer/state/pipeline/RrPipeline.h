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

struct rrPipelineCompositeInput
{
	gpu::Texture*		deferred_albedo = nullptr;
	gpu::Texture*		deferred_normals = nullptr;
	gpu::Texture*		deferred_surface = nullptr;
	gpu::Texture*		deferred_emissive = nullptr;

	gpu::Texture*		combined_depth = nullptr;

	gpu::Texture*		forward_color = nullptr;

	rrCameraPass*		cameraPass = nullptr;
	//gpu::Texture*		output_color = nullptr;
};

struct rrPipelineLayerFinishInput
{
	renderer::rrRenderLayer
						layer = renderer::kRenderLayerSkip;

	gpu::Texture*		color = nullptr;
	gpu::Texture*		depth = nullptr;

	//gpu::Texture*		output_color = nullptr;
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

	//	CompositeDeferred() : Called when the renderer wants to combine a deferred pass with a forward pass.
	RENDER_API virtual rrCompositeOutput
							CompositeDeferred ( gpu::GraphicsContext* gfx, const rrPipelineCompositeInput& compositeInput, RrOutputState* state )
		{ return rrCompositeOutput{*(gpu::Texture*)compositeInput.forward_color}; }

	//	RenderLayerEnd() : Called when the renderer finishes a given layer.
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