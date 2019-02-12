#ifndef RENDERER_PIPELINE_PASSES_H_
#define RENDERER_PIPELINE_PASSES_H_

#include "core/types.h"
#include "renderer/gpuw/Buffers.h"
#include "renderer/gpuw/Pipeline.h"

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
	};
}}

#endif//RENDERER_PIPELINE_PASSES_H_