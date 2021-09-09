#ifndef RENDERER_STATE_PIPLEINE_STANDARD_H_
#define RENDERER_STATE_PIPLEINE_STANDARD_H_

#include "renderer/state/pipeline/RrPipeline.h"

//=====================================
// Standard renderer
//=====================================

#include <vector>
class RrLight;
class RrPipelineStandardRenderer : public RrPipelineStateRenderer
{
public:
	RENDER_API				RrPipelineStandardRenderer ( void );
	RENDER_API virtual		~RrPipelineStandardRenderer ( void );

	//	IsCompatible()
	// Is this state compatible with the given pipeline? If not, it is destroyed and correct one is created.
	RENDER_API virtual bool	IsCompatible ( const renderer::PipelineMode mode ) const override
	{ return mode == renderer::PipelineMode::kNormal; }

	//	CullObjects() : Called to cull objects.
	RENDER_API virtual void	CullObjects ( rrRenderContext* context, const RrOutputInfo& output, RrOutputState* state, RrWorld* world ) override;

	//	PostDepth()
	// Called before a world begins to render.
	RENDER_API virtual void	PostDepth ( rrRenderContext* context, const rrPipelinePostDepthInput& postDepthInput, RrOutputState* state ) override;

	//	CompositeDeferred() : Called when the renderer wants to combine a deferred pass with a forward pass.
	RENDER_API virtual rrCompositeOutput
							CompositeDeferred ( rrRenderContext* context, const rrPipelineCompositeInput& compositeInput, RrOutputState* state ) override;

	//	CompositePostOpaques() : Called when the renderer wants to combine deferred+forward opaques with forward translucents.
	RENDER_API virtual rrCompositeOutput
							CompositePostOpaques ( rrRenderContext* context, const rrPipelinePostOpaqueCompositeInput& compositeInput, RrOutputState* state ) override;

	//	RenderLayerEnd() : Called when the renderer finishes a given layer.
	RENDER_API virtual rrPipelineOutput
							RenderLayerEnd ( rrRenderContext* context, const rrPipelineLayerFinishInput& finishInput, RrOutputState* state ) override;

private:
	RrShaderProgram*	m_lightingCompositeProgram = nullptr;
	gpu::Pipeline*		m_lightingCompositePipeline;

	RrShaderProgram*	m_lightingLighting0Program = nullptr;
	gpu::Pipeline*		m_lightingLighting0Pipeline;

	RrShaderProgram*	m_lightingLightingOmniProgram = nullptr;
	gpu::Pipeline*		m_lightingLightingOmniPipeline;

	RrShaderProgram*	m_lightingLightingSpotProgram = nullptr;
	gpu::Pipeline*		m_lightingLightingSpotPipeline;

	RrShaderProgram*	m_hzbGenerationProgram = nullptr;

	RrShaderProgram*	m_shadowingProjectionProgram = nullptr;
	RrShaderProgram*	m_shadowingContactShadowProgram = nullptr;

	RrShaderProgram*	m_bloomDownscaleProgram = nullptr;
	RrShaderProgram*	m_bloomBlurProgram = nullptr;

	RrShaderProgram*	m_postprocessBloomProgram = nullptr;
	gpu::Pipeline*		m_postprocessBloomPipeline;

	RrShaderProgram*	m_postprocessTonemapProgram = nullptr;
	gpu::Pipeline*		m_postprocessTonemapPipeline;

protected:
	struct rrLightSetup
	{
		int				directionalLightFirstIndex = 0;
		int				spotLightFirstIndex = 0;
		int				omniLightFirstIndex = 0;
		gpu::Buffer*	lightParameterBuffer = nullptr;
	};

	// TODO: organize this properly. light management likely should be outside of RrPipeline.cpp as it grows larger
	void					SortLights ( void );
	std::vector<RrLight*> directional_lights;
	std::vector<RrLight*> spot_lights;
	std::vector<RrLight*> omni_lights;
	std::vector<RrLight*> all_lights;

	std::vector<gpu::Texture> shadow_masks;

