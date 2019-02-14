#ifndef RENDERER_SHADER_VERTEX_ATTRIBUTE_H_
#define RENDERER_SHADER_VERTEX_ATTRIBUTE_H_

#include <stdint.h>
#include "renderer/types/shaders/vattribs.h"
#include "gpuw/Pipeline.h" // TODO?
#include "gpuw/Public/Enums.h"

namespace renderer
{
	namespace shader
	{
		// see gpuw/Pipeline.h for more information on vertex binding
		// specifically structs VertexInputBindingDescription and VertexInputAttributeDescription.
		// the data here is used to generate those two structs as input to pipelines

		enum class InputRate
		{
			PerVertex	= gpu::kInputRatePerVertex,
			PerInstance	= gpu::kInputRatePerInstance,
		};

		struct VertexAttribute
		{
			// location in the shader to bind to
			Location	location;
			// binding index, the vbuffer goes here
			// corresponds to a binding description. 
			// gfx->bindVertexBuffer(N, ...) will generally be binding N.
			uint32_t	binding;
			// byte offset of where the data starts in the buffer
			uint32_t	dataOffset;
			// data format of each element
			gpu::Format	dataFormat;
			// byte offset to the next element
			uint32_t	dataStride;
			// increment rate for the element (when is dataStride added to the pointer)
			InputRate	dataInputRate;

			// Default value constructor
			VertexAttribute() : 
				location(Location::kInvalid),
				binding(0xFFFFFFFF),
				dataOffset(0xFFFFFFFF),
				dataFormat(gpu::kFormatUndefined),
				dataStride(0xFFFFFFFF),
				dataInputRate(InputRate::PerVertex)
				{}
		};
	}
}

#endif//RENDERER_SHADER_VERTEX_ATTRIBUTE_H_