#ifndef RENDERER_STATE_PIPELINE_PALETTED_H_
#define RENDERER_STATE_PIPELINE_PALETTED_H_

#include "renderer/state/pipeline/RrPipelineStandard.h"

class RrPipelinePalettedRenderer : public RrPipelineStandardRenderer
{
public:
							~RrPipelinePalettedRenderer ( void );

	//	IsCompatible()
	// Is this state compatible with the given pipeline? If not, it is destroyed and correct one is created.
	RENDER_API bool			IsCompatible ( const renderer::PipelineMode mode ) const override
		{ return mode == renderer::PipelineMode::kPaletted; }

	//	CompositeDeferred() : Called when the renderer wants to combine a deferred pass with a forward pass.
	RENDER_API rrCompositeOutput
							CompositeDeferred ( gpu::GraphicsContext* gfx, const rrPipelineCompositeInput& compositeInput, RrOutputState* state ) override;

private:
	RrShaderProgram*	m_postprocessOutlineProgram = nullptr;
	gpu::Pipeline*		m_postprocessOutlinePipeline;
};

class RrPipelinePalettedOptions : public RrPipelineStandardOptions
{
public:
	//	IsCompatible()
	// Are these options compatible with the given pipeline? If not, assumes no pipeline options.
	RENDER_API bool			IsCompatible ( const renderer::PipelineMode mode ) const override
		{ return mode == renderer::PipelineMode::kPaletted; }
};

#endif//RENDERER_STATE_PIPELINE_PALETTED_H_