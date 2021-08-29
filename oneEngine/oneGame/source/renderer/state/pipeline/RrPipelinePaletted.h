#ifndef RENDERER_STATE_PIPELINE_PALETTED_H_
#define RENDERER_STATE_PIPELINE_PALETTED_H_

#include "renderer/state/pipeline/RrPipelineStandard.h"

class RrPipelinePalettedRenderer : public RrPipelineStandardRenderer
{
public:
	RENDER_API virtual		~RrPipelinePalettedRenderer ( void );

	//	IsCompatible()
	// Is this state compatible with the given pipeline? If not, it is destroyed and correct one is created.
	RENDER_API virtual bool	IsCompatible ( const renderer::PipelineMode mode ) const override
		{ return mode == renderer::PipelineMode::kPaletted; }

	//	CompositeDeferred() : Called when the renderer wants to combine a deferred pass with a forward pass.
	RENDER_API virtual rrCompositeOutput
							CompositeDeferred ( gpu::GraphicsContext* gfx, const rrPipelineCompositeInput& compositeInput, RrOutputState* state ) override;

	//	CompositePostOpaques() : Called when the renderer is done rendering all deferred+forward opaques.
	RENDER_API virtual rrCompositeOutput
							CompositePostOpaques ( gpu::GraphicsContext* gfx, const rrPipelinePostOpaqueCompositeInput& compositeInput, RrOutputState* state ) override;

	//	RenderLayerEnd() : Called when the renderer finishes a given layer.
	RENDER_API virtual rrPipelineOutput
							RenderLayerEnd ( gpu::GraphicsContext* gfx, const rrPipelineLayerFinishInput& finishInput, RrOutputState* state ) override;

private:
	RrShaderProgram*	m_postprocessOutlineProgram = nullptr;
	gpu::Pipeline*		m_postprocessOutlinePipeline;

	RrShaderProgram*	m_postprocessPalettizeProgram = nullptr;
	gpu::Pipeline*		m_postprocessPalettizePipeline;

protected:
	RENDER_API gpu::Texture	ApplyOutline (
		gpu::GraphicsContext* gfx,
		gpu::Texture* color,
		gpu::Texture* depth,
		rrCameraPass* cameraPass,
		RrOutputState* state );

	RENDER_API gpu::Texture	ApplyPalettize (
		gpu::GraphicsContext* gfx,
		gpu::Texture* color,
		rrCameraPass* cameraPass,
		RrOutputState* state );
};

class RrPipelinePalettedOptions : public RrPipelineStandardOptions
{
public:
	RENDER_API virtual		~RrPipelinePalettedOptions ( void );

	//	IsCompatible()
	// Are these options compatible with the given pipeline? If not, assumes no pipeline options.
	RENDER_API virtual bool	IsCompatible ( const renderer::PipelineMode mode ) const override
		{ return mode == renderer::PipelineMode::kPaletted; }

	//	LoadPalette( resource_name )
	// Loads the given PNG palette from the resource, creating a 3D texture.
	RENDER_API void			LoadPalette ( const char* resource_name );

public:
	gpu::Texture		m_paletteLUT_Primary;
	gpu::Texture		m_paletteLUT_Secondary;
};

#endif//RENDERER_STATE_PIPELINE_PALETTED_H_