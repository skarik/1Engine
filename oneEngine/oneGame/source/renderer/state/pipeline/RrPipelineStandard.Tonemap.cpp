#include "RrPipelineStandard.h"

#include "core-ext/settings/SessionSettings.h"

#include "renderer/light/RrLight.h"
#include "renderer/object/RrRenderObject.h"
#include "renderer/material/RrShaderProgram.h"
#include "renderer/camera/RrCamera.h"
#include "renderer/state/RrRenderer.h"
#include "renderer/types/shaders/cbuffers.h"
#include "renderer/types/shaders/sbuffers.h"

#include "renderer/state/RaiiHelpers.h"

#include "gpuw/Pipeline.h"
#include "gpuw/ShaderPipeline.h"
#include "gpuw/WriteableResource.h"
#include "gpuw/Sampler.h"

RrPipelineStandardRenderer::rrTonemapSetup
RrPipelineStandardRenderer::SetupTonemap (
		gpu::GraphicsContext* gfx,
		gpu::Texture* input_color,
		RrOutputState* state)
{
	return rrTonemapSetup();
}

RrPipelineStandardRenderer::rrExposureSetup
RrPipelineStandardRenderer::SetupExposure (
		gpu::GraphicsContext* gfx,
		rrPreviousFrameOutput* reference_frame,
		RrOutputState* state)
{
	return rrExposureSetup();
}