	// TODO: better organize this if possible.
	gpu::Texture hzb_4;
	gpu::Texture hzb_16;

	struct rrPreviousFrameOutput
	{
		gpu::Texture	m_color;
	};
	rrPreviousFrameOutput
						m_previousFrameOutput;

	struct rrBloomSetup
	{
		gpu::Texture	m_colorDownscale4;
		gpu::Texture	m_colorDownscale16;

		gpu::Texture	m_colorDownscale4_Blurred;
		gpu::Texture	m_colorDownscale16_Blurred;
	};
	struct rrTonemapSetup
	{
	};
	struct rrExposureSetup
	{
	};

protected:
	RENDER_API void			CopyRenderTexture (
		gpu::Texture* output_copy,
		gpu::Texture* input_texture,
		gpu::GraphicsContext* gfx,
		RrOutputState* state);

	RENDER_API virtual void	DrawWithPipelineAndGBuffers (
		rrRenderContext* context,
		const rrPipelineCompositeInput& compositeInput,
		gpu::Pipeline* pipeline,
		gpu::Buffer* cbuffer,
		gpu::Buffer* sbuffer,
		std::function<void(RrRenderer*, rrRenderContext*)> renderCall);

	RENDER_API virtual void	DrawDebugOutput (
		rrRenderContext* context,
		const rrPipelineCompositeInput& compositeInput,
		RrOutputState* state,
		gpu::Texture clearedOutputTexture);

	RENDER_API virtual void GenerateHZB (
		rrRenderContext* context,
		gpu::Texture* combined_depth,
		rrCameraPass* cameraPass,
		RrOutputState* state);

	RENDER_API rrLightSetup	SetupLights ( rrRenderContext* context );

	RENDER_API virtual void	RenderShadows (
		rrRenderContext* context,
		gpu::Texture* deferred_normals,
		gpu::Texture* combined_depth,
		rrCameraPass* cameraPass,
		RrOutputState* state,
		rrLightSetup* lightSetup);

	RENDER_API virtual gpu::Texture
							RenderLights (
		rrRenderContext* context,
		const rrPipelineCompositeInput& gbuffers,
		RrOutputState* state,
		rrLightSetup* lightSetup,
		gpu::Texture clearedOutputTexture);

	RENDER_API virtual rrBloomSetup
							SetupBloom (
		rrRenderContext* context,
		gpu::Texture* input_color,
		rrCameraPass* cameraPass,
		RrOutputState* state);
	RENDER_API virtual rrTonemapSetup
							SetupTonemap (
		rrRenderContext* context,
		gpu::Texture* input_color,
		RrOutputState* state);
	RENDER_API virtual rrExposureSetup
							SetupExposure (
		rrRenderContext* context,
		rrPreviousFrameOutput* reference_frame,
		RrOutputState* state);

	RENDER_API virtual gpu::Texture
							ApplyTonemap (
		rrRenderContext* context,
		gpu::Texture* input_color,
		rrBloomSetup* bloom_setup,
		rrTonemapSetup* tonemap_setup,
		rrExposureSetup* exposure_setup,
		rrCameraPass* cameraPass,
		RrOutputState* state);

	RENDER_API virtual gpu::Texture
							ApplyBloom (
		rrRenderContext* context,
		gpu::Texture* input_color,
		rrBloomSetup* bloom_setup,
		rrTonemapSetup* tonemap_setup,
		rrExposureSetup* exposure_setup,
		rrCameraPass* cameraPass,
		RrOutputState* state);

	RENDER_API virtual void	SaveAndAnalyzeOutput (
		rrRenderContext* context,
		gpu::Texture* final_output_color,
		RrOutputState* state);
};

class RrPipelineStandardOptions : public RrPipelineOptions
{
public:
	//	IsCompatible()
	// Are these options compatible with the given pipeline? If not, assumes no pipeline options.
	RENDER_API bool			IsCompatible ( const renderer::PipelineMode mode ) const override
		{ return mode == renderer::PipelineMode::kNormal; }

public:
	bool				m_celShadeLighting = false;
};

#endif//RENDERER_STATE_PIPLEINE_STANDARD_H_