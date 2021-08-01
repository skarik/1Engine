#ifndef GPU_WRAPPER_PUBLIC_PIPELINE_H_
#define GPU_WRAPPER_PUBLIC_PIPELINE_H_

#include "core/types/types.h"
#include "gpuw/Public/Formats.h"
#include "gpuw/Public/Enums.h"

namespace gpu
{
	class ShaderPipeline;

	enum InputRate
	{
		kInputRatePerVertex,
		kInputRatePerInstance,

		kInputRateInvalid,
	};

	struct VertexInputBindingDescription
	{
		// binding index, the vbuffer goes here
		uint32_t    binding;
		// byte offset to the next element
		uint32_t	stride;
		// input increment rate for the element
		InputRate	inputRate = kInputRateInvalid;
	};

	struct VertexInputAttributeDescription
	{
		// location in the shader to bind to
		uint32_t    location;
		// binding index, the vbuffer goes here
		// corresponds to a binding description
		uint32_t    binding;
		// attribute format.
		// extra values will be ignored
		// missing values in VS stage will be padded GBA -> 0,0,1
		Format		format;
		// byte offset of the data
		uint32_t    offset;
	};

	struct PipelineCreationDescription
	{
		// Shader pipeline to be used with this pipeline.
		ShaderPipeline*		shader_pipeline;
		// Vertex shader inputs
		VertexInputBindingDescription*
							vv_inputBindings;
		uint32_t			vv_inputBindingsCount;
		VertexInputAttributeDescription*
							vv_inputAttributes;
		uint32_t			vv_inputAttributesCount;
		// Input assembler outputs
		PrimitiveTopology	ia_topology;
		bool				ia_primitiveRestartEnable;
		// Pixel shader outputs (TODO, for MRT)
	};
}

#endif//GPU_WRAPPER_PUBLIC_PIPELINE_H_