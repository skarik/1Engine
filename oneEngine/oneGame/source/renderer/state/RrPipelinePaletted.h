#ifndef RENDERER_STATE_PIPELINE_PALETTED_H_
#define RENDERER_STATE_PIPELINE_PALETTED_H_

#include "renderer/state/RrPipeline.h"

class RrPipelinePalettedRenderer : public RrPipelineStandardRenderer
{
public:
	//	IsCompatible()
	// Is this state compatible with the given pipeline? If not, it is destroyed and correct one is created.
	RENDER_API bool			IsCompatible ( const renderer::PipelineMode mode ) const override
		{ return mode == renderer::PipelineMode::kPaletted; }
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