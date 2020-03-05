#ifndef RENDERER_PIPELINE_PASSES_H_
#define RENDERER_PIPELINE_PASSES_H_

#include "core/types.h"
#include "gpuw/Buffers.h"
#include "gpuw/Pipeline.h"

class RrPass;

namespace renderer {
namespace pipeline
{
	class RrPipelinePasses
	{
	public:
								RrPipelinePasses ( void );
								~RrPipelinePasses ( void );

	public:

		RrPass*				CopyScaled;
		RrPass*				LightingPass;
		RrPass*				EchoPass;
		RrPass*				ShaftPass;
		RrPass*				Lighting2DPass;

		gpu::Pipeline		m_pipelineScreenQuadCopy;
		gpu::Buffer			m_vbufScreenQuad;
		gpu::Buffer			m_vbufScreenQuad_ForOutputSurface; // Per-API flips
	};
}}

#endif//RENDERER_PIPELINE_PASSES_H_