#ifndef GPU_WRAPPER_PIPELINE_H_
#define GPU_WRAPPER_PIPELINE_H_

#include "core/types/types.h"
#include "renderer/types/types.h"
#include "renderer/gpuw/Public/Formats.h"
#include "renderer/gpuw/Public/Enums.h"

namespace gpu
{
	class ShaderPipeline;
	class Device;
	class GraphicsContext;
	class ComputeContext;
	class Buffer;

	enum InputRate
	{
		kInputRatePerVertex,
		kInputRatePerInstance,
	};

	struct VertexInputBindingDescription
	{
		// binding index, the vbuffer goes here
		uint32_t    binding;
		// byte offset to the next element
		uint32_t	stride;
		// input increment rate for the element
		InputRate	inputRate;
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
		// Pixel shader outputs
	};

	// defines a state of shaders, vertex formats to bind to shader inputs, etc.
	class Pipeline
	{
	public:
		//	Constructor : creates uninitalized GPU wrapper object.
		//RENDER_API explicit		Pipeline ( void );
		//	Destructor : destroys any allocated VAO/Layout, if existing.
		//RENDER_API 				~Pipeline ( void );

		// Sets up with default values
		RENDER_API explicit		Pipeline ( void )
		{
			m_vao = 0;
			m_pipeline = NULL;
		}

		//RENDER_API void			setShaderPipeline
		//RENDER_API int			setShaderPipeline ( ShaderPipeline* pipeline );

		//RENDER_API bool			assemble ( void );
		RENDER_API int			create ( Device* device, const PipelineCreationDescription* params );

		RENDER_API int			destroy ( Device* device );

		//	valid() : is this pipeline valid to be used?
		// If the pipeline failed to be created or doesnt exist, this will be false
		RENDER_API bool			valid ( void );
		//	nativePtr() : returns native index or pointer to the resource.
		RENDER_API gpuHandle	nativePtr ( void );

	private:
		friend GraphicsContext;
		friend ComputeContext;

		unsigned int		m_vao;
		ShaderPipeline*		m_pipeline;
		Buffer*				m_boundIndexBuffer;
	};
}

#endif//GPU_WRAPPER_PIPELINE_H_